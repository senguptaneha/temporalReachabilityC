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

void addInterval(std::vector<Interval*> &bfsIntervals, int startTime, int endTime){
	Interval dummy;
	for (int i = 0; i < bfsIntervals.size(); i++){
        if (endTime < bfsIntervals[i]->getStartTime()){
            bfsIntervals.insert(bfsIntervals.begin()+i, new Interval(startTime, endTime));
            return;
        }
		if (startTime <= bfsIntervals[i]->getEndTime()){
			int minStartTime = dummy.min(startTime, bfsIntervals[i]->getStartTime());
			int j = i + 1;
			while ((j < bfsIntervals.size()) && (endTime >= bfsIntervals[j]->getStartTime()))
				j++;
			int maxEndTime = dummy.max(endTime, bfsIntervals[j-1]->getEndTime());
			for (int k = i; k < j; k++) free(bfsIntervals[k]);
			bfsIntervals.erase(bfsIntervals.begin()+i, bfsIntervals.begin()+j);
			bfsIntervals.insert(bfsIntervals.begin()+i, new Interval(minStartTime, maxEndTime));
			return;
		}
		if ((startTime <= bfsIntervals[i]->getStartTime()) && (endTime >= bfsIntervals[i]->getStartTime())){
			int minStartTime = startTime;
			int j = i + 1;
			while ((j < bfsIntervals.size()) && (endTime >= bfsIntervals[j]->getStartTime()))
				j++;
			int maxEndTime = dummy.max(endTime, bfsIntervals[j-1]->getEndTime());
			for (int k = i; k < j; k++) free(bfsIntervals[k]);
			bfsIntervals.erase(bfsIntervals.begin()+i, bfsIntervals.begin()+j);
			bfsIntervals.insert(bfsIntervals.begin()+i, new Interval(minStartTime, maxEndTime));
			return;
		}
	}
	bfsIntervals.push_back(new Interval(startTime, endTime));

}

bool intervalsOverlap(std::vector<Interval*> l1, std::vector<Interval*> l2, int c, bool fractional){
	int i = 0, j = 0, totalOverlap = 0, lastEndTime = -1;
	while (i < l1.size() && j < l2.size()){
		Interval *i1 = l1[i], *i2 = l2[j];
		if (i1->doesOverlap(i2)){
			int s = i1->getStartTime();
			if (s < i2->getStartTime()) s = i2->getStartTime();
			int e = i1->getEndTime();
			if (e > i2->getEndTime()) e = i2->getEndTime();
			if (e - s >= 0){
				if (fractional) totalOverlap += (e - s + 1);
				else{
					if ((lastEndTime == -1) 
					|| ((lastEndTime+1) >= s)) totalOverlap += (e - s + 1);
					else totalOverlap = (e - s + 1);
					lastEndTime = e;
				}
			}

		}
		if (i1->getEndTime() < i2->getEndTime()) i++;
		else j++;
	}
	if (totalOverlap >= c) return true;
	return false;
}

int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
	if (src == dst) return 1;
	std::vector<int> queueS, queueD;
	int i = 0, u;
	struct timespec start, finish;
	clock_gettime(CLOCK_MONOTONIC, &start);
	std::vector<Interval*> intervalS[numNodes];
	std::vector<Interval*> intervalD[numNodes];
	bool *isInQueueS = new bool[numNodes];
	bool *isInQueueD = new bool[numNodes];

	for (int i = 0; i < numNodes; i++){

		isInQueueS[i] = false;
		isInQueueD[i] = false;
	}

	queueS.push_back(src);
	queueD.push_back(dst);
	isInQueueS[src] = true;
	isInQueueD[dst] = true;
	addInterval(intervalS[src], startTime, endTime);
	addInterval(intervalD[dst], startTime, endTime);

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
				Interval *x = new Interval(e->getStartTime(),e->getEndTime());
				for (int j = 0; j < intervalS[u].size(); j++){
					Interval *z = x->intersection(intervalS[u][j]);
					if (z == NULL) continue;
					bool subFlag = false;
					for (int k = 0; k < intervalS[v].size(); k++){
						Interval *w = intervalS[v][k];
						bool isSub = w->isSubInterval(z);
						if (isSub) subFlag = true;
					}
					if (!subFlag){
						addInterval(intervalS[v], z->getStartTime(), z->getEndTime());
						//on adding the new interval to v, check if v now gives us a path
						if (intervalsOverlap(intervalS[v], intervalD[v], c, fractional)){
							answer = 1;
							break;
						}
						if (isInQueueS[v] == false){
							queueS.push_back(v);
							isInQueueS[v] = true;
						}
					}
					free(z);
				}
				if (answer == 1) break;
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
				Interval *x = new Interval(e->getStartTime(),e->getEndTime());
				for (int j = 0; j < intervalD[u].size(); j++){
					Interval *z = x->intersection(intervalD[u][j]);
					if (z == NULL) continue;
					bool subFlag = false;
					for (int k = 0; k < intervalD[v].size(); k++){
						Interval *w = intervalD[v][k];
						bool isSub = w->isSubInterval(z);
						if (isSub) subFlag = true;
					}
					if (!subFlag){
						addInterval(intervalD[v], z->getStartTime(), z->getEndTime());
						if (intervalsOverlap(intervalS[v], intervalD[v], c, fractional)){
							answer = 1;
							break;
						}
						if (isInQueueD[v] == false){
							queueD.push_back(v);
							isInQueueD[v] = true;
						}
					}
					free(z);
				}
				if (answer == 1) break;
			}
			if (answer == 1) break;
		}
		if (answer == 1) break;
		//if ((!flagL) || (!flagR)) break;

	}

	clock_gettime(CLOCK_MONOTONIC, &finish);
	cout << " B(s) = " << (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9) << endl;

	free(query);


	for (int i = 0; i < numNodes; i++){
		for (int j = 0; j < intervalS[i].size(); j++){
			delete intervalS[i][j];
		}
		intervalS[i].clear();
		for (int j = 0; j < intervalD[i].size(); j++){
			delete intervalD[i][j];
		}
		intervalD[i].clear();
	}

	delete[] isInQueueS;
	delete[] isInQueueD;

	return answer;
}
