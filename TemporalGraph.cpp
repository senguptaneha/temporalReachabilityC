#include <fstream>
#include <string>
#include <string.h>
#include <math.h>
#include "TemporalGraph.h"

TemporalGraph::TemporalGraph(){
	numNodes = 0;
	numEdges = 0;

	rrParamsInitialized = false;
	initializationTime = 0;

	numQueries = 0;
	numDeletes = 0;
	numInserts = 0;
	queryTimeMean = 0;
	queryTimeVar = 0;
	totalQueryTime = 0;
	insertTimeMean = 0;
	insertTimeVar = 0;
	totalInsertTime = 0;
	deleteTimeMean = 0;
	deleteTimeVar = 0;
	totalDeleteTime = 0;
    maxTimestamp = 0;


}

TemporalGraph::TemporalGraph(char *graphfilename, char *streamfilename){
	std::ifstream myfile(graphfilename);
	std::string line;
	initializationTime = 0;
	struct timespec start, finish;
	double elapsed;

	clock_gettime(CLOCK_MONOTONIC, &start);

	while (getline(myfile,line)){
		if (line[0] == '#') continue;
		char *cstr = &line[0u];
		char *t = strtok(cstr,"\t");
		int u = atoi(t);
		t = strtok(NULL,"\t");
		int v = atoi(t);
		if (u >= numNodes) numNodes = u + 1;
		if (v >= numNodes) numNodes = v + 1;

    }
	myfile.close();

	std::ifstream sfile(streamfilename);

	while (getline(sfile,line)){
		char *cstr = &line[0u];
		char *t = strtok(cstr," ");
		t = strtok(NULL," ");
		int u = atoi(t);
		t = strtok(NULL," ");
		int v = atoi(t);
		if (u >= numNodes) numNodes = u + 1;
		if (v >= numNodes) numNodes = v + 1;
	}
	sfile.close();

	numEdges = 0;

	for (int i = 0; i < numNodes; i++){
		nodes.push_back(new TemporalNode(i));
	}
	clock_gettime(CLOCK_MONOTONIC, &finish);
	//initializationTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);

	rrParamsInitialized = false;

	numQueries = 0;
	numDeletes = 0;
	numInserts = 0;
	queryTimeMean = 0;
	queryTimeVar = 0;
	totalQueryTime = 0;
	insertTimeMean = 0;
	insertTimeVar = 0;
	totalInsertTime = 0;
	deleteTimeMean = 0;
	deleteTimeVar = 0;
	totalDeleteTime = 0;
}

void TemporalGraph::addEdge(int src, int dst, int timestamp){
	if ((src >= numNodes) || (dst >= numNodes)) return;
	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	nodes[src]->addEdge(dst, timestamp, true);
	nodes[dst]->addEdge(src, timestamp, false);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	numEdges += 1;
	numInserts += 1;
	double elapsed = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
	insertTimeMean += elapsed;
	insertTimeVar += elapsed * elapsed;
	addEdgeToIndex(src, dst, timestamp);
}

void TemporalGraph::removeEdge(int src, int dst, int timestamp){
	if ((src >= numNodes) || (dst >= numNodes)) return;

	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int startTime = nodes[src]->removeEdge(dst, timestamp, true);
	nodes[dst]->removeEdge(src, timestamp, false);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	numDeletes += 1;
	double elapsed = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
	deleteTimeMean += elapsed;
	deleteTimeVar += elapsed * elapsed;
	removeEdgeFromIndex(src, dst, startTime, timestamp);
}

void TemporalGraph::readGraph(char *graphfilename){
	std::ifstream myfile(graphfilename);
	std::string line;
	struct timespec start, finish;

	while (getline(myfile,line)){
		if (line[0] == '#') continue;
		char *cstr = &line[0u];
		char *t = strtok(cstr,"\t");
		int u = atoi(t);
		t = strtok(NULL,"\t");
		int v = atoi(t);
		if ((u < numNodes) && (v < numNodes)){
			addEdge(u,v,0);
        }
    }
	myfile.close();


}

void TemporalGraph::readStream(char *streamfilename){
	std::ifstream myfile(streamfilename);
	std::string line;
	int timestamp;
	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int lineNum = 0;

	while (getline(myfile,line)){
		if (line[0] == '#') continue;
		char *cstr = &line[0u];
		char *f = strtok(cstr," ");
		if ((f[0] == 'I') || (f[0] == 'D')){
			char *t = strtok(NULL," ");
			int u = atoi(t);
			t = strtok(NULL," ");
			int v = atoi(t);
			t = strtok(NULL," ");
			timestamp = atoi(t);
			if ((u >= numNodes) || (v >= numNodes)) continue;
			if (f[0] == 'I')
				addEdge(u,v,timestamp);
			else if (f[0] == 'D')
				removeEdge(u,v,timestamp);
            if (timestamp > maxTimestamp) maxTimestamp = timestamp;
		}
		lineNum += 1;
		if (lineNum % 100000 == 0) cout << "Read " << lineNum << " lines."  << endl << flush;
    }
	myfile.close();
	insertTimeMean /= numInserts;
	insertTimeVar /= numInserts;
	insertTimeVar = sqrt(insertTimeVar - insertTimeMean * insertTimeMean);

	deleteTimeMean /= numDeletes;
	deleteTimeVar /= numDeletes;
	deleteTimeVar = sqrt(deleteTimeVar - deleteTimeMean * deleteTimeMean);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	initializationTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
}

