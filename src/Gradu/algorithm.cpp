#include "algorithm.h"

Algorithm::Algorithm()
{
	totalObjectives = 4;
	generation = 0;
	error = false;
	ready = false;
	seed = unsigned long(Utils::getCurrentTime());
	totalSize = 256;
	xSize = 16;
	ySize = 16;
}

Algorithm::~Algorithm()
{
	if(!error)
	{
		for(int i = 0; i < 256; i++)
			delete eternityPieces[i];

		delete [] eternityPieces;
		eternityPieces = 0;
	}
}

bool Algorithm::IsError()
{
	return error;
}

void Algorithm::printPoints(Board *board)
{
	out.str("");
	
	out << generation;
	if(obj1Weight > 0) out << ";" << board->objectives[0];
	if(obj2Weight > 0) out << ";" << board->objectives[1];
	if(obj3Weight > 0) out << ";" << board->objectives[2];
	if(obj4Weight > 0) out << ";" << board->objectives[3];

	if(algorithmType.compare(Constants::MOEA) != 0)
		out << ";" << board->combinedPoints;

	out << endl;

	output << out.str();
	cout << out.str();
}

void Algorithm::printPoints(Board *board, double T)
{
	out.str("");
	
	out << generation;
	if(obj1Weight > 0) out << ";" << board->objectives[0];
	if(obj2Weight > 0) out << ";" << board->objectives[1];
	if(obj3Weight > 0) out << ";" << board->objectives[2];
	if(obj4Weight > 0) out << ";" << board->objectives[3];

	if(algorithmType.compare(Constants::MOEA) != 0)
		out << ";" << board->combinedPoints;

	out << ";" << T;

	out << endl;

	output << out.str();
	cout << out.str();
}

void Algorithm::printHeader()
{
	output << cfg.GetConfigString() << endl;
	out.str("");

	out << "Generation";
	if(obj1Weight > 0) out << ";Side points (max: " << maxSidePoints << ")";
	if(obj2Weight > 0) out << ";Square points (max: " << maxSquarePoints << ")";
	if(obj3Weight > 0) out << ";Cross points (max: " << maxCrossPoints << ")";
	if(obj4Weight > 0) out << ";Edge Points (max: " << maxEdgePoints << ")";
	if(algorithmType.compare(Constants::MOEA) != 0)
		out << ";Combined Points (max: 1.0)";

	if(algorithmType.compare(Constants::SA) == 0)
		out << ";Temperature";

	out << endl;

	output << out.str();
	cout << out.str();
}

void Algorithm::saveLogGA(int E, int C, int M, int CMinB, int CMaxB, int MMinB, int MMaxB, int TS, int points)
{
	// E = ElitismAmount, C = CrossoverAmount, M = MutationAmount, CMinB = CrossoverMinBound, CMaxB = CrossoverMaxBound, MMinB = MutationMinBound
	// MMaxB = MutationMaxBound, TS = TournamentSize, Obj = Objectives, EP = EdgesAsPoints, S = SolveEdges, Seed = Seed
	string objString = "";
	objString += Utils::DoubleToString(obj1Weight); objString += ";";
	objString += Utils::DoubleToString(obj2Weight); objString += ";";
	objString += Utils::DoubleToString(obj3Weight); objString += ";";
	objString += Utils::DoubleToString(obj4Weight); objString += ";";

	out.str("");
	
	out << algorithmType << "_" << points << "_E" << E << "_C" << C << "_M" << M << "_CMinB" << CMinB << "_CMaxB"	<< CMaxB << "_MMinB" << MMinB
		<< "_MMaxB" << MMaxB << "_TS" << TS << "_Obj" << objString << "_S" << solveEdges	<< "_Seed" << seed << ".txt";

	FileIO::WriteFile(out.str(), output.str());
}

