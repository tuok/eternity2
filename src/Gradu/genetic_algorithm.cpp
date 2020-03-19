#include "genetic_algorithm.h"

GeneticAlgorithm::GeneticAlgorithm(string configFilePath)
{
	// Load contents of provided config file
	if(cfg.LoadConfig(configFilePath))
	{
		// Parse all needed parameters from config
		loadParametersFromConfig();

		populationSize = elitismAmount + crossoverAmount + mutationAmount;

		// Try to load Eternity pieces to array of Piece pointers
		if(!Utils::LoadEternityPieces(eternityPiecePath, &eternityPieces))
			error = true;

		int* idsByLocation = new int[totalSize];
		int* locationsById = new int[totalSize];
		int* rotations = new int[totalSize];

		idsByLoc = new int[totalSize];
		locsById = new int[totalSize];
		rots = new int[totalSize];

		for(int i = 0; i < totalSize; i++)
			idsByLocation[i] = i;

		activeObjectives[0] = obj1Weight > 0;
		activeObjectives[1] = obj2Weight > 0;
		activeObjectives[2] = obj3Weight > 0;
		activeObjectives[3] = obj4Weight > 0;

		// TODO: Ker‰‰ yhteinen algoritmien alustustoiminnallisuus ja laita se Algorithm-luokkaan. Rakentajaa voisi muutenkin siivota hieman.
		if(!error)
		{
			if(edgesAsPoints)	maxSidePoints = 2 * totalSize + xSize + ySize;
			else				maxSidePoints = xSize * (xSize-1) + ySize * (ySize-1);

			maxSquarePoints = (xSize - 1) * (ySize - 1);
			maxCrossPoints = xSize * ySize;
			maxEdgePoints = 2*xSize + 2*ySize;
			
			if(userSeed != 0)
			{
				seed = (unsigned long)userSeed;
				rand.seed(seed);
				cout << "User provided seed: " << seed << endl;
				output << "User provided seed: " << seed << endl;
			}

			else
			{
				cout << "Random seed: " << seed << endl;
				output << "Random seed: " << seed << endl;
				rand.seed(seed);
			}
			
			// Initialize population container by population size (fetched from config file)
			population = new Board*[populationSize];
			newPopulation = new Board*[populationSize];
			tempPopulation = new Board*[populationSize];

			distObjTemp = new int*[populationSize];

			// Create new Board pointers and initialize population
			for(int i = 0; i < populationSize; i++)
			{
				population[i] = new Board();
				newPopulation[i] = new Board();
				tempPopulation[i] = new Board();

				Board* b1 = population[0];

				Utils::ShuffleIntArrays(rand, idsByLocation, rotations);

				for(int j = 0; j < totalSize; j++)
					locationsById[idsByLocation[j]] = j;

				population[i]->Init(eternityPieces, idsByLocation, locationsById, rotations, totalSize, xSize, ySize);
				newPopulation[i]->Init(eternityPieces, idsByLocation, locationsById, rotations, totalSize, xSize, ySize);
				tempPopulation[i]->Init(eternityPieces, idsByLocation, locationsById, rotations, totalSize, xSize, ySize);
			}

			delete [] idsByLocation;
			delete [] locationsById;
			delete [] rotations;
			idsByLocation = 0;
			locationsById = 0;
			rotations = 0;

			overallBestSidePoints = 0;
			generationsFromLastBest = 0;
			
			if(improvementLimit == 0) improvementLimit = maxGenerations;

			testPopulation();

			// Set desired population sorting algorithm
			if(sortingAlgorithm.compare(Constants::QUICKSORT) == 0)
			{
				if(algorithmType.compare(Constants::MOEA) == 0)
					sorter = new QuickSorter(true);

				else if(algorithmType.compare(Constants::GA) == 0)
					sorter = new QuickSorter(false);
			}


			else if(sortingAlgorithm.length() < 1)
				error = true;

			else
			{
				error = true;
				cout << "Error, sorting algorithm '" << sortingAlgorithm << "' is not implemented!" << endl;
			}

			pointCalculator = new PointCalculator(population, xSize, ySize, populationSize, maxSidePoints, maxSquarePoints, maxCrossPoints, maxEdgePoints, edgesAsPoints);
			pointCalculator->SetActiveObjectives(obj1Weight, obj2Weight, obj3Weight, obj4Weight);
			objectiveCount = pointCalculator->objectiveCount;

			for(int i = 0; i < populationSize; i++)
				distObjTemp[i] = new int[totalObjectives + 1];

			globalIndex = 0;

			if(findBestMaxIndex < 1 || findBestMaxIndex > populationSize)
				findBestMaxIndex = populationSize;

			if(solveEdges && populationReadWrite.compare(Constants::READ) != 0)
			{
				EdgeSolver es(xSize, ySize, stepLimit);
				int steps;
				int ignoredBoards = 0;

				for(int i = 0; i < populationSize; i++)
				{
					steps = es.Solve(population[i]);

					if(steps == -1)
						ignoredBoards++;

					else
					{
						cout << "Solved edges for " << i+1 << "/" << populationSize << " boards... (" << steps << " steps)    ";
						cout << "\r";
					}
				}

				cout << endl << "Edge solving finished. Ignored total of " << ignoredBoards << " boards. Continuing with actual algorithm..." << endl;
				output << endl << "Edge solving finished. Ignored total of " << ignoredBoards << " boards. Continuing with actual algorithm..." << endl;
			}

			if(populationReadWrite.compare(Constants::WRITE) == 0)
			{
				cout << "Saving population to file '" << populationSavePath << "'...";
				output << "Saving population to file '" << populationSavePath << "'...";
				BoardIO boardIO(populationSavePath, totalSize, totalObjectives);
				boardIO.Write(population, populationSize);
				cout << " Done!" << endl;
				output << " Done!" << endl;
			}

			else if(populationReadWrite.compare(Constants::READ) == 0)
			{
				testPopulation();
				cout << "Reading population from file '" << populationSavePath << "'...";
				output << "Reading population from file '" << populationSavePath << "'...";
				BoardIO boardIO(populationSavePath, totalSize, totalObjectives);
				boardIO.Read(population, populationSize);
				cout << " Done!" << endl;
				output << " Done!" << endl;
			}

			else
			{
				if(populationReadWrite.compare(Constants::NONE) != 0)
					cout << "Warning, PopulationReadWrite set to '" << populationReadWrite << "', assuming None..." << endl;
			}
		}
	}

	else
		error = true;

}

