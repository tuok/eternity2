#include "hcsa.h"

HCSA::HCSA(string configFilePath)
{
	// Load contents of provided config file
	if(cfg.LoadConfig(configFilePath))
	{
		// Parse all needed parameters from config
		loadParametersFromConfig();

		// Try to load Eternity pieces to array of Piece pointers
		if(!Utils::LoadEternityPieces(eternityPiecePath, &eternityPieces))
			error = true;

		idsByLocation = new int[totalSize];
		locationsById = new int[totalSize];
		rotations = new int[totalSize];

		for(int i = 0; i < totalSize; i++)
			idsByLocation[i] = i;

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
			}

			else
			{
				cout << "Random seed: " << seed << endl;
				rand.seed(seed);
			}

			board = new Board();
			tempBoard = new Board();

			Utils::ShuffleIntArrays(rand, idsByLocation, rotations);

			for(int j = 0; j < totalSize; j++)
				locationsById[idsByLocation[j]] = j;

			board->Init(eternityPieces, idsByLocation, locationsById, rotations, totalSize, xSize, ySize);
			tempBoard->Init(eternityPieces, idsByLocation, locationsById, rotations, totalSize, xSize, ySize);

			pointCalculator = new PointCalculator(board, xSize, ySize, maxSidePoints, maxSquarePoints, maxCrossPoints, maxEdgePoints, edgesAsPoints);
			pointCalculator->SetActiveObjectives(obj1Weight, obj2Weight, obj3Weight, obj4Weight);
			objectiveCount = pointCalculator->objectiveCount;

			generationsFromLastBest = 0;
			bestPoints = 0;

			T0 = T;

			if(solveEdges)
			{
				EdgeSolver es(xSize, ySize, 0);
				es.Solve(board);
				tempBoard->Clone(board);
				//Utils::PrintBoard(board, xSize, ySize);
			}
			pointCalculator->Calculate(board);
		}
	}

	else
		error = true;
}

void HCSA::loadParametersFromConfig()
{
	// Load needed parameters from config
	if(!cfg.GetString(Constants::MUTATIONTYPE, mutationType)) error = true;
	if(!cfg.GetInt(Constants::MUTATIONMINBOUND, mutationMinBound)) error = true;
	if(!cfg.GetInt(Constants::MUTATIONMAXBOUND, mutationMaxBound)) error = true;
	if(!cfg.GetInt(Constants::MAXGENERATIONS, maxGenerations)) error = true;
	if(!cfg.GetString(Constants::ALGORITHMTYPE, algorithmType)) error = true;
	if(!cfg.GetBool(Constants::EDGESASPOINTS, edgesAsPoints)) error = true;
	if(!cfg.GetString(Constants::ETERNITYPIECESPATH, eternityPiecePath)) error = true;
	if(!cfg.GetBool(Constants::SOLVEEDGES, solveEdges)) error = true;
	if(!cfg.GetDouble(Constants::OBJ1WEIGHT, obj1Weight)) error = true;
	if(!cfg.GetDouble(Constants::OBJ2WEIGHT, obj2Weight)) error = true;
	if(!cfg.GetDouble(Constants::OBJ3WEIGHT, obj3Weight)) error = true;
	if(!cfg.GetDouble(Constants::OBJ4WEIGHT, obj4Weight)) error = true;
	if(!cfg.GetLong(Constants::SEED, userSeed)) error = true;
	if(!cfg.GetBool(Constants::MODIFYFAILEDBOARD, modifyFailedBoard)) error = true;
	if(!cfg.GetBool(Constants::LINEARANNEALING, linearAnnealing)) error = true;
	if(!cfg.GetBool(Constants::ALLOWSAMEPOINTS, allowSamePoints)) error = true;
	if(!cfg.GetDouble(Constants::ALPHA, alpha)) error = true;
	if(!cfg.GetDouble(Constants::TEMPERATURE, T)) error = true;
	if(!cfg.GetDouble(Constants::EPSILON, epsilon)) error = true;
	if(!cfg.GetDouble(Constants::MUTATIONRATIO, mutationRatio)) error = true;
	if(!cfg.GetInt(Constants::IMPROVEMENTLIMIT, improvementLimit)) error = true;
}

HCSA::~HCSA()
{
	if(!error)
	{
		delete board;
		delete tempBoard;
		board = 0;
		tempBoard = 0;
	}
}

