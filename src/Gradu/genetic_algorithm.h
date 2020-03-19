#ifndef GENETICALGORITHM_H
#define GENETICALGORITHM_H

#include <vector>
#include "algorithm.h"
#include "board.h"

using namespace std;

class GeneticAlgorithm : public Algorithm
{
	private:
		Board** population;
		Board** newPopulation;
		Board** tempPopulation;
		int populationSize;
		int elitismAmount;
		int crossoverAmount;
		int crossoverMinBound;
		int crossoverMaxBound;
		int mutationAmount;
		int mutationMinBound;
		int mutationMaxBound;
		int globalIndex;
		int tournamentSize;
		PopulationSorter *sorter;
		string sortingAlgorithm;
		string sortingOrder;
		int* idsByLoc;
		int* locsById;
		int* rots;
		vector<int> paretoFrontIndexes;
		int** distObjTemp;
		int findBestMaxIndex;

		void loadParametersFromConfig();
		void orderPopulation();
		void checkPopulation();
		void elitismGA();
		void elitismMOEA();
		void crossover();
		void performCrossover();
		void mutation();
		void assignNewPopulation();
		void calculateObjectives();
		void calculatePointsMOEA();
		void exchangeMutation(int i);
		void rotateMutation(int i);
		int tournamentSelection();
		void RunGA();
		void RunMOEA();
		void assignDominance();
		int dominance(int i, int j);
		void assignDistances();

		void testPopulation();
		void testNewPopulation();
		void testBoard(Board *b);

	public:
		GeneticAlgorithm(string configPath);
		~GeneticAlgorithm();
		void Run();

};

#endif