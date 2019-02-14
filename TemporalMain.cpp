#include <iostream>
#include <stdlib.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <math.h>

#include "TemporalGraph.h"


using namespace std;

int main(int argc, char *argv[]){
	if (argc < 4){
		cout << "Usage: <TemporalReachability> graphFile updateFile queryFile [[c fractional] [c_walklength c_numWalks c_budget]]" << endl;
		return 0;
	}
	char *graphFile = argv[1];
	char *streamFile = argv[2];
	char *queryFile = argv[3];
	int c = 1, c_budget = 1;
	double c_numWalks = 1.0, c_walkLength = 1;
	bool fractional = false;
	if (argc > 4) c = atoi(argv[4]);
	if (argc > 5) fractional = (atoi(argv[5]) > 0);
	if (argc >= 9){
	        c_walkLength = atof(argv[6]);
        	c_numWalks = atof(argv[7]);
        	c_budget = atoi(argv[8]);
	}
	srand(time(NULL));
	TemporalGraph *tg = new TemporalGraph(graphFile, streamFile);
	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	tg->readGraph(graphFile);
	cout << "Number of nodes = " << tg->numberOfNodes() << endl;
	tg->readStream(streamFile);

	clock_gettime(CLOCK_MONOTONIC, &finish);
	//cout << "Graph Construction Time(s) = " << (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9) << endl;

	/******Setting optional params*********/
	int dia = 19;
	cout << "Read Graph " << tg->numberOfNodes() << endl << flush;

	tg->c_budget = c_budget;
	tg->c_numWalks = c_numWalks;
	tg->c_walkLength = 1;
	/*for (c_walkLength = 0.01; c_walkLength < 2.0; c_walkLength *= 2){
		cout << "Params: " << c_numWalks << " " << c_walkLength << " " << c_budget << endl << flush;
		tg->c_walkLength = c_walkLength;
		tg->c_numWalks = c_numWalks;
		tg->c_budget = c_budget;
		tg->readAndRunQuery(queryFile, c, fractional);
	}*/

	tg->readAndRunQuery(queryFile, c, fractional);




//	tg->readAndRunQuery(queryFile, c, fractional);

	cout << tg->insertTimeMean << " " << tg->insertTimeVar << " " <<
		tg->deleteTimeMean << " " << tg->deleteTimeVar << " " <<
		tg->queryTimeMean << " " << tg->queryTimeVar << endl;
	cout << "Initialization Time(s) = " << tg->initializationTime << ", Memory (MBs) = " << malloc_count_peak()/(1024.0 * 1024.0) << endl;
}
