#include "edge_solver.h"
#include <algorithm>

EdgeSolver::EdgeSolver(int xSize, int ySize, int stepLimit)
{
	this->xSize = xSize;
	this->ySize = ySize;

	if(stepLimit == 0)	this->stepLimit = 2000000000;
	else				this->stepLimit = stepLimit;

	totalSize = xSize * ySize;

	edgeIndexesLength = 2*(xSize-2) + 2*(ySize-2);
	cornerIndexesLength = 4;
	centerIndexesLength = (xSize-2) * (ySize-2);
}


void EdgeSolver::createIndexes()
{
	edgeIndexes = new int[edgeIndexesLength];
	cornerIndexes = new int[cornerIndexesLength];
	centerIndexes = new int[centerIndexesLength];

	int iEdge = 0, iCorner = 0, iCenter = 0;

	for(int i = 0; i < totalSize; i++)
	{
		int walls = 0;

		if(original->GetUp(i) == WALL) walls++;
		if(original->GetDown(i) == WALL) walls++;
		if(original->GetLeft(i) == WALL) walls++;
		if(original->GetRight(i) == WALL) walls++;

		if(walls == 0)
		{
			centerIndexes[iCenter++] = i;
		}

		else if(walls == 1)
		{
			edgeIndexes[iEdge++] = i;
		}

		else
		{
			cornerIndexes[iCorner++] = i;
		}
	}

	if(iCenter + iEdge + iCorner != totalSize)
		cout << "Something has gone awry..." << endl;
}

