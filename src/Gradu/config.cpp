#include "config.h"

Config::Config()
{
	dict = new map<string, string>;
}

Config::~Config()
{
	delete dict;
	dict = 0;
}

bool Config::LoadConfig(string path)
{
	dict->clear();
	ifstream configFile(path.c_str());
	vector<string> tokens;
	
	if(configFile.is_open())
	{
		while(configFile.good())
		{
			tokens.clear();
			getline(configFile, str);

			if(str.length() < 2 || str.at(0) == '#')
				continue;

			istringstream iss(str);

			copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string>>(tokens));

			dict->insert(pair<string, string>(tokens[0], tokens[1]));
		}
	}
	
	else
	{
		cout << "Couldn't open config file '" << path << "'!" << endl;
		return false;
	}

	configFile.close();

	return true;
}

bool Config::findValue(string key, string& value)
{
	map<string, string>::iterator iter;
	iter = dict->find(key);

	if(iter != dict->end())
	{
		 value = iter->second;
		 return true;
	}

	else
	{
		value = "";
		cout << "Couldn't find key '" << key << "' from config file!" << endl;
		return false;
	}
}

bool Config::GetString(string key, string& value)
{
	if(findValue(key, str))
	{
		value = str;
		return true;
	}

	else
	{
		value = "";
		return false;
	}	
}

bool Config::GetDouble(string key, double& value)
{
	if(findValue(key, str))
	{
		istringstream iss;
		iss.str(str);
		iss >> value;
		return true;
	}

	else
	{
		value = 0.0;
		return false;
	}	
}

bool Config::GetBool(string key, bool& value)
{
	if(findValue(key, str))
	{
		if(str.compare("true") == 0 || str.compare("True") == 0 || str.compare("TRUE") == 0)
		{
			value = true;
			return true;
		}

		else if(str.compare("false") == 0 || str.compare("False") == 0 || str.compare("FALSE") == 0)
		{
			value = false;
			return true;
		}

		else
		{
			value = false;
			return false;
		}
	}

	else
	{
		value = false;
		return false;
	}	
}

bool Config::GetInt(string key, int& value)
{
	if(findValue(key, str))
	{
		istringstream iss;
		iss.str(str);
		iss >> value;
		return true;
	}

	else
	{
		value = 0;
		return false;
	}	
}

bool Config::GetLong(string key, long& value)
{
	if(findValue(key, str))
	{
		istringstream iss;
		iss.str(str);
		iss >> value;
		return true;
	}

	else
	{
		value = 0;
		return false;
	}	
}

string Config::GetConfigString()
{
	map<string, string>::iterator iter;

	stringstream ss;

	for(iter = dict->begin(); iter != dict->end(); iter++)
	{
		ss << iter->first << " " << iter->second << endl;
	}

	return ss.str();
}