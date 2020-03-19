#ifndef UTILS_H
#define UTILS_H

#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <string>
#include <sstream>
#include <ctime>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include "mtrand.h"
#include "piece.h"
#include "board.h"

namespace Utils
{
	extern bool LoadEternityPieces(std::string piecePath, Piece*** eternityPieces);
	extern void ShuffleEternityPieces(MTRand_int32& rand, Piece** originalPieces, int* rotations);
	extern void ShuffleIntArrays(MTRand_int32& rand, int* idsByLocation, int* rotations);
	extern string PrintBoard(Board* b, int xSize, int ySize, bool printIds);
	extern long getCurrentTime();
	extern string DoubleToString(double d);
}

#endif