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


int max(int x1, int x2){
	return (x1 > x2)? x1 : x2;
}

int min(int x1, int x2){
	return (x1 < x2)? x1 : x2;
}

struct Interval{
	int startTime;
	int endTime;

	Interval(){
		startTime = -1;
		endTime = -1;
	}

	Interval(int isStartTime){
		startTime = isStartTime;
		endTime = -1;
	}

	Interval(int isStartTime, int isEndTime){
		startTime = isStartTime;
		endTime = isEndTime;
	}

	Interval *Intersection(Interval *other){
		if (other == NULL) return NULL;
		if ((endTime == -1) && (other->endTime == -1))
			return new Interval(max(startTime, other->startTime), -1);
		if ((endTime == -1) && (startTime <= other->endTime))
			return new Interval(max(startTime, other->startTime), other->endTime);
		if ((other->endTime == -1) && (other->startTime <= endTime))
			return new Interval(max(startTime, other->startTime), endTime);
		if ((endTime < other->startTime) || (other->endTime < startTime)) return NULL;
		return new Interval(max(startTime, other->startTime), min(endTime, other->endTime));
	}

	bool isSubInterval(Interval *other){
		if (other == NULL) return false;
		bool answer = false;
		if ((endTime == -1) && (other->startTime >= startTime)) answer = true;
		else if ((endTime != -1) && (other->endTime != -1) && 
			(other->endTime <= endTime) && (other->startTime >= startTime))
			answer = true;
/*		Interval *temp = Intersection(other);
		if (temp == NULL) return false;
		bool answer = false;
		if ((endTime == -1) && (temp->startTime >= startTime)) answer = true;
		else if ((endTime != -1) && (temp->endTime != -1) &&
			(temp->endTime <= endTime) && (temp->startTime >= startTime))
			answer = true;
		free(temp);
*/
		return answer;
	}

	void setEndTime(int isEndTime){
		endTime = isEndTime;
	}

};

struct Edge{
	int destId;
	Interval *interval;

	Edge(int isDestId, int isStartTime){
		destId = isDestId;
		interval = new Interval(isStartTime);
	}

	bool isValid(int start, int end){
		Interval *temp1 = new Interval(start, end);
		Interval *temp2 = interval->Intersection(temp1);
		bool answer = false;
		if (temp2) answer = true;
		free(temp1);
		free(temp2);
		return answer;
	}

	int getEndTime(){
		return interval->endTime;
	}

	int getStartTime(){
		return interval->startTime;
	}

	void setEndTime(int isEndTime){
		interval->endTime = isEndTime;
	}
};

struct Node{
	int nodeId;
	vector<Edge*> fwdEdges;
	vector<Edge*> bwdEdges;
	bool isInQueue;

	vector<Interval*> bfsIntervals;

	Node(int isNodeId){
		nodeId = isNodeId;
	}

	void clearIntervals(){
		for (int i = 0; i < bfsIntervals.size(); i++){
			free(bfsIntervals[i]);
		}
		bfsIntervals.clear();
	}

	void addInterval(int startTime, int endTime){
		for (int i = 0; i < bfsIntervals.size(); i++){
			if (startTime <= bfsIntervals[i]->endTime){
				int minStartTime = min(startTime, bfsIntervals[i]->startTime);
				int j = i + 1;
				while ((j < bfsIntervals.size()) && (endTime >= bfsIntervals[j]->startTime))
					j++;
				int maxEndTime = max(endTime, bfsIntervals[j-1]->endTime);
				for (int k = i; k < j; k++) free(bfsIntervals[k]);
				bfsIntervals.erase(bfsIntervals.begin()+i, bfsIntervals.begin()+j);
				bfsIntervals.insert(bfsIntervals.begin()+i, new Interval(minStartTime, maxEndTime));
				return;
			}
			if ((startTime <= bfsIntervals[i]->startTime) && (endTime >= bfsIntervals[i]->startTime)){
				int minStartTime = startTime;
				int j = i + 1;
				while ((j < bfsIntervals.size()) && (endTime >= bfsIntervals[j]->startTime))
					j++;
				int maxEndTime = max(endTime, bfsIntervals[j-1]->endTime);
				for (int k = i; k < j; k++) free(bfsIntervals[k]);
				bfsIntervals.erase(bfsIntervals.begin()+i, bfsIntervals.begin()+j);
				bfsIntervals.insert(bfsIntervals.begin()+i, new Interval(minStartTime, maxEndTime));
				return;
			}
		}
		bfsIntervals.push_back(new Interval(startTime, endTime));
	}
	void printIntervals(){
		for (int i = 0; i< bfsIntervals.size(); i++){
			cout << "( " << bfsIntervals[i]->startTime << ", " << bfsIntervals[i]->endTime << ") ";
		}
		cout << endl;
	}

	void addEdgeFwd(int destId, int startTime){
		fwdEdges.push_back(new Edge(destId, startTime));
	}

	void addEdgeBwd(int destId, int startTime){
		bwdEdges.push_back(new Edge(destId, startTime));
	}

