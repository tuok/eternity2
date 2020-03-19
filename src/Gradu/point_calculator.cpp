#include "point_calculator.h"

PointCalculator::PointCalculator(Board* board, int xsize, int ysize, double maxSidePoints, double maxSquarePoints, double maxCrossPoints, double maxEdgePoints, bool edgesAsPoints)
{
	xSize = xsize;
	ySize = ysize;
	totalSize = xsize * ysize;
	this->maxSidePoints = maxSidePoints;
	this->maxSquarePoints = maxSquarePoints;
	this->maxCrossPoints = maxCrossPoints;
	this->maxEdgePoints = maxEdgePoints;
	this->edgesAsPoints = edgesAsPoints;
	this->populationSize = 1;
	this->obj1Weight = 0;
	this->obj2Weight = 0;
	this->obj3Weight = 0;
	this->obj4Weight = 0;
	this->totalWeight = 0.0;

	WALL = 0;

	initSingleLocations();

	style = SingleBoard;
}

PointCalculator::PointCalculator(Board** population, int xsize, int ysize, int populationsize, double maxSidePoints, double maxSquarePoints, double maxCrossPoints, double maxEdgePoints, bool edgesAsPoints)
{
	xSize = xsize;
	ySize = ysize;
	totalSize = xsize * ysize;
	populationSize = populationsize;
	this->maxSidePoints = maxSidePoints;
	this->maxSquarePoints = maxSquarePoints;
	this->maxCrossPoints = maxCrossPoints;
	this->maxEdgePoints = maxEdgePoints;
	this->edgesAsPoints = edgesAsPoints;
	this->obj1Weight = 0;
	this->obj2Weight = 0;
	this->obj3Weight = 0;
	this->obj4Weight = 0;
	this->totalWeight = 0.0;

	WALL = 0;

	initSingleLocations();

	style = SingleArray;
}

void PointCalculator::initSingleLocations()
{
	maxSingleSidePoints = 4.0;
	maxSingleCrossPoints = 4.0;

	singleLocations = new Location[totalSize];

	for (int i = 0; i < totalSize; i++)
	{
		// U/UL/UR
		if (i < xSize)
		{
			if (i == 0)
				singleLocations[i] = UL;

			else if (i == xSize - 1)
				singleLocations[i] = UR;

			else
				singleLocations[i] = U;
		}


		// D/DL/DR
		else if (i > totalSize - xSize - 1)
		{
			if (i == totalSize - xSize)
				singleLocations[i] = DL;

			else if (i == totalSize - 1)
				singleLocations[i] = DR;

			else
				singleLocations[i] = D;
		}

		// L/C/R
		else
		{
			if (i % xSize == 0)
				singleLocations[i] = L;

			else if (i % xSize == xSize - 1)
				singleLocations[i] = R;

			else
				singleLocations[i] = C;
		}
	}
}

PointCalculator::~PointCalculator()
{
	if(singleLocations != 0)
		delete [] singleLocations;

	singleLocations = 0;
}

void PointCalculator::Calculate(Board* board)
{
	calculateBoard(board);
	calculateCombinedPoints(board);
}

void PointCalculator::Calculate(Board **population)
{
	calculatePopulation(population);		
	calculateCombinedPointsPopulation(population);
}

double PointCalculator::CalculateIndex(Board *board, int i)
{
	calculateSingleIndex(board, i);
	return calculateCombinedPointsForSingleIndex();
}

double PointCalculator::CalculateAdjacentIndex(Board* board, int i, int dir)
{
	calculateSingleAdjacentIndex(board, i, dir);
	return calculateCombinedPointsForSingleIndex();
}

void PointCalculator::calculatePopulation(Board **population)
{
	for(int i = 0; i < populationSize; i++)
		calculateBoard(population[i]);
}

void PointCalculator::calculateCombinedPointsPopulation(Board **population)
{
	for(int i = 0; i < populationSize; i++)
		calculateCombinedPoints(population[i]);
}

