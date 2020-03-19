#ifndef POPULATIONSORTER_H
#define POPULATIONSORTER_H

#include "board.h"

class PopulationSorter
{
	public:
		virtual void sort(Board** population, int populationSize) = 0;
};

#endif