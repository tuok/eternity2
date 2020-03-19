#ifndef HCSA_H
#define HCSA_H

#include "algorithm.h"
#include "board.h"
#include "edge_solver.h"

class HCSA : public Algorithm
{
	private:
		Board* board;
		Board* tempBoard;
		int crossoverAmount;
		int crossoverMinBound;
		int crossoverMaxBound;
		int mutationAmount;
		int mutationMinBound;
		int mutationMaxBound;
		string mutationType;
		double iPts, jPts, iMaxPts, jMaxPts, ijPts, ijMaxPts, iMaxSwapPts, jMaxSwapPts, ijMaxSwapPts;
		int iRot, jRot, iSwapRot, jSwapRot, iDir, jDir;
		bool swapped;
		bool improvement;
		double T, T0, epsilon, alpha, delta;
		bool linearAnnealing;
		bool allowSamePoints;
		double mutationRatio;
		double bestPoints;

		bool checkPoints();
		void loadParametersFromConfig();
		void runHC_GA();
		void runHC_BF();
		void runSA();
		void exchangeMutation();
		void rotateMutation();
		void swap(int i, int j, bool useTemp);
		void swapAction(int i, int j, bool useTemp);
		void testChange(int i, int j, bool swapped, bool adjacent, bool useTemp);
		
	public:
		HCSA(string configPath);
		~HCSA();
		void Run();
};

#endif