void GeneticAlgorithm::loadParametersFromConfig()
{
	// Load needed parameters from config
	if(!cfg.GetInt(Constants::ELITISMAMOUNT, elitismAmount)) error = true;
	if(!cfg.GetInt(Constants::CROSSOVERAMOUNT, crossoverAmount)) error = true;
	if(!cfg.GetInt(Constants::CROSSOVERAMOUNT, crossoverAmount)) error = true;
	if(!cfg.GetInt(Constants::CROSSOVERMINBOUND, crossoverMinBound)) error = true;
	if(!cfg.GetInt(Constants::CROSSOVERMAXBOUND, crossoverMaxBound)) error = true;
	if(!cfg.GetInt(Constants::MUTATIONAMOUNT, mutationAmount)) error = true;
	if(!cfg.GetInt(Constants::MUTATIONMINBOUND, mutationMinBound)) error = true;
	if(!cfg.GetInt(Constants::MUTATIONMAXBOUND, mutationMaxBound)) error = true;
	if(!cfg.GetBool(Constants::EDGESASPOINTS, edgesAsPoints)) error = true;
	if(!cfg.GetString(Constants::ETERNITYPIECESPATH, eternityPiecePath)) error = true;
	if(!cfg.GetInt(Constants::MAXGENERATIONS, maxGenerations)) error = true;
	if(!cfg.GetString(Constants::SORTINGALGORITHM, sortingAlgorithm)) error = true;
	if(!cfg.GetInt(Constants::TOURNAMENTSIZE, tournamentSize)) error = true;
	if(!cfg.GetInt(Constants::GENERATIONSBETWEENOUTPUTS, generationsBetweenOutputs)) error = true;
	if(!cfg.GetString(Constants::ALGORITHMTYPE, algorithmType)) error = true;
	if(!cfg.GetInt(Constants::FINDBESTMAXINDEX, findBestMaxIndex)) error = true;
	if(!cfg.GetDouble(Constants::OBJ1WEIGHT, obj1Weight)) error = true;
	if(!cfg.GetDouble(Constants::OBJ2WEIGHT, obj2Weight)) error = true;
	if(!cfg.GetDouble(Constants::OBJ3WEIGHT, obj3Weight)) error = true;
	if(!cfg.GetDouble(Constants::OBJ4WEIGHT, obj4Weight)) error = true;
	if(!cfg.GetBool(Constants::SOLVEEDGES, solveEdges)) error = true;
	if(!cfg.GetInt(Constants::STEPLIMIT, stepLimit)) error = true;
	if(!cfg.GetInt(Constants::IMPROVEMENTLIMIT, improvementLimit)) error = true;
	if(!cfg.GetLong(Constants::SEED, userSeed)) error = true;
	if(!cfg.GetString(Constants::POPULATIONSAVEPATH, populationSavePath)) error = true;
	if(!cfg.GetString(Constants::POPULATIONREADWRITE, populationReadWrite)) error = true;
}

