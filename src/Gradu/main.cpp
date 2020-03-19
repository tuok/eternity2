#include <iostream>
#include <string>

#include "genetic_algorithm.h"
#include "hcsa.h"

void printUsage(string);

int main(int argc, char* argv[])
{
	if(argc != 3)
		printUsage(argv[0]);

	else
	{
		Algorithm *alg;
		string algorithm = argv[1];
		string configPath = argv[2];

		if(algorithm.compare(Constants::GA) == 0)
			alg = new GeneticAlgorithm(configPath);

		else if(algorithm.compare(Constants::HCSA) == 0)
			alg = new HCSA(configPath);

		else
		{
			cout << "Error, '" << algorithm << "' is invalid algorithm! Valid algorithms are 'GA' and 'HCSA'." << endl;
			return 1;
		}

		if(alg->IsError())
		{
			cout << "Aborting due to error in algorithm initialization..." << endl;
			delete alg;
			return 1;
		}

		alg->Run();

		delete alg;
		return 0;
	}
}

void printUsage(string s)
{
	cout << "Usage: " << s << " <algorithm> <config file>" << endl;
	cout << "\talgorithm:\tGA (Genetic Algorithm), SA (Simulated Annealing) or HC (Hill Climbing)" << endl;
	cout << "\tconfig file:\tLocation of config file for corresponding algoritm" << endl;
}

