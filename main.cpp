
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include "Node.cpp"

using namespace std;

int numVertices = 1000;
int lastTimestep = 0;


double insertTime = 0, deleteTime = 0, queryTime = 0;
int numInserts = 0, numDeletes = 0, numQueries = 0;

void readGraph(char *graphFile, int numVertices, Node **graph, Node **graphR){
	ifstream myfile(graphFile);
	string line;

	while (getline(myfile,line)){
		if (line[0] == '#') continue;
		char *cstr = &line[0u];
		char *t = strtok(cstr,"\t");
		int u = atoi(t);
		t = strtok(NULL,"\t");
		int v = atoi(t);
		if ((u < numVertices) && (v < numVertices)){
			graph[u]->addEdge(v,0,1);
			graphR[v]->addEdge(u,0,1);
		}
	}
	myfile.close();
}

void readStream(char *streamFile, Node **graph, Node **graphR){
	ifstream myfile(streamFile);
	string line;
	int timestep, numLevels = 1, nextIncr = 2;
	struct timespec start, finish;
	int numUpdates = 0;

	while (getline(myfile,line)){
		char *cstr = &line[0u];
		char *f = strtok(cstr," ");
//		cout << "Read line: " << line << endl << flush;

		if ((f[0]== 'I') || (f[0] == 'D')){	//insert or delete Edge
			char *t = strtok(NULL," ");
			int u = atoi(t);
			t = strtok(NULL," ");
			int v = atoi(t);
			t = strtok(NULL," ");
			timestep = atoi(t);
			clock_gettime(CLOCK_MONOTONIC, &start);
			if (timestep > lastTimestep){
				lastTimestep = timestep;

				int newNumLevels = (int)(ceil(log(timestep+1)/log(2))) + 1;
				numLevels = newNumLevels;
				/*for (int j = 0; j < numVertices; j++){
//					cout << "Processing for node " << j << ", numLevels = " << numLevels << endl << flush;
					graph[j]->trieHeightIncrease(timestep, numLevels);
					graphR[j]->trieHeightIncrease(timestep, numLevels);
//					cout << "Processed for node " << j << ", numLevels = " << numLevels << endl << flush;
				}*/
			}
			if (f[0] == 'I'){
				graph[u]->addEdge(v, timestep, numLevels);
				graphR[v]->addEdge(u, timestep, numLevels);
				numInserts += 1;
			}
			else{
				graph[u]->removeEdge(v, timestep, numLevels);
				graphR[v]->removeEdge(u, timestep, numLevels);
				numDeletes += 1;
			}
			clock_gettime(CLOCK_MONOTONIC, &finish);

			double updateTime = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
			(f[0] == 'I')? insertTime += updateTime : deleteTime += updateTime;
			numUpdates += 1;
		}
	}
	myfile.close();
}

Node **graph, **graphR;
int *uStops, *vStops;
int numWalks, walkLength, numStops;

int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

void getStops(int sourceNode, int *stops, int dir, int startTime, int endTime){
//	cout << "Inside getStops " << sourceNode << ", " << dir << endl;
	int labelBits = getCommonLabel(startTime, endTime);
	int label = setLabel(startTime, labelBits);
	for (int i = 0; i <= numStops; i++) stops[i] = -1;
	int n, ctr = 0;
	for (int i = 0; i < numWalks; i++){
		n = sourceNode;
		for (int j = 0; j < walkLength; j++){
			int d = -1;
			if (dir == 0) d = graph[n]->randomNeighbor(label, labelBits, startTime, endTime);
			if (dir == 1) d = graphR[n]->randomNeighbor(label, labelBits, startTime, endTime);
			if (d != -1){
				n = d;
				stops[ctr] = n;
				ctr += 1;
			}
		}
	}
	int i = 0;
	while (i<=numStops && stops[i]>-1){
		i++;
//		cout << stops[i] << ", ";
	}
//	cout << endl;
        qsort(stops, i, sizeof(int), cmpfunc);
}