GeneticAlgorithm::~GeneticAlgorithm()
{
	if(!error)
	{
		for(int i = 0; i < populationSize; i++)
			delete population[i];

		delete [] population;
		population = 0;

		delete pointCalculator;
		pointCalculator = 0;
	}
}

void GeneticAlgorithm::Run()
{
	//cout << Utils::PrintBoard(population[0], xSize, ySize, true) << endl;

	if(algorithmType.compare(Constants::GA) == 0)
		RunGA();

	else if(algorithmType.compare(Constants::MOEA) == 0)
		RunMOEA();

	else
	{
		error = true;
		cout << "Algorithm type '" << algorithmType << "' not implemented!" << endl;
	}

	if(!error)
	{
		int maxSidePoints = 0;
		int index = -1;

		for(int i = 0; i < populationSize; i++)
		{
			if(population[i]->objectives[Constants::SidePoints] > maxSidePoints)
			{
				maxSidePoints = population[i]->objectives[Constants::SidePoints];
				index = i;
			}
		}

		string board = Utils::PrintBoard(population[index], xSize, ySize, true);
		output << board;
		cout << board;
		saveLogGA(elitismAmount, crossoverAmount, mutationAmount, crossoverMinBound, crossoverMaxBound, mutationMinBound, mutationMaxBound, tournamentSize, maxSidePoints);
	}
}

void GeneticAlgorithm::RunGA()
{
	printHeader();

	while(generation++ < maxGenerations && !ready)
	{
		calculateObjectives();
		orderPopulation();
		checkPopulation();

		if(ready) break;
		
		elitismGA();
		crossover();
		mutation();
		assignNewPopulation();
	}
}

void GeneticAlgorithm::RunMOEA()
{
	printHeader();

	while(generation++ < maxGenerations && !ready)
	{
		calculateObjectives();
		assignDominance();
		assignDistances();
		calculatePointsMOEA();
		orderPopulation();
 		checkPopulation();

		if(ready) break;

		elitismMOEA();
		crossover();
		mutation();
		assignNewPopulation();
	}
}

void GeneticAlgorithm::assignDominance()
{
	int dom;
	paretoFrontIndexes.clear();
	
	for(int i = 0; i < populationSize; i++)
	{
		population[i]->dominance = 0;
		population[i]->distance = 0;
	}

	// Tarkasta populaation dominaatio pareittain. Jos A dominoi B:t‰, ei B varmastikaan dominoi A:ta
	for(int i = 0; i < populationSize-1; i++)
	{
		for(int j = i+1; j < populationSize; j++)
		{
			dom = dominance(i, j);
			if(dom == 1)		population[j]->dominance++;
			else if(dom == -1)	population[i]->dominance++;
		}
	}

	for(int i = 0; i < populationSize; i++)
		if(population[i]->dominance == 0)
			paretoFrontIndexes.push_back(i);
}

