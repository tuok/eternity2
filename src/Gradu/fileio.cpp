#include <iostream>
#include <cstdlib>
#include "fileio.h"

// Read text file and place it's contents to parameter vector lines
bool FileIO::ReadFile(string filename, vector<string> &data)
{
	if(filename.compare("") == 0)
	{
		cout << "Error! Filename was empty!" << endl;
		return false;
	}
	
	ifstream file(filename.c_str(), ios::in);
	string temp;
	
	if(file.is_open())
	{
		while(file.peek() != EOF)
		{
			getline(file, temp);
			data.push_back(temp);
		}
			
		file.close();
	}
	
	else
	{
		cout << "Error! File \"" << filename << "\" could not be opened!" << endl;
		return false;
	}
	
	return true;
}


bool FileIO::WriteFile(string filename, vector<string> data)
{
	if(filename.compare("") == 0)
	{
		cout << "Error! Filename was empty!" << endl;
		return false;
	}

	ofstream file(filename.c_str(), ios::trunc | ios::out);

	if(file.is_open())
	{
		for(unsigned int i = 0; i < data.size(); i++)
			file << data[i] << endl;

		file.close();
	}

	else
	{
		cout << "Error! File \"" << filename << "\" could not be opened for writing!" << endl;
		return false;
	}

	return true;
}

bool FileIO::WriteFile(string filename, string data)
{
	if(filename.compare("") == 0)
	{
		cout << "Error! Filename was empty!" << endl;
		return false;
	}

	ofstream file(filename.c_str(), ios::trunc | ios::out);

	if(file.is_open())
	{
		file << data << endl;
		file.close();
	}

	else
	{
		cout << "Error! File \"" << filename << "\" could not be opened for writing!" << endl;
		return false;
	}

	return true;
}

