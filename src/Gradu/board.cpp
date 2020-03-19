#include "board.h"

#define SINGLEDIM

Board::Board()
{
}

Board::~Board()
{
	delete [] pieceLocationsById;
	delete [] pieceIdsByLocation;
	delete [] rotations;
	delete [] objectives;
}

int Board::GetUp(int i)
{
	switch(rotations[i])
	{
		case 0: return eternityPieces[pieceIdsByLocation[i]]->UP(); break;
		case 1: return eternityPieces[pieceIdsByLocation[i]]->LEFT(); break;
		case 2: return eternityPieces[pieceIdsByLocation[i]]->DOWN(); break;
		case 3: return eternityPieces[pieceIdsByLocation[i]]->RIGHT(); break;
	}	

	return -1;
}

int Board::GetLeft(int i)
{
	switch(rotations[i])
	{
		case 0: return eternityPieces[pieceIdsByLocation[i]]->LEFT(); break;
		case 1: return eternityPieces[pieceIdsByLocation[i]]->DOWN(); break;
		case 2: return eternityPieces[pieceIdsByLocation[i]]->RIGHT(); break;
		case 3: return eternityPieces[pieceIdsByLocation[i]]->UP(); break;
	}	

	return -1;
}

int Board::GetDown(int i)
{
	switch(rotations[i])
	{
		case 0: return eternityPieces[pieceIdsByLocation[i]]->DOWN(); break;
		case 1: return eternityPieces[pieceIdsByLocation[i]]->RIGHT(); break;
		case 2: return eternityPieces[pieceIdsByLocation[i]]->UP(); break;
		case 3: return eternityPieces[pieceIdsByLocation[i]]->LEFT(); break;
	}

	return -1;
}

int Board::GetRight(int i)
{
	switch(rotations[i])
	{
		case 0: return eternityPieces[pieceIdsByLocation[i]]->RIGHT(); break;
		case 1: return eternityPieces[pieceIdsByLocation[i]]->UP(); break;
		case 2: return eternityPieces[pieceIdsByLocation[i]]->LEFT(); break;
		case 3: return eternityPieces[pieceIdsByLocation[i]]->DOWN(); break;
	}	

	return -1;
}

int Board::GetID(int i)
{
	//return eternityPieces[pieceIdsByLocation[i]]->ID();
	return pieceIdsByLocation[i];
}

void Board::Rotate(int i)
{
	if(++rotations[i] > 3) rotations[i] = 0;
}

int Board::GetRotation(int i)
{
	return rotations[i];
}

int Board::CountGaps()
{
	int gaps = 0;

	for(int i = 0; i < totalSize; i++)
		if(pieceIdsByLocation[i] == -1)
			gaps++;

	return gaps;
}

void Board::Rotate(int i, int angle)
{
	switch(angle)
	{
		case 180: rotations[i] += 2; break;
		case 270: rotations[i] += 3; break;
	}

	if(rotations[i] > 3) rotations[i] = 0;
}

// TODO: Remove when not needed anymore (l. homma toimii)
void Board::InspectBoard()
{
	CheckLocationsAndIDs();

	bool ok[256];

	for(int i = 0; i < 256; i++)
		ok[i] = false;

	for(int i = 0; i < 256; i++)
		ok[pieceIdsByLocation[i]] = true;

	for(int i = 0; i < 256; i++)
		if(!ok[i])
			cout << "ERROR";

	for(int i = 0; i < 256; i++)
	{
		int j;
		j = pieceIdsByLocation[i];
		j = pieceLocationsById[i];
		j = rotations[i];	
	}
}

void Board::Set(int index, int id, int rotation)
{
	rotations[index] = rotation;
	pieceIdsByLocation[index] = id;
	pieceLocationsById[id] = index;
}

void Board::CheckLocationsAndIDs()
{
	for(int i = 0; i < totalSize; i++)
	{
		if(pieceLocationsById[pieceIdsByLocation[i]] != i)
			cout << "ERROR";
	}
}

void Board::CheckDuplicatePieces()
{
	int ids[256];

	for(int i = 0; i < totalSize; i++)
		ids[i] = pieceIdsByLocation[i];

	sort(ids, ids + 256);

	for(int i = 0; i < totalSize-1; i++)
		if(ids[i] == ids[i+1])
			cout << "ERROR";
	

}

void Board::DeletePiece(int i)
{
	pieceIdsByLocation[i] = -1;
}

bool Board::PieceExists(int i)
{
	return pieceIdsByLocation[i] != -1;
}

void Board::Init(Piece** eternityPieces, int* pieceIdsByLocation, int* pieceLocationsById, int* rotations, int totalSize, int xsize, int ysize)
{
	this->totalSize = totalSize;
	this->xSize = xsize;
	this->ySize = ysize;
	this->eternityPieces = eternityPieces;

	this->pieceIdsByLocation = new int[totalSize];
	this->rotations = new int[totalSize];
	this->pieceLocationsById = new int[totalSize];
	this->objectives = new int[sizeof(Constants::Objectives)];

	for(int i = 0; i < totalSize; i++)
	{
		this->rotations[i] = rotations[i];
		this->pieceIdsByLocation[i] = pieceIdsByLocation[i];
		this->pieceLocationsById[i] = pieceLocationsById[i];
	}

	this->objectives[Constants::SidePoints] = 0;
	this->objectives[Constants::SquarePoints] = 0;
	this->objectives[Constants::CrossPoints] = 0;
	this->objectives[Constants::EdgePoints] = 0;
	this->distance = 0;
	this->dominance = 0;
	this->combinedPoints = 0;
	
}

void Board::SetBoard(int* pieceIdsByLocation, int* pieceLocationById, int* rotations)
{
	for(int i = 0; i < totalSize; i++)
	{
		this->pieceIdsByLocation[i] = pieceIdsByLocation[i];
		this->pieceLocationsById[i] = pieceLocationById[i];
		this->rotations[i] = rotations[i];
	}
}

void Board::Clone(Board* b)
{
	for(int i = 0; i < totalSize; i++)
	{
		this->pieceIdsByLocation[i] = b->pieceIdsByLocation[i];
		this->pieceLocationsById[i] = b->pieceLocationsById[i];
		this->rotations[i] = b->rotations[i];
	}
}