void HCSA::Run()
{
	// Pohdi myös vaihtoehtoa, jossa objective(j)a vaihdetaan kesken kaiken!
	if(algorithmType.compare(Constants::HC) == 0)
	{
		if(mutationType.compare(Constants::GA) == 0)
			runHC_GA();

		else if(mutationType.compare(Constants::BF) == 0)
			runHC_BF();
		
		else
		{
			error = true;
			cout << "Mutation type '" << mutationType << "' not implemented!" << endl;
		}
	}

	else if(algorithmType.compare(Constants::SA) == 0)
	{
		if(mutationType.compare(Constants::GA) == 0 || mutationType.compare(Constants::BF) == 0)
			runSA();
	
		else
		{
			error = true;
			cout << "Mutation type '" << mutationType << "' not implemented!" << endl;
		}
	}

	else
	{
		error = true;
		cout << "Algorithm type '" << algorithmType << "' not implemented!" << endl;
	}

	if(!error)
	{
		string finalBoard = Utils::PrintBoard(board, xSize, ySize, true);

		if(algorithmType.compare(Constants::HC) == 0)
			printPoints(board);

		else if(algorithmType.compare(Constants::SA) == 0)
			printPoints(board, T);

		cout << "Best result after " << generation << " generations: " << board->objectives[Constants::SidePoints] << "/" << maxSidePoints << endl;
		output << "Best result after " << generation << " generations: " << board->objectives[Constants::SidePoints] << "/" << maxSidePoints << endl;
		
		cout << finalBoard;
		output << finalBoard;

		if(algorithmType.compare(Constants::HC) == 0)
		{
			if(mutationType.compare(Constants::GA) == 0)
				saveLogHC_GA(mutationMinBound, mutationMaxBound, board->objectives[Constants::SidePoints]);

			else
				saveLogHC_BF(board->objectives[Constants::SidePoints]);
		}
		
		else if(algorithmType.compare(Constants::SA) == 0)
		{
			if(mutationType.compare(Constants::GA) == 0)
				saveLogSA_GA(mutationMinBound, mutationMaxBound, board->objectives[Constants::SidePoints], alpha, epsilon, T0, allowSamePoints, mutationRatio);

			else
				saveLogSA_BF(board->objectives[Constants::SidePoints], alpha, epsilon, T0, allowSamePoints);
		}
	}
}

bool HCSA::checkPoints()
{
	if(board->objectives[Constants::SidePoints] == maxSidePoints)
		return true;

	generationsFromLastBest++;

	if(board->combinedPoints > bestPoints)
	{
		bestPoints = board->combinedPoints;
		generationsFromLastBest = 0;
	}

	if(generationsFromLastBest >= improvementLimit)
	{
		//cout << "GenFromLastBest: " << generationsFromLastBest << endl;
		//cout << "Improvementlimit: " << improvementLimit << endl;
		//cout << "bestPoints: " << bestPoints << endl;
		//cout << "board->combinedPoints: " << board->combinedPoints << endl;
		return true;
	}

	return false;
}

void HCSA::runHC_GA()
{
	printHeader();
	double beginningPoints = 0;

	while(!checkPoints())
	{
		generation++;
		//if(beginningPoints == board->combinedPoints)
		//	break;

		//beginningPoints = board->combinedPoints;

		if(randFloat() < mutationRatio)	exchangeMutation();
		else							rotateMutation();

		pointCalculator->Calculate(tempBoard);

		if(tempBoard->combinedPoints > board->combinedPoints)
		{
			board->Clone(tempBoard);
		
			for(int o = 0; o < totalObjectives; o++)
				board->objectives[o] = tempBoard->objectives[o];

			board->combinedPoints = tempBoard->combinedPoints;
			printPoints(board);
		}

		else
		{
			if(!modifyFailedBoard)
				tempBoard->Clone(board);
		}
	}
}