void TemporalGraph::readAndRunQuery(char *queryFileName, int c, bool fractional){
	std::ifstream myfile(queryFileName);
	std::string line;

	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int dia = diameter();


	for (int i = 0; i < 10; i++){

		int d = diameter();
		if (d > dia) dia = d;
	}
	//int dia = 19;

	initializeRRParams(dia);

	clock_gettime(CLOCK_MONOTONIC, &finish);
	initializationTime = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
	int numOnes = 0;
	cout << "numWalks = " << numWalks << ", numStops = " << numStops << endl << flush;
	while (getline(myfile, line)){
		char *cstr = &line[0u];
		char *t = strtok(cstr," ");
		t = strtok(NULL, " ");
		int u = atoi(t);
		t = strtok(NULL," ");
		int v = atoi(t);
		t = strtok(NULL, " ");
		int startTime = atoi(t);
		t = strtok(NULL, " ");
		int endTime = atoi(t);
		clock_gettime(CLOCK_MONOTONIC, &start);
		int answer = isReachable(u,v,startTime, endTime, c, fractional);
		clock_gettime(CLOCK_MONOTONIC, &finish);
		double elapsed = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
		numQueries += 1;
		queryTimeMean += elapsed;
		queryTimeVar += elapsed * elapsed;
		numOnes += answer;
		std::cout << "Q: " << u << " " << v << " " << startTime << " " << endTime << " " << answer << " " << elapsed << std::endl << std::flush;
	}
	myfile.close();
	queryTimeMean /= numQueries;
	queryTimeVar /= numQueries;
	queryTimeVar = sqrt(queryTimeVar - queryTimeMean*queryTimeMean);
	cout << "Reached here = " << numOnes << endl;
}

int TemporalGraph::numberOfNodes(){
	return numNodes;
}

int TemporalGraph::numberOfEdges(){
	return numEdges;
}

int TemporalGraph::getMaxTimestamp(){
    return maxTimestamp;
}

void TemporalGraph::initializeRRParams(int diameter){

	walkLength = (int) floor(c_walkLength * diameter);
	cout << "diameter = " << diameter << ", c_walkLength = " << c_walkLength << ", walkLength = " << walkLength << endl;
	//numStops = (int)(floor(c_numWalks*sqrt(numNodes * log(numNodes))));
	numStops = (int)(pow(pow(numNodes * log(numNodes), 2), 1.0/3.0));
	cout << "numStops = " << numStops << ", c_numWalks = " << c_numWalks << ", numNodes = " << numNodes << endl;
	if (numStops == 0) numStops = 1;
	if (walkLength == 0){
		numWalks = numStops;
	}
	else numWalks = numStops/walkLength;
	if (numWalks == 0) numWalks = 1;
	srcStops = (Stop*)malloc(sizeof(Stop) * (numStops + 1));
	dstStops = (Stop*)malloc(sizeof(Stop) * (numStops + 1));

	rrParamsInitialized = true;

}

int TemporalGraph::diameter(){
	vector<int> color;
	for (int i = 0; i < numNodes; i++) color.push_back(0);
	int src = rand() % numNodes;
	color[src] = 1;
	vector<int> queue;
	int u = src;

	while (u >= 0){
		TemporalNode *n = nodes[u];
		int numE = n->getNumEdges(true);
		for (int i = 0; i < numE; i++){
			TemporalEdge *e = n->getEdgeAt(i, true);
			int v = e->getDestId();
			if (color[v] > 0) continue;
			color[v] = color[u] + 1;
			queue.push_back(v);
		}
		if (queue.size() == 0) break;
		u = queue[0];
		queue.erase(queue.begin());
	}

	int dia = 6;
	for (int i = 0; i < numNodes; i++)
		if (dia < (color[i]-1))
			dia = color[i]-1;
	return dia;
}

void TemporalGraph::changeForTopChain(){
	for (int i = 0; i < numNodes; i++){
		int nE = nodes[i]->getNumEdges(true);
		for (int j = 0; j < nE; j++){
			TemporalEdge *e = nodes[i]->getEdgeAt(j,true);
			e->setEndTime(e->getStartTime() + 1);
		}
		nE = nodes[i]->getNumEdges(false);
		for (int j = 0; j < nE; j++){
			TemporalEdge *e = nodes[i]->getEdgeAt(j,false);
			e->setEndTime(e->getStartTime() + 1);
		}
	}
}

void TemporalGraph::populateRandomFile(){
	numRandomNumbers = 1000000;
	randomNumbers = new int[numRandomNumbers];

	for (int i = 0; i < numRandomNumbers; i++){
		randomNumbers[i] = rand();
	}
	randomNumbersUsed = 0;

	/*randomFile.open("RandomNumbers", ios::binary | ios::out);
	for (int i = 0; i < 1000000; i++){
		int r = rand();
		randomFile.write((char*)&r, sizeof(int));
	}
	randomFile.close();*/
}

int TemporalGraph::getNextRand(){
	if (randomNumbersUsed >= numRandomNumbers) populateRandomFile();
	int  x = randomNumbers[randomNumbersUsed];
	randomNumbersUsed += 1;
	return x;
}