int EdgeSolver::Solve(Board* b)
{
	bool ready = false;
	WALL = 0;
	int steps = 0;
	original = b;

	createIndexes();

	usedEdges = new bool[edgeIndexesLength];
	usedCorners = new bool[cornerIndexesLength];
	usedIndexByLocation = new int[totalSize];

	Board board;
	board.Init(b->eternityPieces, b->pieceIdsByLocation, b->pieceLocationsById, b->rotations, totalSize, xSize, ySize);

	for(int i = 0; i < totalSize; i++)
	{
		if(i < cornerIndexesLength)	usedCorners[i] = false;
		if(i < edgeIndexesLength)	usedEdges[i] = false;
		usedIndexByLocation[i] = -1;
	}

	int iBoard = 0;
	int iEdge = 0;
	int iCorner = 0;
	int rot = 0;
	int pieceID = 0;

	while (!ready)
	{
		if(++steps > stepLimit && stepLimit > 0)
			break;

		// Ollaan vas. yläkulmassa
		if (iBoard == 0)
		{
			for (int i = (usedIndexByLocation[iBoard] == -1 ? 0 : usedIndexByLocation[iBoard]); i < cornerIndexesLength; i++)
			{
				board.pieceIdsByLocation[iBoard] = original->pieceIdsByLocation[cornerIndexes[i]]; board.rotations[iBoard] = original->rotations[cornerIndexes[i]];

				while (board.GetLeft(iBoard) != WALL || board.GetUp(iBoard) != WALL)
					board.Rotate(iBoard);

				usedIndexByLocation[iBoard] = i;
				usedCorners[i] = true;
				iBoard++;

				break;
			}
		}

		// Ollaan yläreunassa
		else if (iBoard < xSize - 1)
		{
			for (int i = usedIndexByLocation[iBoard] == -1 ? 0 : usedIndexByLocation[iBoard]; i < edgeIndexesLength; i++)
			{
				if (!usedEdges[i])
				{
					board.pieceIdsByLocation[iBoard] = original->pieceIdsByLocation[edgeIndexes[i]]; board.rotations[iBoard] = original->rotations[edgeIndexes[i]];

					while (board.GetUp(iBoard) != WALL)
						board.Rotate(iBoard);

					if (board.GetRight(iBoard - 1) == board.GetLeft(iBoard))
					{
						if (usedIndexByLocation[iBoard] != -1) usedEdges[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = i;
						iBoard++;
						usedEdges[i] = true;
						break;
					}
				}

				if (i == edgeIndexesLength - 1)
				{
					if (usedIndexByLocation[iBoard] != -1)
					{
						usedEdges[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = -1;
					}

					iBoard--;
				}
			}
		}

		// Oikea alakulma
		else if (iBoard == totalSize - 1)
		{
			for (int i = usedIndexByLocation[iBoard] == -1 ? 0 : usedIndexByLocation[iBoard]; i < cornerIndexesLength; i++)
			{
				if (!usedCorners[i])
				{
					board.pieceIdsByLocation[iBoard] = original->pieceIdsByLocation[cornerIndexes[i]]; board.rotations[iBoard] = original->rotations[cornerIndexes[i]];

					while (board.GetDown(iBoard) != WALL || board.GetRight(iBoard) != WALL)
						board.Rotate(iBoard);

					if (board.GetRight(iBoard - 1) == board.GetLeft(iBoard) && board.GetDown(iBoard - xSize) == board.GetUp(iBoard))
					{
						if (usedIndexByLocation[iBoard] != -1) usedCorners[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = i;
						iBoard++;
						usedCorners[i] = true;
						ready = true;
						break;
					}
				}

				if (i == cornerIndexesLength - 1)
				{
					if (usedIndexByLocation[iBoard] != -1)
					{
						usedCorners[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = -1;
					}

					iBoard--;
				}
			}
		}

		// Alareuna
		else if (iBoard > totalSize - xSize)
		{
			for (int i = usedIndexByLocation[iBoard] == -1 ? 0 : usedIndexByLocation[iBoard]; i < edgeIndexesLength; i++)
			{
				if (!usedEdges[i])
				{
					board.pieceIdsByLocation[iBoard] = original->pieceIdsByLocation[edgeIndexes[i]]; board.rotations[iBoard] = original->rotations[edgeIndexes[i]];

					while (board.GetDown(iBoard) != WALL)
						board.Rotate(iBoard);
						
					if (board.GetRight(iBoard - 1) == board.GetLeft(iBoard))
					{
						if (usedIndexByLocation[iBoard] != -1) usedEdges[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = i;
						iBoard++;
						usedEdges[i] = true;
						break;
					}
				}

				if (i == edgeIndexesLength - 1)
				{
					if (usedIndexByLocation[iBoard] != -1)
					{
						usedEdges[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = -1;
					}

					iBoard--;
				}
			}
		}

		// Oikea yläkulma
		else if (iBoard == xSize - 1)
		{
			for (int i = usedIndexByLocation[iBoard] == -1 ? 0 : usedIndexByLocation[iBoard]; i < cornerIndexesLength; i++)
			{
				if (!usedCorners[i])
				{
					board.pieceIdsByLocation[iBoard] = original->pieceIdsByLocation[cornerIndexes[i]]; board.rotations[iBoard] = original->rotations[cornerIndexes[i]];

					while (board.GetUp(iBoard) != WALL || board.GetRight(iBoard) != WALL)
						board.Rotate(iBoard);

					if (board.GetRight(iBoard - 1) == board.GetLeft(iBoard))
					{
						if (usedIndexByLocation[iBoard] != -1) usedCorners[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = i;
						iBoard++;
						usedCorners[i] = true;
						break;
					}
				}

				if (i == cornerIndexesLength - 1)
				{
					if (usedIndexByLocation[iBoard] != -1)
					{
						usedCorners[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = -1;
					}

					iBoard--;
				}
			}
		}

		// Vasen alakulma
		else if (iBoard == totalSize - xSize)
		{
			for (int i = usedIndexByLocation[iBoard] == -1 ? 0 : usedIndexByLocation[iBoard]; i < cornerIndexesLength; i++)
			{
				if (!usedCorners[i])
				{
					board.pieceIdsByLocation[iBoard] = original->pieceIdsByLocation[cornerIndexes[i]]; board.rotations[iBoard] = original->rotations[cornerIndexes[i]];

					while (board.GetDown(iBoard) != WALL || board.GetLeft(iBoard) != WALL)
						board.Rotate(iBoard);

					if (board.GetDown(iBoard - xSize) == board.GetUp(iBoard))
					{
						if (usedIndexByLocation[iBoard] != -1) usedCorners[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = i;
						iBoard++;
						usedCorners[i] = true;
						break;
					}
				}

				if (i == cornerIndexesLength - 1)
				{
					if (usedIndexByLocation[iBoard] != -1)
					{
						usedCorners[usedIndexByLocation[iBoard]] = false;
						usedIndexByLocation[iBoard] = -1;
					}

					iBoard--;
				}
			}
		}

		// Vasen ja oikea reuna
		else
		{
			// Vasen reuna
			if (iBoard % xSize == 0)
			{
				for (int i = usedIndexByLocation[iBoard] == -1 ? 0 : usedIndexByLocation[iBoard]; i < edgeIndexesLength; i++)
				{
					if (!usedEdges[i])
					{
						board.pieceIdsByLocation[iBoard] = original->pieceIdsByLocation[edgeIndexes[i]]; board.rotations[iBoard] = original->rotations[edgeIndexes[i]];

						while (board.GetLeft(iBoard) != WALL)
							board.Rotate(iBoard);

						if (board.GetDown(iBoard - xSize) == board.GetUp(iBoard))
						{
							if (usedIndexByLocation[iBoard] != -1) usedEdges[usedIndexByLocation[iBoard]] = false;
							usedIndexByLocation[iBoard] = i;
							usedEdges[i] = true;
							iBoard += xSize - 1;
							break;
						}
					}

					if (i == edgeIndexesLength - 1)
					{
						if (usedIndexByLocation[iBoard] != -1)
						{
							usedEdges[usedIndexByLocation[iBoard]] = false;
							usedIndexByLocation[iBoard] = -1;
						}

						iBoard--;
					}
				}
			}

			// Oikea reuna
			else
			{
				for (int i = usedIndexByLocation[iBoard] == -1 ? 0 : usedIndexByLocation[iBoard]; i < edgeIndexesLength; i++)
				{
					if (!usedEdges[i])
					{
						board.pieceIdsByLocation[iBoard] = original->pieceIdsByLocation[edgeIndexes[i]]; board.rotations[iBoard] = original->rotations[edgeIndexes[i]];

						while (board.GetRight(iBoard) != WALL)
							board.Rotate(iBoard);

						if (board.GetDown(iBoard - xSize) == board.GetUp(iBoard))
						{
							if (usedIndexByLocation[iBoard] != -1) usedEdges[usedIndexByLocation[iBoard]] = false;
							usedIndexByLocation[iBoard] = i;
							iBoard++;
							usedEdges[i] = true;
							break;
						}
					}

					if (i == edgeIndexesLength - 1)
					{
						if (usedIndexByLocation[iBoard] != -1)
						{
							usedEdges[usedIndexByLocation[iBoard]] = false;
							usedIndexByLocation[iBoard] = -1;
						}

						iBoard -= xSize - 1;
					}
				}
			}
		}
	}

	if(steps >= stepLimit)
		return -1;

	int centerIndex = 0;

	for (int i = 0; i < totalSize; i++)
	{
		if ((i > xSize && i < totalSize - xSize - 1) && (i % xSize != 0 && (i + 1) % xSize != 0))
			board.pieceIdsByLocation[i] = original->pieceIdsByLocation[centerIndexes[centerIndex++]];

		board.pieceLocationsById[board.pieceIdsByLocation[i]] = i;
	}
	
	for (int i = 0; i < totalSize; i++)
	{
		b->pieceIdsByLocation[i] = board.pieceIdsByLocation[i];
		b->pieceLocationsById[i] = board.pieceLocationsById[i];
		b->rotations[i] = board.rotations[i];
	}
	
	return steps;
}