	void removeEdgeFwd(int destId, int endTime){
		for (int i = 0; i < fwdEdges.size(); i++)
			if ((fwdEdges[i]->destId == destId) && (fwdEdges[i]->getEndTime() == -1)) fwdEdges[i]->setEndTime(endTime);
	}

	void removeEdgeBwd(int destId, int endTime){
		for (int i = 0; i < bwdEdges.size(); i++)
			if ((bwdEdges[i]->destId == destId) && (bwdEdges[i]->getEndTime() == -1)) bwdEdges[i]->setEndTime(endTime);
	}

};

int lastTimestep = 0, numVertices = 1000;

void resetNodes(Node **graph){
	for (int i = 0; i < numVertices; i++){
		graph[i]->clearIntervals();
		graph[i]->isInQueue = false;
	}
}

int bfsTemporal(int srcNode, int destNode, int startTime, int endTime, Node **graph){
	resetNodes(graph);
	vector<int> queue;
	graph[srcNode]->addInterval(startTime, endTime);
	graph[srcNode]->isInQueue = true;
	queue.push_back(srcNode);
	cout << "starting bfsTempral: CP1 ("  << srcNode << ", " << destNode << ")" << endl << flush;
	while (queue.size() > 0){
		int u = queue[0];
		queue.erase(queue.begin());
		graph[u]->isInQueue = false;
//		cout << "bfsTempral: CP2 " << endl << flush;

		for (int i = 0; i < graph[u]->fwdEdges.size(); i++){
			Edge *e = graph[u]->fwdEdges[i];
			int v = e->destId;
//			cout << "bfsTempral: CP3 " << endl << flush;

			/*Insersect e's interval with each of u's intervals*/
			Interval *x = e->interval;
			for (int j = 0; j < graph[u]->bfsIntervals.size(); j++){
				Interval *y = graph[u]->bfsIntervals[j];
				Interval *z = y->Intersection(x);
				if (z == NULL) continue;
//				cout << "bfsTempral: CP4 " << endl << flush;
/*				cout << "y = ( " << y->startTime << ", " << y->endTime << "); ";
				cout << "x = ( " << x->startTime << ", " << x->endTime << "); ";
				cout << "z = ( " << z->startTime << ", " << z->endTime << "); " << endl;
*/				bool subFlag = false;
				for (int k = 0; k < graph[v]->bfsIntervals.size(); k++){
					Interval *w = graph[v]->bfsIntervals[k];
//					cout << "bfsTempral: CP4.1 " << endl << flush;
					bool isSub = w->isSubInterval(z);
//					cout << "bfsTempral: CP4.2 " << endl << flush;
					if (isSub) subFlag = true;
//					cout << "bfsTempral: CP4.3 " << endl << flush;
				}
//				cout << "bfsTempral: CP5 " << endl << flush;
				if (!subFlag){
					//cout << v << ": added Interval ( " << z->startTime << ", " << z->endTime << ")" << endl;
					graph[v]->addInterval(z->startTime, z->endTime);
					if (graph[v]->isInQueue == false){
						queue.push_back(v);
						graph[v]->isInQueue = true;
					}
					//graph[v]->printIntervals();
				}
				free(z);
//				cout << "bfsTempral: CP6 " << endl << flush;
			}
//			cout << "bfsTempral: CP7 " << endl << flush;


		}
//		cout << "bfsTempral: CP8 " << endl << flush;

	}
	if (graph[destNode]->bfsIntervals.size() > 0) return 1;
	return 0;
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
		int answer = bfsTemporal(u,v,startTime, endTime, graph);
		cout << "Q: " << u << " " << v << " " << startTime << " " << endTime << " " << answer << endl << flush;
		clock_gettime(CLOCK_MONOTONIC, &finish);
		queryTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
		numQueries += 1;
		cout << "queryTime: " << queryTime << endl;
		if (numQueries >= 100) break;
	}
	myfile.close();
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
//		cout << "readStream: CP0 line = " << line << endl << flush;
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
//			cout << "readStream: CP1 " << endl << flush;
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
//			cout << "readStream: CP2 " << endl << flush;
                        clock_gettime(CLOCK_MONOTONIC, &finish);
                        double updateTime = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
			(f[0] == 'I')? insertTime += updateTime : deleteTime += updateTime;
                        numUpdates += 1;
                }
        }
        myfile.close();
}

int main(int argc, char *argv[]){
        if (argc < 5){
                //print usage
		cout << "Usage: baseline1 numVertices graphFile updateFile queryFile" << endl;
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
	cout << "Read Graph " << endl << flush;
        readStream(streamFile, graph);
        struct timespec start, finish;
	cout << "Checkpoint 1 "<< endl << flush;
	readAndRunQuery(queryFile, graph);

/*	cout << "Average Insert Time = " << (insertTime/numInserts) <<
		", Average Delete Time = " << (deleteTime/numDeletes) <<
		", Average Query Time = " << (queryTime/numQueries) << endl;
*/
	cout << (insertTime/numInserts) << "\t"
	     << (deleteTime/numDeletes) << "\t"
	     << (queryTime/numQueries) << endl;
}

