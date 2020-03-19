#ifndef BOARD_H
#define BOARD_H

#include "piece.h"
#include "constants.h"
#include <iostream>
#include <algorithm>

using namespace std;

class Board
{
	private:
		int xSize;
		int ySize;
		int totalSize;
		int objectiveCount;

	public:
		Piece** eternityPieces;
		int* objectives;
		double combinedPoints;
		int* pieceLocationsById;
		int* pieceIdsByLocation;
		int* rotations;
		int dominance;
		double distance;

		Board();
		~Board();
		int GetUp(int i);
		int GetDown(int i);
		int GetLeft(int i);
		int GetRight(int i);
		int GetID(int i);
		int GetRotation(int i);
		int CountGaps();
		void Clone(Board* b);
		void SetBoard(int* pieceIdsByLocation, int* pieceLocationById, int* rotations);
		void InspectBoard();
		void Rotate(int i);
		void Rotate(int i, int angle);
		void Set(int index, int id, int rotation);
		void DeletePiece(int i);
		void CheckLocationsAndIDs();
		void CheckDuplicatePieces();
		bool PieceExists(int i);
		void Init(Piece** eternityPieces, int* pieceIdsByLocation, int* pieceLocationById, int* rotations, int totalSize, int xsize, int ysize);
};

#endif