// Palauttaa, kumpaa dominoidaan, -1: j dominoi i:t‰, 1: i dominoi j:t‰
// HUOM: Oma toteutus merkkasi dominanssin, jos toisella yksilˆll‰ oli enemm‰n voittoja tavoitteissa
// Munozin versio dominoi, jos toinen yksilˆ "voitti" (yht‰suuruuskin voittaa yhden tavoitteen) kaikki tavoitteet
inline int GeneticAlgorithm::dominance(int i, int j)
{
	// Debug code
	//int iSide = population[i]->objectives[Constants::SidePoints];
	//int iSquare = population[i]->objectives[Constants::SquarePoints];
	//int iCross = population[i]->objectives[Constants::CrossPoints];

	//int jSide = population[j]->objectives[Constants::SidePoints];
	//int jSquare = population[j]->objectives[Constants::SquarePoints];
	//int jCross = population[j]->objectives[Constants::CrossPoints];

	int iDominates = 0;
	int jDominates = 0;

	for(int k = 0; k < totalObjectives; k++)
	{
		if(activeObjectives[k])
		{
			if(population[i]->objectives[k] >= population[j]->objectives[k]) iDominates++;
			if(population[j]->objectives[k] >= population[i]->objectives[k]) jDominates++;
		}
	}

	// TODO: Testaa virheellisesti lasketun dominanssin (iDominance < jDominance ? -1 : 1, tjsp) tehokkuutta
	if(jDominates == iDominates)			return 0;
	else if(iDominates == objectiveCount)	return 1;
	else if(jDominates == objectiveCount)	return -1;
	else									return 0;
}

void GeneticAlgorithm::assignDistances()
{
	int paretoFrontSize = paretoFrontIndexes.size();
	int p = 0;
	double maxDist = 0;
	bool sameObjectives = false;
	
	// Loop through all pareto front individuals
	for(int i = 0; i < paretoFrontSize; i++)
	{
		// Check if individual with same objectives (same distance) has already been processed
		for(int j = 0; j < p; j++)
		{
			sameObjectives = true;

			for(int k = 0; k < totalObjectives; k++)
			{
				if(activeObjectives[k] && population[paretoFrontIndexes[i]]->objectives[k] != distObjTemp[j][k])
				{
					sameObjectives = false;
					break;
				}
			}

			// If found individual with same objectives, assign corresponding distance value to current population index
			if(sameObjectives)
			{
				population[paretoFrontIndexes[i]]->distance = distObjTemp[j][totalObjectives];
				break;
			}
		}

		// If there's no individual with same objectives, calculate distance for current population index
		if(!sameObjectives)
		{
			for(int j = i+1; j < paretoFrontSize; j++)
			{
				for(int k = 0; k < totalObjectives; k++)
				{
					if(activeObjectives[k] && population[paretoFrontIndexes[i]]->objectives[k] != population[paretoFrontIndexes[j]]->objectives[k])
					{
						population[paretoFrontIndexes[i]]->distance++;
						population[paretoFrontIndexes[j]]->distance++;
					}
				}
			}
		}

		if(population[paretoFrontIndexes[i]]->distance > maxDist)
			maxDist = population[paretoFrontIndexes[i]]->distance;

		if(activeObjectives[0]) distObjTemp[p][0] = (int)population[paretoFrontIndexes[i]]->objectives[0];
		if(activeObjectives[1]) distObjTemp[p][1] = (int)population[paretoFrontIndexes[i]]->objectives[1];
		if(activeObjectives[2]) distObjTemp[p][2] = (int)population[paretoFrontIndexes[i]]->objectives[2];
		if(activeObjectives[3]) distObjTemp[p][3] = (int)population[paretoFrontIndexes[i]]->objectives[3];
		distObjTemp[p][4] = (int)population[paretoFrontIndexes[i]]->distance;
		p++;
	}
	
	for(int i = 0; i < paretoFrontSize; i++)
		population[paretoFrontIndexes[i]]->distance = (maxDist == 0) ? 0 : 1 - population[paretoFrontIndexes[i]]->distance / maxDist;
}

inline void GeneticAlgorithm::orderPopulation()
{
	sorter->sort(population, populationSize);
}

inline void GeneticAlgorithm::checkPopulation()
{
	int bestSidePoints = 0;
	int bestSidePointsIndex = -1;

	for(int i = 0; i < findBestMaxIndex; i++)
	{
		if(population[i]->objectives[0] > bestSidePoints)
		{
			bestSidePoints = population[i]->objectives[0];
			bestSidePointsIndex = i;
		}
	}

	generationsFromLastBest++;

	if(bestSidePoints > overallBestSidePoints)
	{
		overallBestSidePoints = bestSidePoints;
		generationsFromLastBest = 0;
	}

	if(bestSidePoints == maxSidePoints)
		ready = true;
		
	if(generation % generationsBetweenOutputs == 0)
		printPoints(population[bestSidePointsIndex]);

	if(generationsFromLastBest >= improvementLimit)
	{
		ready = true;

		while(generation++ < maxGenerations)
			printPoints(population[bestSidePointsIndex]);			
	}

}

