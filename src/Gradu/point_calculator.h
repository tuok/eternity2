#ifndef POINTCALCULATOR_H
#define POINTCALCULATOR_H

#include "board.h"
#include "utils.h"
// TODO: Remove iostream when calculateOverallPoints is finished
#include <iostream>

enum Location {C, U, D, L, R, UL, UR, DL, DR};
enum BoardStyle {SingleBoard, SingleArray};

class PointCalculator
{
	private:
		Location *singleLocations;
		Piece** eternityPieces;
		int xSize;
		int ySize;
		int totalSize;
		int populationSize;
		BoardStyle style;
		int WALL;
		double maxSidePoints;
		double maxSquarePoints;
		double maxCrossPoints;
		double maxEdgePoints;
		bool edgesAsPoints;
		double obj1Weight, obj2Weight, obj3Weight, obj4Weight;
		double totalWeight;
		double obj1Pts, obj2Pts, obj3Pts, obj4Pts;
		double maxSingleSidePoints;
		double maxSingleSquarePoints;
		double maxSingleCrossPoints;
		double maxSingleEdgePoints;		

		void initSingleLocations();
		void calculateSingleIndex(Board *board, int i);
		void calculateSingleAdjacentIndex(Board *board, int i, int dir);
		double calculateCombinedPointsForSingleIndex();
		void calculateBoard(Board *board);
		void calculateCombinedPoints(Board *board);
		void calculatePopulation(Board **board);
		void calculateCombinedPointsPopulation(Board **board);

	public:
		int objectiveCount;

		~PointCalculator();
		PointCalculator(Board* board, int xsize, int ysize, double maxSidePoints, double maxSquarePoints, double maxCrossPoints, double maxEdgePoints, bool edgesAsPoints);
		PointCalculator(Board** population, int xsize, int ysize, int populationsize, double maxSidePoints, double maxSquarePoints, double maxCrossPoints, double maxEdgePoints, bool edgesAsPoints);
		
		void Calculate(Board* board);
		void Calculate(Board** population);
		double CalculateIndex(Board* board, int i);
		double CalculateAdjacentIndex(Board* board, int i, int dir);
		void SetActiveObjectives(double obj1Weight, double obj2Weight, double obj3Weight, double obj4Weight);
};

#endif