#include "quick_sorter.h"

QuickSorter::QuickSorter(bool asc)
{
	ascending = asc;
}

void QuickSorter::sort(Board** population, int populationSize)
{
	if(ascending)
		quickSortAscending(population, 0, populationSize - 1);

	else
		quickSortDescending(population, 0, populationSize - 1);
}

void QuickSorter::quickSortAscending(Board **population, int begin, int end)
{
	if(end > begin)
	{
		// select pivot
		Board* pivot = population[(begin+end)/2];
		
		// partition
		int left = begin, right = end;
		double pivotPoints = pivot->combinedPoints;

		do
		{
			while(left <= end && population[left]->combinedPoints <= pivotPoints)
				left++;	

			while( right >= begin && population[right]->combinedPoints > pivotPoints)
				right--;	

			if(left < right)
			{
				Board* aux;
				aux = population[left];
				population[left] = population[right];
				population[right] = aux;
			}

		} while(left < right);
		
		// put pivots at the end because they are the greatest ones in this part
		for(int i = begin; i < right; i++)
		{
			if(population[i]->combinedPoints == pivotPoints)
			{
				Board* aux;
				aux = population[i];
				population[i] = population[right];
				population[right] = aux;
				right--;
			}
		}
		
		// recursive sort
		quickSortAscending(population, begin, right);
		quickSortAscending(population, left, end);
	}
}

void QuickSorter::quickSortDescending(Board **population, int begin, int end)
{
	if(end > begin)
	{
		// select pivot
		Board* pivot = population[(begin+end)/2];
		
		// partition
		int left = begin, right = end;
		double pivotPoints = pivot->combinedPoints;

		do
		{
			while(left <= end && population[left]->combinedPoints >= pivotPoints)
				left++;	

			while( right >= begin && population[right]->combinedPoints < pivotPoints)
				right--;	

			if(left < right)
			{
				Board* aux;
				aux = population[left];
				population[left] = population[right];
				population[right] = aux;
			}

		} while(left < right);
		
		// put pivots at the end because they are the greatest ones in this part
		for(int i = begin; i < right; i++)
		{
			if(population[i]->combinedPoints == pivotPoints)
			{
				Board* aux;
				aux = population[i];
				population[i] = population[right];
				population[right] = aux;
				right--;
			}
		}
		
		// recursive sort
		quickSortDescending(population, begin, right);
		quickSortDescending(population, left, end);
	}
}