inline void GeneticAlgorithm::testPopulation()
{
	// TODO: Jos kiinnostaa tarpeeksi, niin tarkista miksi seuraava breakpointti tuottaa erilaisen rivin riippuen siit‰, onko
	// v‰estˆ ladattu tiedostosta vai onko v‰estˆ generoitu suoraan (molemmissa tapauksissa sama seedi)
	for(int h = 0; h < populationSize; h++)
		for(int i = 0; i < totalSize-1; i++)
			for(int j = i+1; j < totalSize; j++)
				if(population[h]->pieceIdsByLocation[i] == population[h]->pieceIdsByLocation[j])
					cout << "Error at generation " << generation << ": Indexes " << i << " and " << j << " are the same for individual #" << h << "!" << endl;
}

inline void GeneticAlgorithm::testNewPopulation()
{
	// TODO: Jos kiinnostaa tarpeeksi, niin tarkista miksi seuraava breakpointti tuottaa erilaisen rivin riippuen siit‰, onko
	// v‰estˆ ladattu tiedostosta vai onko v‰estˆ generoitu suoraan (molemmissa tapauksissa sama seedi)
	for(int h = 0; h < globalIndex; h++)
		for(int i = 0; i < totalSize-1; i++)
			for(int j = i+1; j < totalSize; j++)
				if(newPopulation[h]->pieceIdsByLocation[i] == newPopulation[h]->pieceIdsByLocation[j])
					cout << "Error at generation " << generation << ": Indexes " << i << " and " << j << " are the same for individual #" << h << "!" << endl;
}

inline void GeneticAlgorithm::testBoard(Board *b)
{
	for(int i = 0; i < totalSize-1; i++)
		for(int j = i+1; j < totalSize; j++)
			if(b->PieceExists(i) && b->PieceExists(j) && b->pieceIdsByLocation[i] == b->pieceIdsByLocation[j])
				cout << "Error at board " << b << ": Indexes " << i << " and " << j << " are the same!" << endl;
}

inline void GeneticAlgorithm::elitismGA()
{
	for(int i = 0; i < elitismAmount; i++)
		newPopulation[globalIndex++]->Clone(population[i]);
}

inline void GeneticAlgorithm::elitismMOEA()
{
	//for(int i = 0; i < elitismAmount && population[i]->dominance == 0; i++)
	//	newPopulation[globalIndex++]->Clone(population[i]);
	int i = 0;

	while(globalIndex < elitismAmount && i < populationSize)
	{
		if(population[i]->dominance == 0)
			newPopulation[globalIndex++]->Clone(population[i]);

		++i;
	}
}

inline void GeneticAlgorithm::crossover()
{
	// If there is no elitismAmount individuals in Pareto front, fill rest individuals with crossover
	int totalCrossovers = crossoverAmount + elitismAmount - globalIndex;

	for(int z = 0; z < totalCrossovers; z++)
	{
		performCrossover();
	}
}

