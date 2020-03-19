#ifndef EDGE_SOLVER_H
#define EDGE_SOLVER_H

#include <ctime>
#include "board.h"
#include "utils.h"

using namespace std;

class EdgeSolver
{
	private:
		int xSize;
		int ySize;
		int totalSize;
		int *cornerIndexes;
		int *edgeIndexes;
		int *centerIndexes;
		bool *usedCorners;
		bool *usedEdges;
		int *usedIndexByLocation;
		Board *original;
		int WALL;
		int edgeIndexesLength;
		int cornerIndexesLength;
		int centerIndexesLength;
		int stepLimit;

		void createIndexes();

	public:
		EdgeSolver(int xSize, int ySize, int stepLimit);
		int Solve(Board* b);
};


#endif