void Algorithm::saveLogHC_GA(int MMinB, int MMaxB, int points)
{
	// E = ElitismAmount, C = CrossoverAmount, M = MutationAmount, CMinB = CrossoverMinBound, CMaxB = CrossoverMaxBound, MMinB = MutationMinBound
	// MMaxB = MutationMaxBound, TS = TournamentSize, Obj = Objectives, EP = EdgesAsPoints, S = SolveEdges, MFB = ModifyFailedBoard, SD = Seed
	string objString = "";
	objString += Utils::DoubleToString(obj1Weight); objString += ";";
	objString += Utils::DoubleToString(obj2Weight); objString += ";";
	objString += Utils::DoubleToString(obj3Weight); objString += ";";
	objString += Utils::DoubleToString(obj4Weight); objString += ";";

	string mfbString = modifyFailedBoard ? "x" : "o";

	out.str("");
	
	out << algorithmType << "_GA_" << points << "_Obj" << objString << "_MMinB" << MMinB << "_MMaxB" << MMaxB 
		<< "_S" << solveEdges << "_MFB" << mfbString << "_SD" << seed << ".txt";

	FileIO::WriteFile(out.str(), output.str());
}

void Algorithm::saveLogHC_BF(int points)
{
	// E = ElitismAmount, C = CrossoverAmount, M = MutationAmount, CMinB = CrossoverMinBound, CMaxB = CrossoverMaxBound, MMinB = MutationMinBound
	// MMaxB = MutationMaxBound, TS = TournamentSize, Obj = Objectives, EP = EdgesAsPoints, S = SolveEdges, MFB = ModifyFailedBoard, SD = Seed
	string objString = "";
	objString += Utils::DoubleToString(obj1Weight); objString += ";";
	objString += Utils::DoubleToString(obj2Weight); objString += ";";
	objString += Utils::DoubleToString(obj3Weight); objString += ";";
	objString += Utils::DoubleToString(obj4Weight); objString += ";";

	string mfbString = modifyFailedBoard ? "x" : "o";

	out.str("");
	
	out << algorithmType << "_BF_" << points << "_Obj" << objString << "_S" << solveEdges << "_MFB" << mfbString << "_SD" << seed << ".txt";

	FileIO::WriteFile(out.str(), output.str());
}

void Algorithm::saveLogSA_BF(int points, double alpha, double epsilon, double T, bool allowSamePoints)
{
	string objString = "";
	objString += Utils::DoubleToString(obj1Weight); objString += ";";
	objString += Utils::DoubleToString(obj2Weight); objString += ";";
	objString += Utils::DoubleToString(obj3Weight); objString += ";";
	objString += Utils::DoubleToString(obj4Weight); objString += ";";

	string allowSamePointsString = allowSamePoints ? "x" : "o";

	out.str("");
	
	out << algorithmType << "_BF_" << points << "_Obj" << objString << "_a" << alpha << "_e" << epsilon << "_T" << T << "_ASP" << allowSamePoints << "_SD" << seed << ".txt";

	FileIO::WriteFile(out.str(), output.str());
}

void Algorithm::saveLogSA_GA(int MMinB, int MMaxB,int points, double alpha, double epsilon, double T, bool allowSamePoints, double mutationRatio)
{
	string objString = "";
	objString += Utils::DoubleToString(obj1Weight); objString += ";";
	objString += Utils::DoubleToString(obj2Weight); objString += ";";
	objString += Utils::DoubleToString(obj3Weight); objString += ";";
	objString += Utils::DoubleToString(obj4Weight); objString += ";";

	string allowSamePointsString = allowSamePoints ? "x" : "o";

	out.str("");
	
	out << algorithmType << "_BF_" << points << "_Obj" << objString << "_a" << alpha << "_e" << epsilon << "_T" << T << "_MMinB" << MMinB << "_MMaxB" << MMaxB << "_MR" << mutationRatio << "_ASP"
		<< allowSamePointsString << "_SD" << seed << ".txt";

	FileIO::WriteFile(out.str(), output.str());
}
