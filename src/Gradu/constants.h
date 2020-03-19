#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>

using namespace std;

namespace Constants
{
	// Global parameters
	static const string COMBINEDPOINTS = "CombinedPoints";
	static const string	ETERNITYPIECESPATH = "EternityPiecesPath";
	static const string MAXGENERATIONS = "MaxGenerations";
	static const string TOURNAMENTSIZE = "TournamentSize";
	static const string EDGESASPOINTS = "EdgesAsPoints";
	static const string GENERATIONSBETWEENOUTPUTS = "GenerationsBetweenOutputs";
	static const string FINDBESTMAXINDEX = "FindBestMaxIndex";
	static const string SOLVEEDGES = "SolveEdges";
	static const string OBJ1WEIGHT = "Objective1Weight";
	static const string OBJ2WEIGHT = "Objective2Weight";
	static const string OBJ3WEIGHT = "Objective3Weight";
	static const string OBJ4WEIGHT = "Objective4Weight";
	static const string STEPLIMIT = "StepLimit";
	static const string SEED = "Seed";
	static const string POPULATIONREADWRITE = "PopulationReadWrite";
	static const string POPULATIONSAVEPATH = "PopulationSavePath";
	static const string READ = "Read";
	static const string WRITE = "Write";
	static const string NONE = "None";
	static const enum Objectives { SidePoints, SquarePoints, CrossPoints, EdgePoints };
	static const enum PieceTypes { Center, Edge, Corner };
	static const enum Directions { UP, RIGHT, DOWN, LEFT };
	static const string HCSA = "HCSA";
	static const string GA = "GA";
	static const string SA = "SA";
	static const string HC = "HC";
	static const string IMPROVEMENTLIMIT = "ImprovementLimit";

	// Genetic algorithm parameters
	static const string CROSSOVERAMOUNT = "CrossoverAmount";
	static const string CROSSOVERMINBOUND = "CrossoverMinBound";
	static const string CROSSOVERMAXBOUND = "CrossoverMaxBound";
	static const string ELITISMAMOUNT = "ElitismAmount";
	static const string MUTATIONAMOUNT = "MutationAmount";
	static const string MUTATIONMINBOUND = "MutationMinBound";
	static const string MUTATIONMAXBOUND = "MutationMaxBound";
	static const string POPULATIONSIZE = "PopulationSize";
	static const string SORTINGALGORITHM = "SortingAlgorithm";
	static const string ASCENDING = "Ascending";
	static const string DESCENDING = "Descending";
	static const string QUICKSORT = "QuickSort";
	static const string ALGORITHMTYPE = "AlgorithmType";
	static const string MOEA = "MOEA";
	static const string OBJECTIVECOUNT = "ObjectiveCount";
	
	// Simulated Annealing / Hill Climbing parameters
	static const string ALPHA = "Alpha";
	static const string EPSILON = "Epsilon";
	static const string TEMPERATURE = "StartingTemperature";
	static const string MUTATIONTYPE = "MutationType";
	static const string BF = "BruteForce";
	static const string MODIFYFAILEDBOARD = "ModifyFailedBoard";
	static const string LINEARANNEALING = "LinearAnnealing";
	static const string ALLOWSAMEPOINTS = "AllowSamePoints";
	static const string MUTATIONRATIO = "MutationRatio";
}

#endif