void HCSA::runSA()
{
	bool geneticSA = false;

	if(mutationType.compare(Constants::GA) == 0)
		geneticSA = true;

	MTRand randFloat;
	printHeader();
	pointCalculator->Calculate(tempBoard);
	double currentPoints = tempBoard->combinedPoints;
	double prob;
	double exp_function;
	double bestPoints = 0.0;
	double tempDelta = 0.0;
	int i, j;

	if(linearAnnealing)
	{
		if(maxGenerations == 0)
			tempDelta = 1.0;

		else
			tempDelta = T / maxGenerations;
	}

	int zeroDeltaCount = 0;
	int negDeltaCount = 0;
	int posDeltaCount = 0;
	
	while(++generation < maxGenerations && !checkPoints() && T > epsilon)
	{
		//board->CheckDuplicatePieces();
		//tempBoard->CheckDuplicatePieces();

		//if(generation == 100000)
		//	generation = 100000;

		if(geneticSA)
		{
			if(randFloat() < mutationRatio)	exchangeMutation();
			else							rotateMutation();
		}

		else
		{
			i = rand() % totalSize; j = rand() % totalSize;
			swapAction(i, j, true);
		}

		pointCalculator->Calculate(tempBoard);
		delta = tempBoard->combinedPoints - currentPoints;

		if(delta > 0)
		{
			posDeltaCount++;

			currentPoints += delta;

			if(currentPoints > bestPoints)
			{
				bestPoints = currentPoints;
				//printPoints(board, T);
			}

			board->Clone(tempBoard);
		
			for(int o = 0; o < totalObjectives; o++)
				board->objectives[o] = tempBoard->objectives[o];

			board->combinedPoints = tempBoard->combinedPoints;
			printPoints(board, T);

		}
		
		else
		{
			if(delta == 0.0)
				zeroDeltaCount++;

			else
				negDeltaCount++;


			prob = randFloat();
			exp_function = exp(delta / T);
			/*
			if(exp_function < 1.0)
			{
				if(prob < exp_function)
					cout << prob << " < " << exp_function << " (prob < exp_function. delta = " << delta << ", T = " << T << ")" <<  endl;

				else if(prob > exp_function)
					cout << prob << " > " << exp_function << " (prob > exp_function. delta = " << delta << ", T = " << T << ")" <<  endl;

				else
					cout << prob << " == " << exp_function << " (prob == exp_function. delta = " << delta << ", T = " << T << ")" <<  endl;
			}
			*/
			// Even if board is worse than old one, update board if random number falls below exp_function
			if( (allowSamePoints || (!allowSamePoints && delta != 0)) && prob < exp_function)
			{
				currentPoints += delta;
				board->Clone(tempBoard);
				//cout << "-";
				//printPoints(board);
			}

			// Otherwise revert all changes made to tempBoard IF ModifyFailedBoard isn't active
			else
			{
				if(!modifyFailedBoard)
					tempBoard->Clone(board);
			}
		}

		if(linearAnnealing)
			T -= tempDelta;

		else
			T *= alpha;		
	}

	cout << "Negative delta count: " << negDeltaCount << endl;
	cout << "Zero delta count: " << zeroDeltaCount << endl;
	cout << "Positive delta count: " << posDeltaCount << endl;
	cout << "Generations: " << generation << endl;
}

void HCSA::runHC_BF()
{
	// Hill climbing should pick always the best solution, but here it picks first solution that increases points.
	// That is because most of the solutions yield worse points, so there's probably no need for expensive exhaustive
	// search for (marginally) best solution

	printHeader();
	
	int counter = 0;
	double beginningPoints = 0, bestPoints = 0;
	int r, iGoal, jGoal, idleRounds = 0;

	pointCalculator->Calculate(tempBoard);
	//printPoints(board);

	while(!checkPoints() && ++generation < maxGenerations)
	{
		if(idleRounds >= 50 || !improvement)
			break;

		r = rand() % totalSize;
		iGoal = r - 1;
		if(iGoal < 0) iGoal += totalSize;

		jGoal = r;
		if(jGoal < 0)	jGoal += totalSize;

		// Loop through the board starting from random index
		for(int i = r; i != iGoal; i++)
		{
			if(i == totalSize) i = 0;

			for(int j = i+1; j != jGoal; j++)
			{

				if(j == totalSize)
				{
					j = 0;
					if(j == jGoal)	break;
				}

				swapAction(i, j, false);

				if(improvement)
					break;
			}

			if(improvement)
			{
				//if(generation == 20000)
				//	cout << Utils::PrintBoard(board, xSize, ySize, true);

				pointCalculator->Calculate(board);
				printPoints(board);

				if(board->combinedPoints > bestPoints)
				{
					bestPoints = board->combinedPoints;
					idleRounds = 0;
				}

				else
					idleRounds++;


				break;
			}
		}
	}
}

