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
	if (src == dst) return 1;
	std::vector<int> queueS, queueD;
	int i = 0, u;
	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	int *earliestArrival = new int[numNodes];
	int *latestStart = new int[numNodes];
	bool *isInQueueS = new bool[numNodes];
	bool *isInQueueD = new bool[numNodes];

	for (int i = 0; i < numNodes; i++){
		earliestArrival[i] = -1;
		latestStart[i] = -1;
		isInQueueS[i] = false;
		isInQueueD[i] = false;
	}

	queueS.push_back(src);
	queueD.push_back(dst);
	isInQueueS[src] = true;
	isInQueueD[dst] = true;
	earliestArrival[src] = startTime;
	latestStart[dst] = endTime;
	Interval *query = new Interval(startTime, endTime);
	int answer = 0;
	clock_gettime(CLOCK_MONOTONIC, &finish);
	cout << "I(s) = " << (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
	clock_gettime(CLOCK_MONOTONIC, &start);
	while (true){
		i++;
		int L1 = queueS.size();
		int L2 = queueD.size();
		if ((L1 == 0) && (L2 == 0)) break;

		bool flagL = false, flagR = false;	//whether any new nodes were added in this iteration
		for (int k = 0; k < L1; k++){
			u = queueS[0];
			queueS.erase(queueS.begin());
			isInQueueS[u] = false;
			TemporalNode *n = nodes[u];
			for (int i = 0; i < n->getNumEdges(true); i++){
				TemporalEdge *e = n->getEdgeAt(i,true);
				int v = e->getDestId();

				if ((e->getEndTime() != -2)			//no point in taking perpetual edges, temporarily changed -1 to -2
				&& (e->getStartTime() >= earliestArrival[u])
				&& ((earliestArrival[v] > e->getEndTime()) || (earliestArrival[v] == -1))
				&& (e->getEndTime() <= endTime)){
					earliestArrival [v] = e->getEndTime();
					if ((latestStart[v] >= 0) && (earliestArrival[v] <= latestStart[v])){
						answer = 1;
						break;
					}

					if (isInQueueS[v] == false){
						queueS.push_back(v);
						isInQueueS[v] = true;
						flagL = true;
					}
				}
			}
			if (answer == 1) break;
		}
		if (answer == 1) break;

		for (int k = 0; k < L2; k++){
			u = queueD[0];
			queueD.erase(queueD.begin());
			isInQueueD[u] = false;
			TemporalNode *n = nodes[u];
			for (int i = 0; i < n->getNumEdges(false); i++){
				TemporalEdge *e = n->getEdgeAt(i,false);
				int v = e->getDestId();

				if ((e->getEndTime() != -2)			//no point in taking perpetual edges, temporarily changed -1 to -2
				&& (e->getEndTime() <= latestStart[u])
				&& ((latestStart[v] < e->getStartTime()) || (latestStart[v] == -1))
				&& (e->getStartTime() >= startTime)){
					latestStart [v] = e->getStartTime();
					if ((earliestArrival[v] >= 0) && (earliestArrival[v] <= latestStart[v])){
						answer = 1;
						break;
					}

					if (isInQueueD[v] == false){
						queueD.push_back(v);
						isInQueueD[v] = true;
						flagR = true;
					}

				}
			}
			if (answer == 1) break;
		}
		if (answer == 1) break;
		//if ((!flagL) || (!flagR)) break;

	}

	clock_gettime(CLOCK_MONOTONIC, &finish);
	cout << " B(s) = " << (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9) << endl;

	free(query);


	delete[] earliestArrival;
	delete[] latestStart;
	delete[] isInQueueS;
	delete[] isInQueueD;

	return answer;
}
/*
int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
	//reset all nodes
	int *earliestArrival = new int[numNodes];
	int *latestStart = new int[numNodes];
	int *smallestHop = new int[numNodes];
	bool *isInQueue = new bool[numNodes];
	int *pred = new int[numNodes];
	int *succ = new int[numNodes];
	struct timespec start, finish;
	double initTime = 0, bfsTime1 = 0, bfsTime2 = 0;

	clock_gettime(CLOCK_MONOTONIC, &start);
	std::vector<int> queue;
	for (int i = 0; i < numNodes; i++){
		earliestArrival[i] = -1;
		smallestHop[i] = -1;
		isInQueue[i] = false;
		pred[i] = -1;
		succ[i] = -1;
	}
	clock_gettime(CLOCK_MONOTONIC, &finish);
	initTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);

	int u = src;
	earliestArrival[u] = startTime;
	smallestHop[u] = 0;
	queue.push_back(u);
	isInQueue[u] = true;
	Interval *query = new Interval(startTime, endTime);
	int answer = 0;
	int numMarkedBySrc = 0, numMarkedByDst = 0;
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
	clock_gettime(CLOCK_MONOTONIC, &finish);
	bfsTime1 += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);


	u = dst;
	if (earliestArrival[u] == -1){
		clock_gettime(CLOCK_MONOTONIC, &start);
		for (int i = 0; i < numNodes; i++){
			smallestHop[i] = -1;
			latestStart[i] = -1;
			isInQueue[i] = false;
		}
		clock_gettime(CLOCK_MONOTONIC, &finish);
		initTime += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
		latestStart[u] = endTime;
		smallestHop[u] = 0;
		queue.push_back(u);
		isInQueue[u] = true;
		clock_gettime(CLOCK_MONOTONIC, &start);
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
		clock_gettime(CLOCK_MONOTONIC, &finish);
		bfsTime2 += (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);


	}
	else answer = 1;;
	free(query);
	cout << "Initialization (s) = " << initTime << ", BFS (s) = " << bfsTime1 << ", " << bfsTime2 << endl;

	delete[] earliestArrival;
	delete[] latestStart;
	delete[] isInQueue;
	delete[] pred;
	delete[] succ;
	return answer;
}*/
