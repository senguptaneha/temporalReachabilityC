#include <iostream>
#include <vector>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <string>
#include <string.h>

using namespace std;

double insertTime = 0, deleteTime = 0, queryTime = 0;
int numInserts = 0, numDeletes = 0, numQueries = 0;

struct stop{
	int nodeId;
	int startTime;
	int endTime;

	stop(int isNodeId, int isStartTime, int isEndTime){
		nodeId = isNodeId;
		startTime = isStartTime;
		endTime = isEndTime;
	}

	stop(){
		nodeId = -1;
		startTime = -1;
		endTime = -1;
	}

	void clear(){
		nodeId = -1;
		startTime = -1;
		endTime = -1;
	}

	void setValues(int isNodeId, int isStartTime, int isEndTime){
		nodeId = isNodeId;
		startTime = isStartTime;
		endTime = isEndTime;
	}

	bool intersect(stop* another){
		/*any one edge is a present edge*/
		if ((endTime == -1) && (another->endTime == -1)) return true;
		if ((endTime == -1) && (startTime <= another->endTime)) return true;
		if ((another->endTime == -1) && (another->startTime <= endTime)) return true;

		if (endTime < another->startTime) return false;
		if (another->endTime < startTime) return false;
		return true;
	}

};

struct Edge{
	int destId;
	int startTime;
	int endTime;

	Edge(int isDestId, int isStartTime){
		destId = isDestId;
		startTime = isStartTime;
		endTime = -1;
	}

	bool isValid(int start, int end){
		if (endTime == -1)
			if (startTime <= end) return true;
			else return false;
		if ((start > endTime) || (end < startTime)) return false;
		return true;
	}
};

struct Node{
	int nodeId;
	vector<Edge*> fwdEdges;
	vector<Edge*> bwdEdges;

	Node(int isNodeId){
		nodeId = isNodeId;
	}

	void addEdgeFwd(int destId, int startTime){
		fwdEdges.push_back(new Edge(destId, startTime));
	}

	void addEdgeBwd(int destId, int startTime){
		bwdEdges.push_back(new Edge(destId, startTime));
	}

	void removeEdgeFwd(int destId, int endTime){
		for (int i = 0; i < fwdEdges.size(); i++)
			if ((fwdEdges[i]->destId == destId) && (fwdEdges[i]->endTime == -1)) fwdEdges[i]->endTime = endTime;
	}

	void removeEdgeBwd(int destId, int endTime){
		for (int i = 0; i < bwdEdges.size(); i++)
			if ((bwdEdges[i]->destId == destId) && (bwdEdges[i]->endTime == -1)) bwdEdges[i]->endTime = endTime;
	}


	void sampleEdgeFwd(stop *d, int startTime, int endTime){
		int numCandidates = 0;
		for (int i = 0; i < fwdEdges.size(); i++){
			if (fwdEdges[i]->isValid(startTime, endTime)){
				numCandidates += 1;
				if ((rand() % numCandidates) == 0)
					d->setValues(fwdEdges[i]->destId, fwdEdges[i]->startTime, fwdEdges[i]->endTime);
			}
		}
	}

	void sampleEdgeBwd(stop *d, int startTime, int endTime){
		int destId = -1;
		int numCandidates = 0;
		for (int i = 0; i < bwdEdges.size(); i++){
			if (bwdEdges[i]->isValid(startTime, endTime)){
				numCandidates += 1;
				if ((rand() % numCandidates) == 0)
					d->setValues(bwdEdges[i]->destId, bwdEdges[i]->startTime, bwdEdges[i]->endTime);
			}
		}
	}

};
int lastTimestep = 0, numVertices = 1000;

stop **uStops, **vStops;
int numWalks, walkLength, numStops;

int cmpfunc (const void * a, const void * b)
{
   return ( ((stop*)a)->nodeId - ((stop*)b)->nodeId );
}

void getStops(int sourceNode, stop **stops, int dir, int startTime, int endTime, Node** graph){
        int n, ctr = 0, maxStart;
        for (int i = 0; i < numWalks; i++){
                n = sourceNode;
		maxStart = startTime;
                for (int j = 0; j < walkLength; j++){
                        stop d;
                        if (dir == 0) graph[n]->sampleEdgeFwd(&d, maxStart, endTime);
                        if (dir == 1) graph[n]->sampleEdgeBwd(&d, maxStart, endTime);
                        if (d.nodeId != -1){
                                n = d.nodeId;
				if (d.startTime > maxStart) maxStart = d.startTime;
                                stops[ctr]->setValues(d.nodeId, d.startTime, d.endTime);
                                ctr += 1;
                        }
                }
        }
        int i = 0;
        while (i<=numStops && stops[i]->nodeId > -1) i++;
//      qsort(stops, i, sizeof(int), cmpfunc);
}