void HCSA::swapAction(int i, int j, bool useTemp)
{
	if(i == j) return;

	improvement = false;
	swapped = false;
	ijMaxPts = iMaxPts = jMaxPts = iMaxSwapPts = jMaxSwapPts = ijMaxSwapPts = 0.0;
	iRot = jRot = iSwapRot = jSwapRot = -1;

	// If i and j are in adjacent cells so all rotations must be tested
	if(i == j+xSize || i == j-xSize || (i == j-1 && (i / 16 == j / 16) ) || (i == j+1 && (i / 16 == j / 16)))
	{
		if(i == j-xSize) iDir = Constants::DOWN;
		else if(i == j+xSize) iDir = Constants::UP;
		else if(i == j-1) iDir = Constants::RIGHT;
		else if(i == j+1) iDir = Constants::LEFT;

		if(useTemp)
		{
			ijMaxPts = pointCalculator->CalculateAdjacentIndex(tempBoard, i, iDir) + pointCalculator->CalculateIndex(tempBoard, j);
			tempBoard->Rotate(j);
		}

		else
		{
			ijMaxPts = pointCalculator->CalculateAdjacentIndex(board, i, iDir) + pointCalculator->CalculateIndex(board, j);
			board->Rotate(j);
		}

		for(int r1 = 0; r1 < 4; r1++)
		{
			for(int r2 = 0; r2 < 4; r2++)
			{
				testChange(i, j, false, true, useTemp);
				if(r1 == 0 && r2 == 0)	r2++;
				if(useTemp)	tempBoard->Rotate(j);
				else		board->Rotate(j);
			}

			if(useTemp)	tempBoard->Rotate(i);
			else		board->Rotate(i);
		}

		swap(i, j, useTemp);

		for(int r1 = 0; r1 < 4; r1++)
		{
			for(int r2 = 0; r2 < 4; r2++)
			{
				testChange(i, j, true, true, useTemp);
				if(useTemp)	tempBoard->Rotate(j);
				else		board->Rotate(j);
			}

			if(useTemp)	tempBoard->Rotate(i);
			else		board->Rotate(i);
		}

		if(!this->swapped)
			swap(i, j, useTemp);

		if(iRot != -1)
		{
			if(useTemp)	tempBoard->rotations[i] = iRot;
			else		board->rotations[i] = iRot;
			improvement = true;
		}
		if(jRot != -1)
		{
			if(useTemp)	tempBoard->rotations[j] = jRot;
			else		board->rotations[j] = jRot;
			improvement = true;
		}
		
	}

	// i and ja are NOT in adjacent cells, only 4 rotations for both pieces have to be tested
	else
	{
		//ijMaxPts = pointCalculator->CalculateIndex(board, i) + pointCalculator->CalculateIndex(board, j);
		iMaxPts = pointCalculator->CalculateIndex(board, i);
		jMaxPts = pointCalculator->CalculateIndex(board, j);
		ijMaxPts = iMaxPts + jMaxPts;

		if(useTemp)
		{
			tempBoard->Rotate(i);
			tempBoard->Rotate(j);
		}

		else
		{
			board->Rotate(i);
			board->Rotate(j);
		}

		for(int r = 0; r < 3; r++)
		{
			testChange(i, j, false, false, useTemp);

			if(useTemp)
			{
				tempBoard->Rotate(i);
				tempBoard->Rotate(j);
			}

			else
			{
				board->Rotate(i);
				board->Rotate(j);
			}
		}

		swap(i, j, useTemp);
		

		for(int r = 0; r < 4; r++)
		{
			testChange(i, j, true, false, useTemp);

			if(useTemp)
			{
				tempBoard->Rotate(i);
				tempBoard->Rotate(j);
			}

			else
			{
				board->Rotate(i);
				board->Rotate(j);
			}
		}

		if(ijMaxSwapPts > ijMaxPts)
			this->swapped = true;

		if(!this->swapped)
		{
			swap(i, j, useTemp);

			if(iRot != -1)
			{
				if(useTemp)	tempBoard->rotations[i] = iRot;
				else		board->rotations[i] = iRot;
				improvement = true;
			}
			if(jRot != -1)
			{
				if(useTemp)	tempBoard->rotations[j] = jRot;
				else		board->rotations[j] = jRot;
				improvement = true;
			}
		}

		else
		{
			if(iSwapRot != -1)
			{
				if(useTemp)	tempBoard->rotations[i] = iSwapRot;
				else		board->rotations[i] = iSwapRot;
				improvement = true;
			}
			if(jSwapRot != -1)
			{
				if(useTemp)	tempBoard->rotations[j] = jSwapRot;
				else		board->rotations[j] = jSwapRot;
				improvement = true;
			}		
		}
	}
}

