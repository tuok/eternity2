#ifndef QUICKSORTER_H
#define QUICKSORTER_H

#include "population_sorter.h"

class QuickSorter : public PopulationSorter
{
	private:
		bool ascending;
		void quickSortAscending(Board** population, int begin, int end);
		void quickSortDescending(Board** population, int begin, int end);

	public:
		QuickSorter(bool asc);
		void sort(Board** population, int populationSize);
};

#endif