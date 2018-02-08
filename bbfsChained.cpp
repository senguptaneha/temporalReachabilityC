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
	int *earliestArrival = new int[numNodes];
	int *latestStart = new int[numNodes];
	int *smallestHop = new int[numNodes];
	bool *isInQueue = new bool[numNodes];
	int *pred = new int[numNodes];
	int *succ = new int[numNodes];

	std::vector<int> queue;
	for (int i = 0; i < numNodes; i++){
		earliestArrival[i] = -1;
		smallestHop[i] = -1;
		isInQueue[i] = false;
		pred[i] = -1;
		succ[i] = -1;
	}
	int u = src;
	earliestArrival[u] = startTime;
	smallestHop[u] = 0;
	queue.push_back(u);
	isInQueue[u] = true;
	Interval *query = new Interval(startTime, endTime);
	int answer = 0;
	int numMarkedBySrc = 0, numMarkedByDst = 0;
	while (queue.size() > 0){
		u = queue[0];
		queue.erase(queue.begin());
		isInQueue[u] = false;

		//for each edge from u
		TemporalNode *n = nodes[u];
		for (int i = 0; i < n->getNumEdges(true); i++){
			TemporalEdge *e = n->getEdgeAt(i,true);
			int v = e->getDestId();
			if (smallestHop[v] == -1) numMarkedBySrc++;
			if (smallestHop[v] == -1 || (smallestHop[v] > (smallestHop[u] + 1)) ) smallestHop[v] = smallestHop[u] + 1;

			if ((e->getEndTime() != -2)			//no point in taking perpetual edges, temporarily changed -1 to -2
			 && (e->getStartTime() >= earliestArrival[u])
			 && ((earliestArrival[v] > e->getEndTime()) || (earliestArrival[v] == -1))
			 && (e->getEndTime() <= endTime)){
				earliestArrival [v] = e->getEndTime();
				pred[v] = u;
				if (isInQueue[v] == false && smallestHop[v] < walkLength/2){
					queue.push_back(v);
					isInQueue[v] = true;
				}
			}
		}
	}

	u = dst;
	if (earliestArrival[u] == -1){
		for (int i = 0; i < numNodes; i++){
			smallestHop[i] = -1;
			latestStart[i] = -1;
			isInQueue[i] = false;
		}

		latestStart[u] = endTime;
		smallestHop[u] = 0;
		queue.push_back(u);
		isInQueue[u] = true;

		while (queue.size() > 0){
			u = queue[0];
			queue.erase(queue.begin());
			isInQueue[u] = false;

			//for each edge from u
			TemporalNode *n = nodes[u];
			for (int i = 0; i < n->getNumEdges(false); i++){
				TemporalEdge *e = n->getEdgeAt(i,false);
				int v = e->getDestId();
				if (smallestHop[v] == -1) numMarkedByDst++;
				if (smallestHop[v] == -1 || (smallestHop[v] > (smallestHop[u] + 1)) ) smallestHop[v] = smallestHop[u] + 1;

				if ((e->getEndTime() != -2)			//no point in taking perpetual edges, temporarily changed -1 to -2
				 && (e->getEndTime() <= latestStart[u])
				 && ((latestStart[v] < e->getStartTime()) || (latestStart[v] == -1))
				 && (e->getStartTime() >= startTime)){
					latestStart [v] = e->getStartTime();
					succ[v] = u;
					if ((earliestArrival[v] >= 0) && (earliestArrival[v] <= latestStart[v])){
						answer = 1;
						break;
					}

					if (isInQueue[v] == false && smallestHop[v] < walkLength/2){
						queue.push_back(v);
						isInQueue[v] = true;
					}
				}
			}
			if (answer == 1) break;
		}

	}
	else answer = 1;;
	free(query);

	delete[] earliestArrival;
	delete[] latestStart;
	delete[] isInQueue;
	delete[] pred;
	delete[] succ;
	return answer;
}