void HCSA::testChange(int i, int j, bool swapped, bool adjacent, bool useTemp)
{
	if(adjacent)
	{
		if(useTemp)
			ijPts = pointCalculator->CalculateAdjacentIndex(tempBoard, i, iDir) + pointCalculator->CalculateIndex(tempBoard, j);

		else
			ijPts = pointCalculator->CalculateAdjacentIndex(board, i, iDir) + pointCalculator->CalculateIndex(board, j);

		if(ijPts > ijMaxPts)
		{
			ijMaxPts = ijPts;

			if(useTemp)
			{
				iRot = tempBoard->rotations[i];
				jRot = tempBoard->rotations[j];
			}

			else
			{
				iRot = board->rotations[i];
				jRot = board->rotations[j];
			}

			this->swapped = swapped;
		}
	}

	else
	{
		if(useTemp)
		{
			iPts = pointCalculator->CalculateIndex(tempBoard, i);
			jPts = pointCalculator->CalculateIndex(tempBoard, j);
		}

		else
		{
			iPts = pointCalculator->CalculateIndex(board, i);
			jPts = pointCalculator->CalculateIndex(board, j);
		}

		if(!swapped)
		{
			if(iPts > iMaxPts)
			{
				iMaxPts = iPts;
				if(useTemp)	iRot = tempBoard->rotations[i];
				else		iRot = board->rotations[i];
				this->swapped = false;
				ijMaxPts = iPts + jPts;
			}

			if(jPts > jMaxPts)
			{
				jMaxPts = jPts;
				if(useTemp)	jRot = tempBoard->rotations[j];
				else		jRot = board->rotations[j];
				this->swapped = false;
				ijMaxPts = iPts + jPts;
			}
		}

		else
		{
			if(iPts > iMaxSwapPts)
			{
				iMaxSwapPts = iPts;
				if(useTemp)	iSwapRot = tempBoard->rotations[i];
				else		iSwapRot = board->rotations[i];
				ijMaxSwapPts = iMaxSwapPts + jMaxSwapPts;
			}

			if(jPts > jMaxSwapPts)
			{
				jMaxSwapPts = jPts;
				if(useTemp)	jSwapRot = tempBoard->rotations[j];
				else		jSwapRot = board->rotations[j];
				ijMaxSwapPts = iMaxSwapPts + jMaxSwapPts;
			}
		}


	}
}

void HCSA::swap(int i, int j, bool useTemp)
{
	if(useTemp)
	{
		int temp = tempBoard->pieceIdsByLocation[i];
		tempBoard->pieceIdsByLocation[i] = tempBoard->pieceIdsByLocation[j];
		tempBoard->pieceIdsByLocation[j] = temp;
		//tempBoard->CheckDuplicatePieces();
		//tempBoard->CheckLocationsAndIDs();
	}

	else
	{
		int temp = board->pieceIdsByLocation[i];
		board->pieceIdsByLocation[i] = board->pieceIdsByLocation[j];
		board->pieceIdsByLocation[j] = temp;
		//board->CheckLocationsAndIDs();
	}
}

void HCSA::exchangeMutation()
{
	for(int i = 0; i < totalSize; i++)
	{
		// TODO: Tässä voisi periaatteessa täyttää vain vakiona pysyvät indeksit, sillä vaihdettavat indeksit täytetään myöhemmin jokatapauksessa.
		idsByLocation[i] = board->pieceIdsByLocation[i];
		rotations[i] = board->rotations[i];
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

			idsByLocation[i2] = board->pieceIdsByLocation[i1];
			rotations[i2] = board->rotations[i1];

			idsByLocation[i1] = board->pieceIdsByLocation[i2];
			rotations[i1] = board->rotations[i2];
		}
	}

	for(int i = 0; i < totalSize; i++)
		locationsById[idsByLocation[i]] = i;

	tempBoard->SetBoard(idsByLocation, locationsById, rotations);
}

void HCSA::rotateMutation()
{
	for(int i = 0; i < totalSize; i++)
	{
		// TODO: Tässä voisi periaatteessa täyttää vain vakiona pysyvät indeksit, sillä vaihdettavat indeksit täytetään myöhemmin jokatapauksessa.
		idsByLocation[i] = board->pieceIdsByLocation[i];
		rotations[i] = board->rotations[i];
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

					idsByLocation[i2] = board->pieceIdsByLocation[i1];
					rotations[i2] = board->rotations[i1] + 1;

					if(rotations[i2] > 3) rotations[i2] = 0;

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

					idsByLocation[i2] = board->pieceIdsByLocation[i1];
					rotations[i2] = board->rotations[i1] + 2;

					if(rotations[i2] > 3) rotations[i2] -= 4;

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

					idsByLocation[i2] = board->pieceIdsByLocation[i1];
					rotations[i2] = board->rotations[i1] + 3;

					if(rotations[i2] > 3) rotations[i2] -= 4;

					newY--;
				}

				newX++;
			}
		break;
	}

	for(int i = 0; i < totalSize; i++)
		locationsById[idsByLocation[i]] = i;

	tempBoard->SetBoard(idsByLocation, locationsById, rotations);
}
