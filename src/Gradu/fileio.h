#ifndef FILEIO_H
#define FILEIO_H

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

using namespace std;

class FileIO
{
	public:
		static bool ReadFile(string filename, vector<string> &data);
		static bool WriteFile(string filename, vector<string> data);
		static bool WriteFile(string filename, string data);
};

#endif

