#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>

#include "TemporalGraph.h"

void TemporalGraph::addEdgeToIndex(int src, int dst, int startTime){
}

void TemporalGraph::removeEdgeFromIndex(int src, int dst, int startTime, int endTime){
}

int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
	/*reset all Nodes*/
	//cout << "BFS Log of Query for " << src << " to " << dst << endl;
	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int *earliestArrival = new int[numNodes];
	bool *isInQueue = new bool[numNodes];
	int *smallestHop = new int[numNodes];
	std::vector<int> queue;
	for (int i = 0; i < numNodes; i++){
		earliestArrival[i] = -1;
		isInQueue[i] = false;
		smallestHop[i] = -1;
	}
	int u = src;
	earliestArrival[u] = startTime;
	queue.push_back(u);
	isInQueue[u] = true;
	Interval *query = new Interval(startTime, endTime);
	smallestHop[u] = 0;
	clock_gettime(CLOCK_MONOTONIC, &finish);
	cout << "I(s) = " << (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9) << " " ;
	clock_gettime(CLOCK_MONOTONIC, &start);
	while (queue.size() > 0){
		u = queue[0];
		queue.erase(queue.begin());
		isInQueue[u] = false;

		//for each edge from u
		TemporalNode *n = nodes[u];
		for (int i = 0; i < n->getNumEdges(true); i++){
			TemporalEdge *e = n->getEdgeAt(i,true);
			int v = e->getDestId();

			if ((e->getEndTime() != -2)			//no point in taking perpetual edges, temporarily changed -1 to -2
			 && (e->getStartTime() >= earliestArrival[u])
			 && ((earliestArrival[v] > e->getEndTime()) || (earliestArrival[v] == -1))
			 && (e->getEndTime() <= endTime)){
				//cout << "Traversed edge (" << u << ", " << v << " : [" << e->getStartTime() << ", " << e->getEndTime() << "] ";
				earliestArrival [v] = e->getEndTime();
				//cout << "EarliestArrival[v] = " << earliestArrival[v] << endl;
				if ((smallestHop[v] == -1) || (smallestHop[v] > (smallestHop[u] + 1))) smallestHop[v] = smallestHop[u] + 1;
				if (isInQueue[v] == false){
					queue.push_back(v);
					isInQueue[v] = true;
				}
			}
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &finish);
	cout << "B(s) = " << (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9) << endl;
	free(query);
	int answer = 0;
	if (earliestArrival[dst] != -1) answer =  1;
	delete[] earliestArrival;
	delete[] isInQueue;
	delete[] smallestHop;
	return answer;
}
