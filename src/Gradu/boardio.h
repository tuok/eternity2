#ifndef BOARD_IO_H
#define BOARD_IO_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "fileio.h"
#include "board.h"

using namespace std;

class BoardIO
{
	private:
		int totalSize;
		int totalObjectives;
		Board board;
		bool error;
		string filename;
		char separator;
		vector<string> data;
		BoardIO();
		string boardToText(Board *b);
		void textToBoard(string line, Board *to);

	public:
		BoardIO(string filename, int totalSize, int totalObjectives);
		bool Write(Board *b);
		bool Write(Board **population, int populationSize);
		bool Read(Board *b);
		bool Read(Board **population, int populationSize);
		void Test(Board *b1, Board *b2);
};



#endif