int doesIntersect(stop **a, stop **b){
	for (int i = 0; i <= numStops; i++){
		if (a[i]->nodeId == -1) return 0;
		for (int j = 0; j <= numStops; j++){
			if (b[j]->nodeId == -1) break;
			if ((a[i]->nodeId == b[j]->nodeId) && (a[i]->intersect(b[j]))) return 1;
		}
	}
	return 0;
}

int runQuery(int u, int v, int startTime, int endTime, Node **graph){
	for (int i = 0; i <= numStops; i++){
		uStops[i]->clear();
		vStops[i]->clear();
	}
        getStops(u, uStops, 0, startTime, endTime, graph);
        getStops(v, vStops, 1, startTime, endTime, graph);
        return doesIntersect(uStops, vStops);
}

void generateAndRunQuery(Node **graph){
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
        int answer = runQuery(u,v,startTime, endTime, graph);
	clock_gettime(CLOCK_MONOTONIC, &finish);
	cout << "Q: " << u << " " << v << " " << startTime << " " << endTime << " " << answer << endl;
	numQueries += 1;
	queryTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
}

void readAndRunQuery(char *queryFile, Node **graph){
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
		int answer = runQuery(u,v,startTime, endTime, graph);
		clock_gettime(CLOCK_MONOTONIC, &finish);
		queryTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
		numQueries += 1;
	}
	myfile.close();
}


void setParams(){
        walkLength = 10;
        numWalks = 10;
        numStops = walkLength * numWalks + 1;
        uStops = (stop**)malloc(sizeof(stop*) * (numStops+1));
        vStops = (stop**)malloc(sizeof(stop*) * (numStops+1));
	for (int i = 0; i <= numStops; i++){
		uStops[i] = new stop();
		vStops[i] = new stop();
	}
}

void readGraph(char *graphFile, int numVertices, Node **graph){
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
                        graph[u]->addEdgeFwd(v,0);
                        graph[v]->addEdgeBwd(u,0);
                }
        }
        myfile.close();
}

void readStream(char *streamFile, Node **graph){
        ifstream myfile(streamFile);
        string line;
        int timestep;
        struct timespec start, finish;
        int numUpdates = 0;

        while (getline(myfile,line)){
                char *cstr = &line[0u];
                char *f = strtok(cstr," ");

                if ((f[0]== 'I') || (f[0] == 'D')){     //insert or delete Edge
                        char *t = strtok(NULL," ");
                        int u = atoi(t);
                        t = strtok(NULL," ");
                        int v = atoi(t);
                        t = strtok(NULL," ");
                        timestep = atoi(t);
                        clock_gettime(CLOCK_MONOTONIC, &start);
                        if (timestep > lastTimestep){
                                lastTimestep = timestep;
                        }
                        if (f[0] == 'I'){
                                graph[u]->addEdgeFwd(v, timestep);
                                graph[v]->addEdgeBwd(u, timestep);
				numInserts += 1;
                        }
                        else{
                                graph[u]->removeEdgeFwd(v, timestep);
                                graph[v]->removeEdgeBwd(u, timestep);
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

int main(int argc, char *argv[]){
        if (argc < 4){
                //print usage
		cout << "Usage: baseline numVertices graphFile updateFile queryFile" << endl;
                return 0;
        }
        numVertices = atoi(argv[1]);
        char *graphFile = argv[2];
        char *streamFile = argv[3];
	char *queryFile = argv[4];
        Node ** graph = (Node**)malloc(numVertices*sizeof(Node*));


        for (int i = 0; i < numVertices; i++){
                graph[i] = new Node(i);

        }

        readGraph(graphFile, numVertices, graph);
//        cout << "Finished Reading Graph!" << endl << flush;
        readStream(streamFile, graph);
//        cout << "Finished Reading Stream!" << endl << flush;
        setParams();
        struct timespec start, finish;
        for (int i = 0; i< 10000; i++)
                generateAndRunQuery(graph);


//	readAndRunQuery(queryFile, graph);

/*	cout << "Average Insert Time = " << (insertTime/numInserts) <<
		", Average Delete Time = " << (deleteTime/numDeletes) <<
		", Average Query Time = " << (queryTime/numQueries) << endl;
*/
	cout << (insertTime/numInserts) << "\t"
	     << (deleteTime/numDeletes) << "\t"
	     << (queryTime/numQueries) << endl;
}