void PointCalculator::calculateBoard(Board *board)
{
	board->objectives[Constants::SidePoints] = 0;
	board->objectives[Constants::CrossPoints] = 0;
	board->objectives[Constants::SquarePoints] = 0;
	board->objectives[Constants::EdgePoints] = 0;
	board->combinedPoints = 0;

	for (int i = 0; i < totalSize; i++)
	{
		switch (singleLocations[i])
		{
			// TODO: Optimize point calculation by adding edge points (by one or two) if SidePoints equals 4 with edgesAsPoints, OR if SidePoints equals 2 to 3 without edgesAsPoints
			case C:
				
				if (board->GetUp(i) == board->GetDown(i - xSize)) board->objectives[Constants::SidePoints]++;
				if (board->GetLeft(i) == board->GetRight(i - 1)) board->objectives[Constants::SidePoints]++;
				if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))
					board->objectives[Constants::SquarePoints]++;
				if (board->GetUp(i) == board->GetDown(i - xSize) && board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetLeft(i) == board->GetRight(i - 1))
					board->objectives[Constants::CrossPoints]++;
				break;

			case U:
				if (board->GetLeft(i) == board->GetRight(i - 1)) board->objectives[Constants::SidePoints]++;
				if (edgesAsPoints && board->GetUp(i) == WALL) board->objectives[Constants::SidePoints]++;
				if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))
					board->objectives[Constants::SquarePoints]++;
				if (board->GetUp(i) == WALL && board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetLeft(i) == board->GetRight(i - 1))
					board->objectives[Constants::CrossPoints]++;
				if (board->GetUp(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				break;

			case D:
				if (board->GetUp(i) == board->GetDown(i - xSize)) board->objectives[Constants::SidePoints]++;
				if (board->GetLeft(i) == board->GetRight(i - 1)) board->objectives[Constants::SidePoints]++;
				if (edgesAsPoints && board->GetDown(i) == WALL) board->objectives[Constants::SidePoints]++;
				if (board->GetUp(i) == board->GetDown(i - xSize) && board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == WALL && board->GetLeft(i) == board->GetRight(i - 1))
					board->objectives[Constants::CrossPoints]++;
				if (board->GetDown(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				break;

			case L:
				if (board->GetUp(i) == board->GetDown(i - xSize)) board->objectives[Constants::SidePoints]++;
				if (edgesAsPoints && board->GetLeft(i) == WALL) board->objectives[Constants::SidePoints]++;
				if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))
					board->objectives[Constants::SquarePoints]++;
				if (board->GetUp(i) == board->GetDown(i - xSize) && board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetLeft(i) == WALL)
					board->objectives[Constants::CrossPoints]++;
				if (board->GetLeft(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				break;

			case R:
				if (board->GetUp(i) == board->GetDown(i - xSize)) board->objectives[Constants::SidePoints]++;
				if (board->GetLeft(i) == board->GetRight(i - 1)) board->objectives[Constants::SidePoints]++;
				if (edgesAsPoints && board->GetRight(i) == WALL) board->objectives[Constants::SidePoints]++;
				if (board->GetUp(i) == board->GetDown(i - xSize) && board->GetRight(i) == WALL && board->GetDown(i) == board->GetUp(i + xSize) && board->GetLeft(i) == board->GetRight(i - 1))
					board->objectives[Constants::CrossPoints]++;
				if (board->GetRight(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				break;

			case UL:
				if(edgesAsPoints)
				{
					if (board->GetUp(i) == WALL) board->objectives[Constants::SidePoints]++;
					if (board->GetLeft(i) == WALL) board->objectives[Constants::SidePoints]++;
				}
				if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))
					board->objectives[Constants::SquarePoints]++;
				if (board->GetUp(i) == WALL && board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetLeft(i) == WALL)
					board->objectives[Constants::CrossPoints]++;
				if (board->GetUp(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				if (board->GetLeft(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				break;

			case UR:
				if (board->GetLeft(i) == board->GetRight(i - 1)) board->objectives[Constants::SidePoints]++;
				if(edgesAsPoints)
				{
					if (board->GetUp(i) == WALL) board->objectives[Constants::SidePoints]++;
					if (board->GetRight(i) == WALL) board->objectives[Constants::SidePoints]++;
				}
				if (board->GetUp(i) == WALL && board->GetRight(i) == WALL && board->GetDown(i) == board->GetUp(i + xSize) && board->GetLeft(i) == board->GetRight(i - 1))
					board->objectives[Constants::CrossPoints]++;
				if (board->GetUp(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				if (board->GetRight(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				break;

			case DL:
				if (board->GetUp(i) == board->GetDown(i - xSize)) board->objectives[Constants::SidePoints]++;
				if(edgesAsPoints)
				{
					if (board->GetDown(i) == WALL) board->objectives[Constants::SidePoints]++;
					if (board->GetLeft(i) == WALL) board->objectives[Constants::SidePoints]++;
				}
				if (board->GetUp(i) == board->GetDown(i - xSize) && board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == WALL && board->GetLeft(i) == WALL)
					board->objectives[Constants::CrossPoints]++;
				if (board->GetDown(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				if (board->GetLeft(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				break;

			case DR:
				if (board->GetUp(i) == board->GetDown(i - xSize)) board->objectives[Constants::SidePoints]++;
				if (board->GetLeft(i) == board->GetRight(i - 1)) board->objectives[Constants::SidePoints]++;
				if(edgesAsPoints)
				{
					if (board->GetRight(i) == WALL) board->objectives[Constants::SidePoints]++;
					if (board->GetDown(i) == WALL) board->objectives[Constants::SidePoints]++;
				}
				if (board->GetUp(i) == board->GetDown(i - xSize) && board->GetRight(i) == WALL && board->GetDown(i) == WALL && board->GetLeft(i) == board->GetRight(i - 1))
					board->objectives[Constants::CrossPoints]++;
				if (board->GetDown(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				if (board->GetRight(i) == WALL)
					board->objectives[Constants::EdgePoints]++;
				break;

			default:
				break;
		}
	}
}

void PointCalculator::calculateSingleIndex(Board *board, int i)
{
	obj1Pts = obj2Pts = obj3Pts = obj4Pts = 0;

	//Objective1: Side points, Objective2: Square points, Objective3: Cross points, Objective4: Edge points

	switch (singleLocations[i])
	{
		case C:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 4.0;

			if (board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			if (board->GetDown(i) == board->GetUp(i + xSize)) obj1Pts++;
			if (board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;

			// Piece as top-left
			if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))	obj2Pts++;
			// Piece as top-right
			if (board->GetRight(i-1) == board->GetLeft(i) && board->GetDown(i-1) == board->GetUp(i + xSize - 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetRight(i + xSize - 1) == board->GetLeft(i + xSize))	obj2Pts++;
			// Piece as bottom-left
			if (board->GetRight(i - xSize) == board->GetLeft(i - xSize + 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetDown(i - xSize + 1) == board->GetUp(i + 1) && board->GetRight(i) == board->GetLeft(i + 1))	obj2Pts++;
			// Piece as bottom-right
			if (board->GetRight(i - xSize - 1) == board->GetLeft(i - xSize) && board->GetDown(i - xSize - 1) == board->GetUp(i - 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetRight(i - 1) == board->GetLeft(i))	obj2Pts++;

			if (obj1Pts == 4)	obj3Pts++;
			break;

		case U:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 2.0;

			if (edgesAsPoints && board->GetUp(i) == WALL) obj1Pts++;
			if (board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			if (board->GetDown(i) == board->GetUp(i + xSize)) obj1Pts++;
			if (board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;

			// Piece as top-left
			if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))	obj2Pts++;
			// Piece as top-right
			if (board->GetRight(i-1) == board->GetLeft(i) && board->GetDown(i-1) == board->GetUp(i + xSize - 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetRight(i + xSize - 1) == board->GetLeft(i + xSize))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 3 && board->GetUp(i) == WALL)	obj3Pts++;

			if(board->GetUp(i) == WALL) obj4Pts++;

			break;

		case D:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 2.0;

			if (board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			if (edgesAsPoints && board->GetDown(i) == WALL) obj1Pts++;
			if (board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;

			// Piece as bottom-left
			if (board->GetRight(i - xSize) == board->GetLeft(i - xSize + 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetDown(i - xSize + 1) == board->GetUp(i + 1) && board->GetRight(i) == board->GetLeft(i + 1))	obj2Pts++;
			// Piece as bottom-right
			if (board->GetRight(i - xSize - 1) == board->GetLeft(i - xSize) && board->GetDown(i - xSize - 1) == board->GetUp(i - 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetRight(i - 1) == board->GetLeft(i))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 3 && board->GetDown(i) == WALL)	obj4Pts++;

			if(board->GetDown(i) == WALL) obj4Pts++;

			break;

		case L:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 2.0;

			if (board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			if (board->GetDown(i) == board->GetUp(i + xSize)) obj1Pts++;
			if (edgesAsPoints && board->GetLeft(i) == WALL) obj1Pts++;

			// Piece as top-left
			if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))	obj2Pts++;
			// Piece as bottom-left
			if (board->GetRight(i - xSize) == board->GetLeft(i - xSize + 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetDown(i - xSize + 1) == board->GetUp(i + 1) && board->GetRight(i) == board->GetLeft(i + 1))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 3 && board->GetLeft(i) == WALL)	obj3Pts++;

			if(board->GetLeft(i) == WALL) obj4Pts++;

			break;

		case R:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 2.0;

			if (board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (edgesAsPoints && board->GetRight(i) == WALL) obj1Pts++;
			if (board->GetDown(i) == board->GetUp(i + xSize)) obj1Pts++;
			if (board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;
			
			// Piece as top-right
			if (board->GetRight(i-1) == board->GetLeft(i) && board->GetDown(i-1) == board->GetUp(i + xSize - 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetRight(i + xSize - 1) == board->GetLeft(i + xSize))	obj2Pts++;
			// Piece as bottom-right
			if (board->GetRight(i - xSize - 1) == board->GetLeft(i - xSize) && board->GetDown(i - xSize - 1) == board->GetUp(i - 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetRight(i - 1) == board->GetLeft(i))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 3 && board->GetRight(i) == WALL)	obj3Pts++;

			if(board->GetRight(i) == WALL) obj4Pts++;
			break;

		case UL:
			maxSingleEdgePoints = 2.0;
			maxSingleSquarePoints = 1.0;

			if (board->GetRight(i) == board->GetLeft(i + 1))	obj1Pts++;
			if (board->GetDown(i) == board->GetUp(i + xSize))	obj1Pts++;

			if(edgesAsPoints)
			{
				if (board->GetUp(i) == WALL) obj1Pts++;
				if (board->GetLeft(i) == WALL) obj1Pts++;
			}
			
			// Piece as top-left
			if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 2 && board->GetUp(i) == WALL && board->GetLeft(i) == WALL)	obj3Pts++;

			if(board->GetUp(i) == WALL) obj4Pts++;
			if(board->GetLeft(i) == WALL) obj4Pts++;

			break;

		case UR:
			maxSingleEdgePoints = 2.0;
			maxSingleSquarePoints = 1.0;

			if(edgesAsPoints)
			{
				if (board->GetUp(i) == WALL) obj1Pts++;
				if (board->GetRight(i) == WALL) obj1Pts++;
			}
			if (board->GetDown(i) == board->GetUp(i + xSize))	obj1Pts++;
			if (board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;

			// Piece as top-right
			if (board->GetRight(i-1) == board->GetLeft(i) && board->GetDown(i-1) == board->GetUp(i + xSize - 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetRight(i + xSize - 1) == board->GetLeft(i + xSize))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 2 && board->GetUp(i) == WALL && board->GetRight(i) == WALL)	obj3Pts++;
			
			if(board->GetUp(i) == WALL) obj4Pts++;
			if(board->GetRight(i) == WALL) obj4Pts++;
			break;

		case DL:
			maxSingleEdgePoints = 2.0;
			maxSingleSquarePoints = 1.0;

			if (board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			
			if(edgesAsPoints)
			{
				if (board->GetDown(i) == WALL) obj1Pts++;
				if (board->GetLeft(i) == WALL) obj1Pts++;
			}

			// Piece as bottom-left
			if (board->GetRight(i - xSize) == board->GetLeft(i - xSize + 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetDown(i - xSize + 1) == board->GetUp(i + 1) && board->GetRight(i) == board->GetLeft(i + 1))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 2 && board->GetDown(i) == WALL && board->GetLeft(i) == WALL)	obj3Pts++;

			if(board->GetDown(i) == WALL) obj4Pts++;
			if(board->GetLeft(i) == WALL) obj4Pts++;
			break;

		case DR:
			maxSingleEdgePoints = 2.0;
			maxSingleSquarePoints = 1.0;

			if (board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;
			if(edgesAsPoints)
			{
				if (board->GetRight(i) == WALL) obj1Pts++;
				if (board->GetDown(i) == WALL) obj1Pts++;
			}

			// Piece as bottom-right
			if (board->GetRight(i - xSize - 1) == board->GetLeft(i - xSize) && board->GetDown(i - xSize - 1) == board->GetUp(i - 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetRight(i - 1) == board->GetLeft(i))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 2 && board->GetDown(i) == WALL && board->GetRight(i) == WALL)	obj3Pts++;

			if(board->GetDown(i) == WALL) obj4Pts++;
			if(board->GetLeft(i) == WALL) obj4Pts++;

			break;

		default:
			break;
	}
}


void PointCalculator::calculateSingleAdjacentIndex(Board *board, int i, int dir)
{
	obj1Pts = obj2Pts = obj3Pts = obj4Pts = 0;

	//Objective1: Side points, Objective2: Square points, Objective3: Cross points, Objective4: Edge points

	switch (singleLocations[i])
	{
		case C:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 4.0;

			if (dir != Constants::UP && board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (dir != Constants::RIGHT && board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			if (dir != Constants::DOWN && board->GetDown(i) == board->GetUp(i + xSize)) obj1Pts++;
			if (dir != Constants::LEFT && board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;

			// Piece as top-left
			if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))	obj2Pts++;
			// Piece as top-right
			if (board->GetRight(i-1) == board->GetLeft(i) && board->GetDown(i-1) == board->GetUp(i + xSize - 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetRight(i + xSize - 1) == board->GetLeft(i + xSize))	obj2Pts++;
			// Piece as bottom-left
			if (board->GetRight(i - xSize) == board->GetLeft(i - xSize + 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetDown(i - xSize + 1) == board->GetUp(i + 1) && board->GetRight(i) == board->GetLeft(i + 1))	obj2Pts++;
			// Piece as bottom-right
			if (board->GetRight(i - xSize - 1) == board->GetLeft(i - xSize) && board->GetDown(i - xSize - 1) == board->GetUp(i - 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetRight(i - 1) == board->GetLeft(i))	obj2Pts++;

			if (obj1Pts == 4)	obj3Pts++;
			break;

		case U:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 2.0;

			if (edgesAsPoints && board->GetUp(i) == WALL) obj1Pts++;
			if (dir != Constants::RIGHT && board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			if (dir != Constants::DOWN && board->GetDown(i) == board->GetUp(i + xSize)) obj1Pts++;
			if (dir != Constants::LEFT && board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;

			// Piece as top-left
			if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))	obj2Pts++;
			// Piece as top-right
			if (board->GetRight(i-1) == board->GetLeft(i) && board->GetDown(i-1) == board->GetUp(i + xSize - 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetRight(i + xSize - 1) == board->GetLeft(i + xSize))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 3 && board->GetUp(i) == WALL)	obj3Pts++;

			if(board->GetUp(i) == WALL) obj4Pts++;

			break;

		case D:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 2.0;

			if (dir != Constants::UP && board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (dir != Constants::RIGHT && board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			if (edgesAsPoints && board->GetDown(i) == WALL) obj1Pts++;
			if (dir != Constants::LEFT && board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;

			// Piece as bottom-left
			if (board->GetRight(i - xSize) == board->GetLeft(i - xSize + 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetDown(i - xSize + 1) == board->GetUp(i + 1) && board->GetRight(i) == board->GetLeft(i + 1))	obj2Pts++;
			// Piece as bottom-right
			if (board->GetRight(i - xSize - 1) == board->GetLeft(i - xSize) && board->GetDown(i - xSize - 1) == board->GetUp(i - 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetRight(i - 1) == board->GetLeft(i))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 3 && board->GetDown(i) == WALL)	obj4Pts++;

			if(board->GetDown(i) == WALL) obj4Pts++;

			break;

		case L:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 2.0;

			if (dir != Constants::UP && board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (dir != Constants::RIGHT && board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			if (dir != Constants::DOWN && board->GetDown(i) == board->GetUp(i + xSize)) obj1Pts++;
			if (edgesAsPoints && board->GetLeft(i) == WALL) obj1Pts++;

			// Piece as top-left
			if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))	obj2Pts++;
			// Piece as bottom-left
			if (board->GetRight(i - xSize) == board->GetLeft(i - xSize + 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetDown(i - xSize + 1) == board->GetUp(i + 1) && board->GetRight(i) == board->GetLeft(i + 1))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 3 && board->GetLeft(i) == WALL)	obj3Pts++;

			if(board->GetLeft(i) == WALL) obj4Pts++;

			break;

		case R:
			maxSingleEdgePoints = 1.0;
			maxSingleSquarePoints = 2.0;

			if (dir != Constants::UP && board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (edgesAsPoints && board->GetRight(i) == WALL) obj1Pts++;
			if (dir != Constants::DOWN && board->GetDown(i) == board->GetUp(i + xSize)) obj1Pts++;
			if (dir != Constants::LEFT && board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;
			
			// Piece as top-right
			if (board->GetRight(i-1) == board->GetLeft(i) && board->GetDown(i-1) == board->GetUp(i + xSize - 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetRight(i + xSize - 1) == board->GetLeft(i + xSize))	obj2Pts++;
			// Piece as bottom-right
			if (board->GetRight(i - xSize - 1) == board->GetLeft(i - xSize) && board->GetDown(i - xSize - 1) == board->GetUp(i - 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetRight(i - 1) == board->GetLeft(i))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 3 && board->GetRight(i) == WALL)	obj3Pts++;

			if(board->GetRight(i) == WALL) obj4Pts++;
			break;

		case UL:
			maxSingleEdgePoints = 2.0;
			maxSingleSquarePoints = 1.0;

			if (dir != Constants::RIGHT && board->GetRight(i) == board->GetLeft(i + 1))	obj1Pts++;
			if (dir != Constants::DOWN && board->GetDown(i) == board->GetUp(i + xSize))	obj1Pts++;

			if(edgesAsPoints)
			{
				if (board->GetUp(i) == WALL) obj1Pts++;
				if (board->GetLeft(i) == WALL) obj1Pts++;
			}
			
			// Piece as top-left
			if (board->GetRight(i) == board->GetLeft(i + 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetDown(i + 1) == board->GetUp(i + xSize + 1) && board->GetRight(i + xSize) == board->GetLeft(i + xSize + 1))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 2 && board->GetUp(i) == WALL && board->GetLeft(i) == WALL)	obj3Pts++;

			if(board->GetUp(i) == WALL) obj4Pts++;
			if(board->GetLeft(i) == WALL) obj4Pts++;

			break;

		case UR:
			maxSingleEdgePoints = 2.0;
			maxSingleSquarePoints = 1.0;

			if(edgesAsPoints)
			{
				if (board->GetUp(i) == WALL) obj1Pts++;
				if (board->GetRight(i) == WALL) obj1Pts++;
			}
			if (dir != Constants::DOWN && board->GetDown(i) == board->GetUp(i + xSize))	obj1Pts++;
			if (dir != Constants::LEFT && board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;

			// Piece as top-right
			if (board->GetRight(i-1) == board->GetLeft(i) && board->GetDown(i-1) == board->GetUp(i + xSize - 1) && board->GetDown(i) == board->GetUp(i + xSize) && board->GetRight(i + xSize - 1) == board->GetLeft(i + xSize))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 2 && board->GetUp(i) == WALL && board->GetRight(i) == WALL)	obj3Pts++;
			
			if(board->GetUp(i) == WALL) obj4Pts++;
			if(board->GetRight(i) == WALL) obj4Pts++;
			break;

		case DL:
			maxSingleEdgePoints = 2.0;
			maxSingleSquarePoints = 1.0;

			if (dir != Constants::UP && board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (dir != Constants::RIGHT && board->GetRight(i) == board->GetLeft(i + 1)) obj1Pts++;
			
			if(edgesAsPoints)
			{
				if (board->GetDown(i) == WALL) obj1Pts++;
				if (board->GetLeft(i) == WALL) obj1Pts++;
			}

			// Piece as bottom-left
			if (board->GetRight(i - xSize) == board->GetLeft(i - xSize + 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetDown(i - xSize + 1) == board->GetUp(i + 1) && board->GetRight(i) == board->GetLeft(i + 1))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 2 && board->GetDown(i) == WALL && board->GetLeft(i) == WALL)	obj3Pts++;

			if(board->GetDown(i) == WALL) obj4Pts++;
			if(board->GetLeft(i) == WALL) obj4Pts++;
			break;

		case DR:
			maxSingleEdgePoints = 2.0;
			maxSingleSquarePoints = 1.0;

			if (dir != Constants::UP && board->GetUp(i) == board->GetDown(i - xSize)) obj1Pts++;
			if (dir != Constants::LEFT && board->GetLeft(i) == board->GetRight(i - 1)) obj1Pts++;
			if(edgesAsPoints)
			{
				if (board->GetRight(i) == WALL) obj1Pts++;
				if (board->GetDown(i) == WALL) obj1Pts++;
			}

			// Piece as bottom-right
			if (board->GetRight(i - xSize - 1) == board->GetLeft(i - xSize) && board->GetDown(i - xSize - 1) == board->GetUp(i - 1) && board->GetDown(i - xSize) == board->GetUp(i) && board->GetRight(i - 1) == board->GetLeft(i))	obj2Pts++;

			if (edgesAsPoints && obj1Pts == 4)	obj3Pts++;
			else if(!edgesAsPoints && obj1Pts == 2 && board->GetDown(i) == WALL && board->GetRight(i) == WALL)	obj3Pts++;

			if(board->GetDown(i) == WALL) obj4Pts++;
			if(board->GetLeft(i) == WALL) obj4Pts++;

			break;

		default:
			break;
	}
}


double PointCalculator::calculateCombinedPointsForSingleIndex()
{
	double total = 0.0;

	total += obj1Weight * (obj1Pts / maxSidePoints);
	total += obj2Weight * (obj2Pts / maxSquarePoints);
	total += obj3Weight * (obj3Pts / maxCrossPoints);
	total += obj4Weight * (obj4Pts / maxEdgePoints);

	return 1.0 * total / totalWeight;
}

void PointCalculator::SetActiveObjectives(double obj1Weight, double obj2Weight, double obj3Weight, double obj4Weight)
{
	objectiveCount = 0;

	this->obj1Weight = obj1Weight;
	this->obj2Weight = obj2Weight;
	this->obj3Weight = obj3Weight;
	this->obj4Weight = obj4Weight;

	if(obj1Weight > 0) objectiveCount++;
	if(obj2Weight > 0) objectiveCount++;
	if(obj3Weight > 0) objectiveCount++;
	if(obj4Weight > 0) objectiveCount++;

	totalWeight = obj1Weight + obj2Weight + obj3Weight + obj4Weight;
}

void PointCalculator::calculateCombinedPoints(Board *board)
{
	double total = 0.0;
	total += obj1Weight * (board->objectives[Constants::SidePoints] / maxSidePoints);
	total += obj2Weight * (board->objectives[Constants::SquarePoints] / maxSquarePoints);
	total += obj3Weight * (board->objectives[Constants::CrossPoints] / maxCrossPoints);
	total += obj4Weight * (board->objectives[Constants::EdgePoints] / maxEdgePoints);
	total = 1.0 * total / totalWeight;

	board->combinedPoints = total;
}