void GeneticAlgorithm::performCrossover()
{
	// TODO: Clean function; remove unnecessary temporary variables and debug code
	int index, temp, tempX, tempY;
	vector<int> piecesToRemove;
	vector<int> orphanPieces;
	vector<int> offspringGaps;
	Board* a = population[tournamentSelection()];
	Board* b = population[tournamentSelection()];
	Board* offspring = newPopulation[globalIndex];
	offspring->Clone(a);

	// Get random crossover area
	int W = (rand() % (crossoverMaxBound - crossoverMinBound + 1)) + crossoverMinBound;
	int	H = (rand() % (crossoverMaxBound - crossoverMinBound + 1)) + crossoverMinBound;
	int X = rand() % (xSize - W + 1);
	int Y = rand() % (ySize - H + 1);

	// Mark piece ID's that should be removed from offspring board (crossover area pieces)
	for(int y = Y; y < Y + H; y++)
		for(int x = X; x < X + W; x++)
			piecesToRemove.push_back(b->GetID(y*xSize + x));

	// Delete marked pieces from offspring board and keep record of their indexes which do not remain in crossover area
	for(unsigned int i = 0; i < piecesToRemove.size(); i++)
	{
		index = offspring->pieceLocationsById[piecesToRemove[i]];
		offspring->DeletePiece(index);
		tempX = index % xSize;
		tempY = index / ySize;

		if(tempX < X || tempX >= X+W || tempY < Y || tempY >= Y+H)
			offspringGaps.push_back(index);
	}

	// Mark pieces found from offspring board's crossover area as orphans. Copy pieces from board b's crossover area to offspring board. Mark copied pieces to be saved later.
	for(int y = Y; y < Y + H; y++)
	{
		for(int x = X; x < X + W; x++)
		{
			if(offspring->PieceExists(y*xSize + x))
			{
				// Check if there is piece in this location and insert its ID to orphanPieces
				orphanPieces.push_back(offspring->GetID(y*xSize + x));
				// Then delete piece from offspring board
				offspring->DeletePiece(y*xSize+x);
			}

			index = y*xSize+x;
			offspring->Set(index, b->GetID(index), b->GetRotation(index));
		}
	}

	// Shuffle orphan pieces
	for(int i = orphanPieces.size(); i > 0 ; i--)
	{
		index = rand() % i;
		temp = orphanPieces[index];
		orphanPieces[index] = orphanPieces[i-1];
		orphanPieces[i-1] = temp;
	}

	//if(orphanPieces.size() != offspringGaps.size())
	//	cout << "Something has gone awry, there are not as many gaps at the board as there is pieces" << endl;

	// Fill in gaps of offspring board by orphan pieces (orphans are taken from board b).
	for(unsigned int i = 0; i < offspringGaps.size(); i++)
		offspring->Set(offspringGaps[i], orphanPieces[i], b->GetRotation(orphanPieces[i]));

	//newPopulation[globalIndex++] = offspring;
	globalIndex++;
}

inline void GeneticAlgorithm::mutation()
{
	// TODO: Find out if mutation part of the population is first created with crossover and then mutated or if this group is just mutated
	// TODO: Munoz's and guys' properties file had mutationMin/MaxMutations property (1 and 100), check if amount of mutations is chosen from this range
	for(int i = 0; i < mutationAmount; i++)
	{
		int p = tournamentSelection();

		if(rand() % 2)	exchangeMutation(p);
		else			rotateMutation(p);
	}
}

void GeneticAlgorithm::exchangeMutation(int i)
{
	Board* a = population[tournamentSelection()];
	Board* b = newPopulation[globalIndex];

	// TODO: Tarkista, toimiiko hienosti ilman t‰t‰
	//b->Clone(a);

	for(int i = 0; i < totalSize; i++)
	{
		// TODO: T‰ss‰ voisi periaatteessa t‰ytt‰‰ vain vakiona pysyv‰t indeksit, sill‰ vaihdettavat indeksit t‰ytet‰‰n myˆhemmin jokatapauksessa.
		idsByLoc[i] = a->pieceIdsByLocation[i];
		rots[i] = a->rotations[i];
	}

	int W = (rand() % (mutationMaxBound - mutationMinBound + 1)) + mutationMinBound;
	int H = (rand() % (mutationMaxBound - mutationMinBound + 1)) + mutationMinBound;

	int x1, x2, y1, y2, i1, i2;

	// Left region
	if(rand() % 2 == 1)
	{
		y1 = rand() % (ySize - H + 1);
		x1 = rand() % (xSize - 2*W + 1);
		y2 = rand() % (ySize - H + 1);
		x2 = rand() % (xSize - 2*W - x1 + 1) + W + x1;
	}
	// Bottom region
	else
	{
		y1 = rand() % (ySize - 2*H + 1);
		x1 = rand() % (xSize - W + 1);
		y2 = rand() % (ySize - 2*H - y1 + 1) + H + y1;
		x2 = rand() % (xSize - W + 1);
	}

	// Copy area x1, y1 from copied board to mutated board position x2, y2 and likewise area x2, y2 from copied board to mutated board position x1,y1
	for(int y = 0; y < H; y++)
	{
		for(int x = 0; x < W; x++)
		{
			i1 = (y1+y)*xSize + (x1+x);
			i2 = (y2+y)*xSize + (x2+x);

			idsByLoc[i2] = a->pieceIdsByLocation[i1];
			rots[i2] = a->rotations[i1];

			idsByLoc[i1] = a->pieceIdsByLocation[i2];
			rots[i1] = a->rotations[i2];
		}
	}

	for(int i = 0; i < totalSize; i++)
		locsById[idsByLoc[i]] = i;

	b->SetBoard(idsByLoc, locsById, rots);

	//newPopulation[globalIndex++] = b;
	globalIndex++;
}

