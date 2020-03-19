#include "utils.h"

namespace Utils
{
	bool LoadEternityPieces(std::string piecePath, Piece*** eternityPieces)
	{
		// TODO: Remember to free memory reserved for pieces in the caller!
		std::string str;
		std::ifstream pieceFile(piecePath.c_str());
		std::vector<std::string> tokens;
		Piece** pieces = new Piece*[256];
		int i = 0;

		if(pieceFile.is_open())
		{
			while(pieceFile.good())
			{
				tokens.clear();
				getline(pieceFile, str);

				if(str.length() < 2 || str.at(0) == '#')
					continue;

				std::istringstream iss(str);

				copy(std::istream_iterator<std::string>(iss),
					std::istream_iterator<std::string>(),
					std::back_inserter<std::vector<std::string>>(tokens));
				
				pieces[i++] = new Piece(atoi(tokens[1].c_str()), atoi(tokens[2].c_str()), atoi(tokens[3].c_str()), atoi(tokens[4].c_str()), atoi(tokens[0].c_str()));
			}	
		}
	
		else
		{
			std::cout << "Couldn't open config file '" << piecePath << "'!" << std::endl;
			return false;
		}

		pieceFile.close();

		if(i < 256)
		{
			std::cout << "Error, read only " << i-1 << " of 256 pieces of Eternity II piece file!" << std::endl;
			return false;	
		}

		*eternityPieces = pieces;

		return true;
	}

	void ShuffleEternityPieces(MTRand_int32& rand, Piece** originalPieces, int* rotations)
	{
		Piece* temp;
		int index, rotation;

		for(int i = 255; i > 0; i--)
		{
			index = rand() % (i+1);
			rotations[i] = rotation = rand() % 4;

			temp = originalPieces[index];
			originalPieces[index] = originalPieces[i];
			originalPieces[i] = temp;
		}
	}

	void ShuffleIntArrays(MTRand_int32& rand, int* idsByLocation, int* rotations)
	{
		int index, temp;
		int rot;

		for(int i = 255; i >= 0; i--)
		{
			index = rand() % (i+1);
			rot = rand() % 4;
			rotations[i] = rot;
			//rotations[i] = rand() % 4;

			temp = idsByLocation[index];
			idsByLocation[index] = idsByLocation[i];
			idsByLocation[i] = temp;
		}
	}

	string PrintBoard(Board* b, int xSize, int ySize, bool printIds)
	{
		stringstream out;
		string BOARDWALL;
		int id;

		out << endl;

		if(!printIds)	BOARDWALL = "+------";
		else			BOARDWALL = "+---";

		if(!printIds)
		{
			for (int x = 0; x < xSize; x++)
				out << BOARDWALL;
		}

		else
		{
			for (int x = 0; x < xSize; x++)
			{
				out << BOARDWALL;
				id = b->pieceIdsByLocation[x];

				if(id < 10)			out << "--";
				else if(id < 100)	out << "-";

				out << id;
			}

		}

		out << "+" << endl;
	
		for (int y = 0; y < ySize; y++)
		{
			for (int x = 0; x < xSize; x++)
			{
				out << "|  ";
				if(b->PieceExists(y*ySize+x))
				{
					if (b->GetUp(y * xSize + x) < 10)
						out << " ";
				}

				else 
					out << "  ";

				if(b->PieceExists(y*xSize + x))
					out << b->GetUp(y * xSize + x) << "  ";

				else
					out << "  ";
			}

			out << "|" << endl;

			for (int x = 0; x < xSize; x++)
			{
				out << "|";
				if(b->PieceExists(y*xSize + x))
				{
					if (b->GetLeft(y * xSize + x) < 10) out << " ";
					out << b->GetLeft(y * xSize + x) << "  " << b->GetRight(y * xSize + x);
					if (b->GetRight(y * xSize + x) < 10) out << " ";
				}

				else
					out << "      ";
			}
			out << "|" << endl;

			for (int x = 0; x < xSize; x++)
			{
				out << "|  ";
				if(b->PieceExists(y*xSize + x))
				{
					if (b->GetDown(y * xSize + x) < 10) out << " ";
						out << b->GetDown(y * xSize + x) << "  ";
				}

				else out << "    ";			

			}
			out << "|" << endl;
			
			if(!printIds)
			{
				for (int x = 0; x < xSize; x++) out << BOARDWALL;
			}
			
			else
			{
				for (int x = 0; x < xSize; x++)
				{
					out << BOARDWALL;

					if(y != ySize-1)
					{
						id = b->pieceIdsByLocation[(y+1)*xSize+x];

						if(id < 10)			out << "--";
						else if(id < 100)	out << "-";

						out << id;
					}

					else
						out << "---";
				}

			}
			out << "+" << endl;
		}

		out << endl;

		return out.str();
	}

	long getCurrentTime()
	{
#ifdef _WIN32
		SYSTEMTIME st;
		GetSystemTime(&st);
		stringstream ss;
		long retVal;

		ss << st.wHour << st.wMinute << st.wSecond << st.wMilliseconds;
		ss >> retVal;

		return retVal;
#else
		// Warning, not tested!
		return gettimeofday();
#endif
	}

	string DoubleToString(double d)
	{
		ostringstream oss;
		oss << d;
		return oss.str();
	}
}