int doesIntersect(int *a, int *b){
        int nn = 0;
        int i=0,j=0;
        int answer = 0;
        while ((i<=numStops) && (j<=numStops)){
                if ((a[i]==-1) || (b[j]==-1))
                        break;
                if (a[i] == b[j]){
                        answer = 1;
                        break;
                }
                if (a[i] < b[j]) i++;
                else j++;
        }
	return answer;
}


int runQuery(int u, int v, int startTime, int endTime, int depth){
	int answer = 0, tempStart = startTime, tempEnd = endTime;
	getStops(u, uStops, 0, startTime, endTime);
	getStops(v, vStops, 1, tempStart, endTime);
	answer |= doesIntersect(uStops, vStops);
	if ((answer == 0) && (depth < 4)){
		int midTime = (startTime + endTime)/2;
		int a = runQuery(u, v, startTime, midTime, depth + 1);
		if (a == 0)
			a = runQuery(u,v,startTime, midTime, depth + 1);
		answer |= a;
	}
	return answer;
}

int generateAndRunQuery(){
	struct timespec start, finish;
	int startTime = rand() % lastTimestep;
	int endTime = rand() % lastTimestep;
	if (startTime > endTime){
		int temp = startTime;
		startTime = endTime;
		endTime = temp;
	}
	int u = rand() % numVertices;
	int v = rand() % numVertices;
	clock_gettime(CLOCK_MONOTONIC, &start);
	runQuery(u,v,startTime, endTime,0);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	numQueries += 1;
	queryTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
}

void readAndRunQuery(char *queryFile){
        struct timespec start, finish;
        string line;
        ifstream myfile(queryFile);

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
                int answer = runQuery(u,v,startTime, endTime,0);
                clock_gettime(CLOCK_MONOTONIC, &finish);
                queryTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
		cout << "Q: " << u << " " << v << " " << startTime << " " << endTime << " " << answer << endl << flush;
                numQueries += 1;
		if (numQueries >= 100) break;
        }
        myfile.close();
}

void setParams(){
	walkLength = 14;
	numStops = (int)(floor(sqrt(numVertices*log(numVertices))));
	numWalks = numStops/walkLength;
	uStops = (int*)malloc(sizeof(int) * (numStops+1));
	vStops = (int*)malloc(sizeof(int) * (numStops+1));
}


int main(int argc, char *argv[]){
	if (argc < 5){
		//print usage
		cout << "Usage: baseline numVertices graphFile updateFile queryFile" << endl;
		return 0;
	}
	numVertices = atoi(argv[1]);
	char *graphFile = argv[2];
	char *streamFile = argv[3];
	char *queryFile = argv[4];

	graph = (Node**)malloc(numVertices*sizeof(Node*));
	graphR = (Node**)malloc(numVertices*sizeof(Node*));

	for (int i = 0; i < numVertices; i++){
		graph[i] = new Node(i);
		graphR[i] = new Node(i);
	}

	readGraph(graphFile, numVertices, graph, graphR);
	cout << "Finished Reading Graph!" << endl << flush;
	readStream(streamFile, graph, graphR);
	cout << "Finished Reading Stream!" << endl;
	setParams();
	struct timespec start, finish;
/*	for (int i = 0; i< 100; i++)
		generateAndRunQuery();
*/
	readAndRunQuery(queryFile);
/*	cout << "numDeletes = " << numDeletes << ", numInserts = " << numInserts << endl;
	cout << "Average Insert Time = " << (insertTime/numInserts) <<
		", Average Delete Time = " << (deleteTime/numDeletes) <<
		", Average Query Time = " << (queryTime/numQueries) << endl;
*///	cout << "qTime1 = " << (qTime1/numQueries) << ", qTime2 = " << (qTime2/numQueries) << endl;
	 cout << (insertTime/numInserts) << "\t"
             << (deleteTime/numDeletes) << "\t"
             << (queryTime/numQueries) << endl;

}
