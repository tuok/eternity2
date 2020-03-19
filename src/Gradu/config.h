#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;

class Config
{
    private:
		string str;
		map<string, string>* dict;

		bool findValue(string key, string& value);

    public:
		Config();
		~Config();
		bool LoadConfig(string path);
		bool GetString(string key, string& value);
		bool GetInt(string key, int& value);
		bool GetDouble(string key, double& value);
		bool GetBool(string key, bool& value);
		bool GetLong(string key, long& value);
		string GetConfigString();
};

#endif