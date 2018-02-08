//Graph Node
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "TrieTable.cpp"

using namespace std;

double qTime1 = 0, qTime2 = 0;

struct Edge{
	int destId;
	int startTime;

	Edge(){
		destId = -1;
		startTime = -1;
	}

	Edge(int isDestId, int isStartTime){
		destId = isDestId;
		startTime = isStartTime;
	}
};

struct Node{
	int nodeId;
	TrieTable *t;
	vector<Edge*> presentEdges;

	Node(){
		nodeId = 0;
		t = new TrieTable();
	}

	Node(int isNodeId){
		nodeId = isNodeId;
		t = new TrieTable();
	}

	int getIndexOfEdge(int destId){
		for (int i = 0; i< presentEdges.size(); i++)
			if (presentEdges[i]->destId == destId) return i;

		return -1;
	}

	void addEdge(int destId, int timestep, int numLevels){
		presentEdges.push_back(new Edge(destId, timestep));
		//t->addEdge(destId, timestep, sizeof(int)*8);
	}

	int removeEdge(int destId, int timestep, int numLevels){
		int edgeIndex = getIndexOfEdge(destId);
		if (edgeIndex == -1) return;	//no such edge found
		int startTime = presentEdges[edgeIndex]->startTime;
		int endTime = timestep;
		int numBits = getCommonLabel(startTime, endTime);
		t->addEdge(destId, startTime, numBits);
		presentEdges.erase(presentEdges.begin() + edgeIndex);
		return startTime;
	}

	void trieHeightIncrease(int timestep, int numLevels){
		int numPresentEdges = presentEdges.size();
		int *pE = (int*)malloc(sizeof(int)*numPresentEdges);
		for (int i = 0; i < numPresentEdges; i++) pE[i] = presentEdges[i]->destId;
		t->traverseRightmostPath(timestep, numLevels, pE, numPresentEdges);
		free(pE);
	}


	int randomNeighbor(int label, int numBits, int startTime, int endTime){
		struct timespec start, finish;
		int presentSample = -1;
		int numPresentEdges = 0;
//		clock_gettime(CLOCK_MONOTONIC, &start);
		for (int i = 0; i < presentEdges.size(); i++){
			if (presentEdges[i]->startTime <= endTime){
				numPresentEdges += 1;
				int z = rand() % numPresentEdges;
				if (z == 0) presentSample = presentEdges[i]->destId;
			}
		}
//		clock_gettime(CLOCK_MONOTONIC, &finish);
//		qTime1 += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
//		clock_gettime(CLOCK_MONOTONIC, &start);
		int rN = t->randomNeighbor(startTime, endTime, presentSample, numPresentEdges);
//		clock_gettime(CLOCK_MONOTONIC, &finish);
//		qTime2 += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
		return rN;
	}

};


