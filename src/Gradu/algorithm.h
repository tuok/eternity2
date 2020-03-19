#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <ctime>
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>

#include "constants.h"
#include "config.h"
#include "mtrand.h"
#include "utils.h"
#include "point_calculator.h"
#include "edge_solver.h"
#include "quick_sorter.h"
#include "boardio.h"
#include "fileio.h"

class Algorithm
{
	protected:
		Config cfg;
		MTRand_int32 rand;
		MTRand randFloat;
		Piece** eternityPieces;
		int totalSize;
		int xSize;
		int ySize;
		int generation;
		int maxGenerations;
		bool error;
		bool ready;
		PointCalculator *pointCalculator;
		int maxSidePoints;
		int maxSquarePoints;
		int maxCrossPoints;
		int maxEdgePoints;
		string eternityPiecePath;
		string algorithmType;
		bool edgesAsPoints;
		int* idsByLocation;
		int* locationsById;
		int* rotations;
		int totalObjectives;
		int objectiveCount;
		int generationsBetweenOutputs;
		int overallBestSidePoints;
		int generationsFromLastBest;
		int improvementLimit;
		unsigned long seed;
		long userSeed;
		double obj1Weight, obj2Weight, obj3Weight, obj4Weight;
		bool activeObjectives[4];
		bool solveEdges;
		bool modifyFailedBoard;
		int stepLimit;
		string populationSavePath;
		string populationReadWrite;
		stringstream output;
		stringstream out;

		void printHeader();
		void printPoints(Board* board);
		void printPoints(Board* board, double T);
		void saveLogGA(int E, int C, int M, int CMinB, int CMaxB, int MMinB, int MMaxB, int TS, int points);
		void saveLogHC_GA(int MMinB, int MMaxB, int points);
		void saveLogHC_BF(int points);
		void saveLogSA_BF(int points, double alpha, double epsilon, double T, bool allowSamePoints);
		void saveLogSA_GA(int MMinB, int MMaxB,int points, double alpha, double epsilon, double T, bool allowSamePoints, double mutationRatio);
		//void saveLogHCSA();
		
	public:
		Algorithm();
		virtual ~Algorithm();
		bool IsError();
		virtual void Run() = 0;
};

#endif