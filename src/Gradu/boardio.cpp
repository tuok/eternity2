#include "boardio.h"

BoardIO::BoardIO() {}

BoardIO::BoardIO(string filename, int totalSize, int totalObjectives)
{
	error = false;

	if(filename.length() < 1)
	{
		cout << "Error, filename must contain at least one character!" << endl;
		error = true;
	}

	this->filename = filename;
	this->totalSize = totalSize;
	this->totalObjectives = totalObjectives;
	this->separator = ';';

	board.pieceLocationsById = new int[totalSize];
	board.pieceIdsByLocation = new int[totalSize];
	board.rotations = new int[totalSize];
	board.objectives = new int[totalObjectives];
}

bool BoardIO::Write(Board *b)
{
	data.clear();
	data.push_back(boardToText(b));

	return FileIO::WriteFile(filename, data);
}

bool BoardIO::Write(Board **population, int populationSize)
{
	string line;
	data.clear();

	for(int i = 0; i < populationSize; i++)
		data.push_back(boardToText(population[i]));

	bool result = FileIO::WriteFile(filename, data);

	return result;
}

bool BoardIO::Read(Board *b)
{
	data.clear();

	FileIO::ReadFile(filename, data);

	if(data.size() == 1)
	{
		textToBoard(data[0], b);
		return true;
	}

	else
	{
		cout << "File \"" << filename << "\" contained " << data.size() << " lines instead of single line!" << endl;
		return false;
	}
}

bool BoardIO::Read(Board **population, int populationSize)
{
	data.clear();

	FileIO::ReadFile(filename, data);

	if(data.size() == populationSize)
	{
		for(int i = 0; i < populationSize; i++)
			textToBoard(data[i], population[i]);

		return true;
	}

	else
	{
		cout << "File \"" << filename << "\" contained " << data.size() << " lines instead of expected " << populationSize << " lines!" << endl;
		return false;
	}
}

string BoardIO::boardToText(Board *b)
{
	// Writing order: pieceLocationsById, pieceIdsByLocation, rotations, objectives, combinedPoints, dominance, distance, 
	stringstream ss;

	int pieceLocById;
	int pieceIdByLoc;
	int rot;
	int obj;
	double comb;
	double dom;
	double dist;
	
	for(int i = 0; i < totalSize; i++)
	{
		pieceLocById = b->pieceLocationsById[i];

		if(pieceLocById >= 0)	ss << pieceLocById << separator;
		else	cout << "ERROR!" << endl;
	}

	for(int i = 0; i < totalSize; i++)
	{
		pieceIdByLoc = b->pieceIdsByLocation[i];

		if(pieceIdByLoc >= 0)	ss << pieceIdByLoc << separator;
		else	cout << "ERROR!" << endl;
	}

	for(int i = 0; i < totalSize; i++)
	{
		rot = b->rotations[i];
		if(rot >= 0)	ss << rot << separator;
		else	cout << "ERROR!" << endl;
	}

	for(int i = 0; i < totalObjectives; i++)
	{
		obj = b->objectives[i];
		if(obj >= 0)	ss << obj << separator;
		else			ss << -1 << separator;
	}

	comb = b->combinedPoints;
	dom = b->dominance;
	dist = b->distance;

	if(comb >= 0.0)	ss << b->combinedPoints << separator;
	else			ss << -1 << separator;

	if(dom >= 0.0)	ss << b->dominance << separator;
	else			ss << -1 << separator;

	if(dist >= 0.0)	ss << b->distance;
	else			ss << -1;

	return ss.str();
}

void BoardIO::Test(Board *b1, Board *b2)
{
	string board = boardToText(b1);
	textToBoard(board, b2);

	cout << "\n\npieceLocationsById:\n\n";
	for(int i = 0; i < totalSize; i++)
	{
		cout << b1->pieceLocationsById[i] << ":" << b2->pieceLocationsById[i];
		if(b1->pieceLocationsById[i] != b2->pieceLocationsById[i])
			cout << " <--";

		cout << endl;
	}
	
	cout << "\n\npieceIdsByLocation:\n\n";
	for(int i = 0; i < totalSize; i++)
	{
		cout << b1->pieceIdsByLocation[i] << ":" << b2->pieceIdsByLocation[i];
		if(b1->pieceIdsByLocation[i] != b2->pieceIdsByLocation[i])
			cout << " <--";

		cout << endl;
	}
	cout << "\n\rotations:\n\n";
	for(int i = 0; i < totalSize; i++)
	{
		cout << b1->rotations[i] << ":" << b2->rotations[i];
		if(b1->rotations[i] != b2->rotations[i])
			cout << " <--";

		cout << endl;
	}
}

void BoardIO::textToBoard(string line, Board* to)
{
	istringstream iss(line);
	string temp;
	
	int i = 0;
	int part = 0;
	int tempInt = 0;
	int tempDouble = 0;

	// Readingorder: pieceLocationsById, pieceIdsByLocation, rotations, objectives, combinedPoints, dominance, distance, 
	while(getline(iss, temp, separator))
	{
		if(part == 0)
		{
			board.pieceLocationsById[i++] = atoi(temp.c_str());

			if(i >= totalSize)
			{
				part++;
				i = 0;
			}
		}

		else if(part == 1)
		{
			board.pieceIdsByLocation[i++] = atoi(temp.c_str());

			if(i >= totalSize)
			{
				part++;
				i = 0;
			}
		}

		else if(part == 2)
		{
			board.rotations[i++] = atoi(temp.c_str());

			if(i >= totalSize)
			{
				part++;
				i = 0;
			}
		}

		else if(part == 3)
		{
			board.objectives[i++] = atoi(temp.c_str());

			if(i >= totalObjectives)
			{
				part++;
				i = 0;
			}
		}

		else if(part == 4)
		{
			board.combinedPoints = atof(temp.c_str());
			part++;
		}

		else if(part == 5)
		{
			board.dominance = atoi(temp.c_str());
			part++;
		}

		else if(part == 6)
		{
			board.distance = atof(temp.c_str());
			part++;
		}

		else
			cout << "Too many parts!" << endl;
	}

	for(int i = 0; i < totalSize; i++)
	{
		to->rotations[i] = board.rotations[i];
		to->pieceIdsByLocation[i] = board.pieceIdsByLocation[i];
		to->pieceLocationsById[i] = board.pieceLocationsById[i];

		if(i < 4)
			to->objectives[i] = board.objectives[i];
	}

	to->combinedPoints = board.combinedPoints;
	to->dominance = board.dominance;
	to->distance = board.distance;
}