void GeneticAlgorithm::rotateMutation(int i)
{
	Board* a = population[tournamentSelection()];
	Board* b = newPopulation[globalIndex];

	// b->Clone(a) means that a is copied to b (not very intuitive, I know...)
	b->Clone(a);

	for(int i = 0; i < totalSize; i++)
	{
		// TODO: T‰ss‰ voisi periaatteessa t‰ytt‰‰ vain vakiona pysyv‰t indeksit, sill‰ vaihdettavat indeksit t‰ytet‰‰n myˆhemmin jokatapauksessa.
		idsByLoc[i] = a->pieceIdsByLocation[i];
		rots[i] = a->rotations[i];
	}

	int W = rand() % (mutationMaxBound - mutationMinBound + 1) + mutationMinBound;
	int X = rand() % (xSize - W);
	int Y = rand() % (ySize - W);

	int newX, newY, i1, i2;

	switch(rand() % 3)
	{
		// Rotate 90 degrees
		case 0:
			newX = X + W - 1;

			for(int y = Y; y < Y + W; y++)
			{
				newY = Y;
				
				for(int x = X; x < X + W; x++)
				{
					i1 = y*xSize + x;
					i2 = newY*xSize + newX;

					idsByLoc[i2] = a->pieceIdsByLocation[i1];
					rots[i2] = a->rotations[i1] + 1;

					if(rots[i2] > 3) rots[i2] = 0;

					newY++;
				}

				newX--;
			}
		break;

		// Rotate 180 degrees
		case 1:
			newY = Y + W - 1;

			for(int y = Y; y < Y + W; y++)
			{
				newX = X + W - 1;
				
				for(int x = X; x < X + W; x++)
				{
					i1 = y*xSize + x;
					i2 = newY*xSize + newX;

					idsByLoc[i2] = a->pieceIdsByLocation[i1];
					rots[i2] = a->rotations[i1] + 2;

					if(rots[i2] > 3) rots[i2] -= 4;

					newX--;
				}

				newY--;
			}
		break;

		// Rotate 270 degrees
		case 2:
			newX = X;

			for(int y = Y; y < Y + W; y++)
			{
				newY = Y + W - 1;
				
				for(int x = X; x < X + W; x++)
				{
					i1 = y*xSize + x;
					i2 = newY*xSize + newX;

					idsByLoc[i2] = a->pieceIdsByLocation[i1];
					rots[i2] = a->rotations[i1] + 3;

					if(rots[i2] > 3) rots[i2] -= 4;

					newY--;
				}

				newX++;
			}
		break;
	}

	for(int i = 0; i < totalSize; i++)
		locsById[idsByLoc[i]] = i;

	b->SetBoard(idsByLoc, locsById, rots);
	
	//newPopulation[globalIndex++] = b;
	globalIndex++;
}

inline void GeneticAlgorithm::assignNewPopulation()
{
	//for(int i = elitismAmount; i < populationSize; i++)
	//	delete population[i];

	//delete [] population;

	tempPopulation = population;
	population = newPopulation;
	newPopulation = tempPopulation;

	globalIndex = 0;
}

inline void GeneticAlgorithm::calculateObjectives()
{
	pointCalculator->Calculate(population);
}

inline void GeneticAlgorithm::calculatePointsMOEA()
{
	for(int i = 0; i < populationSize; i++)
		population[i]->combinedPoints = population[i]->dominance + (1 - population[i]->distance);
}

inline int GeneticAlgorithm::tournamentSelection()
{
	int min = 2147483647, temp;
	
	for(int i = 0; i < tournamentSize; i++)
	{
		temp = rand() % populationSize;

		if(temp < min) min = temp;
	}

	return min;
}