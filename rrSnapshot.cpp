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

//last 3 params not actually used, only for compatibility across methods.
int TemporalGraph::buildStops(int src, bool direction, int startTime, int endTime, int dst, int c, bool fractional){
	struct timespec start, finish, start1, finish1, start2, finish2;
	double sampleEdgeTimeN = 0, sampleEdgeTime = 0, sampleEdgeTimeO = 0;
	clock_gettime(CLOCK_MONOTONIC, &start);
	Stop *stops = srcStops;
	if (!direction) stops = dstStops;
	for (int i = 0; i <= numStops; i++) stops[i].reset();
	Interval *newInterval = new Interval();
	int stopIndex = 0;
	stops[stopIndex].setNodeId(src);
	stops[stopIndex].setStartTime(startTime);
	stops[stopIndex].setEndTime(endTime);
	bool flagSeenDest = false;
	for (int i = 0; i < numWalks; i++){
		int budget = c_budget * walkLength;
		Interval *currInterval = new Interval(startTime, endTime);
		int currNode = src;
		bool noEdges = false;
		for (int j = 0; j < walkLength; j++){
			clock_gettime(CLOCK_MONOTONIC, &start1);
			TemporalNode *n = nodes[currNode];
			TemporalEdge *e;

			/*cout << "Sampling from " << currNode << ", with budget = " << budget << ", d = " << direction << endl;
			cout << "currInterval is ( " << currInterval->getStartTime() << ", " << currInterval->getEndTime() << ") " << endl;*/
			bool flag = false;
			while ((flag == false) && (budget > 0)){
				budget -= 1;
				clock_gettime(CLOCK_MONOTONIC, &start2);
				e = n->sampleEdge(direction);
				clock_gettime(CLOCK_MONOTONIC, &finish2);

                sampleEdgeTimeO += (finish2.tv_sec - start2.tv_sec) + (finish2.tv_nsec - start2.tv_nsec)/pow(10,9);
				if (e == NULL){
					n = nodes[src];
					currNode = src; //jump back to source, instead of ending the walk at a dead end.
					currInterval->setStartTime(startTime);
					currInterval->setEndTime(endTime);
				}
				else{
					flag = currInterval->intersectionR(e->getInterval(), newInterval);
					/*newInterval = currInterval->intersection(e->getInterval());
					if (newInterval) flag = true;*/
				}
			}
			clock_gettime(CLOCK_MONOTONIC, &finish1);
			sampleEdgeTime += (finish1.tv_sec - start1.tv_sec) + (finish1.tv_nsec - start1.tv_nsec)/pow(10,9);
			/*cout << "Sampled from " << currNode << ", with budget = " << budget << ", d = " << direction << endl;
			if (newInterval){
				cout << "Found Edge to " << e->getDestId() << ", ( " << e->getStartTime() << ", " << e->getEndTime() << ")" << endl;
				cout << "newInterval = ( " << newInterval->getStartTime() << ", " << newInterval->getEndTime() << ")" << endl;
			}
			*/
			clock_gettime(CLOCK_MONOTONIC, &start1);

			if (n->getNumEdges(direction) == 1 && (flag == false)){   //this is also practically a dead-end - one edge that doesn't satisfy constraint
                n = nodes[src];
                currNode = src; //jump back to source, instead of ending the walk at a dead end.
                currInterval->setStartTime(startTime);
                currInterval->setEndTime(endTime);
			}



			if (flag){
				stopIndex += 1;
				if (stopIndex <= numStops){
					stops[stopIndex].setNodeId(e->getDestId());
					stops[stopIndex].setStartTime(newInterval->getStartTime());
					stops[stopIndex].setEndTime(newInterval->getEndTime());
					if (stops[stopIndex].getNodeId() == dst) flagSeenDest = true;
				}
				currNode = e->getDestId();
				currInterval->setStartTime(newInterval->getStartTime());
				currInterval->setEndTime(newInterval->getEndTime());
			}
			clock_gettime(CLOCK_MONOTONIC, &finish1);
			sampleEdgeTimeN += (finish1.tv_sec - start1.tv_sec) + (finish1.tv_nsec - start1.tv_nsec)/pow(10,9);
			if ((budget == 0) || (noEdges)) break;
		}
		free(currInterval);
	}
	clock_gettime(CLOCK_MONOTONIC, &finish);
	double buildStopsTime = (finish.tv_sec - start.tv_sec) + (finish.tv_nsec - start.tv_nsec)/pow(10,9);
	//cout <<" Time for sampling edges = " << sampleEdgeTime/buildStopsTime << ", " << sampleEdgeTimeN/buildStopsTime << ", " << sampleEdgeTimeO/sampleEdgeTime <<  endl;
	//if (flagSeenDest) return -1;
	return stopIndex+1;
}

void addInterval(std::vector<Interval*> &intervalList, int startTime, int endTime){
	Interval dummy;
	for (int i = 0; i < intervalList.size(); i++){
        if (endTime < intervalList[i]->getStartTime()){
            intervalList.insert(intervalList.begin()+i, new Interval(startTime, endTime));
            return;
        }
		if ((startTime <= intervalList[i]->getEndTime())){
			int minStartTime = dummy.min(startTime, intervalList[i]->getStartTime());
			int j = i + 1;
			while ((j < intervalList.size()) && (endTime >= intervalList[j]->getStartTime()))
				j++;
			int maxEndTime = dummy.max(endTime, intervalList[j-1]->getEndTime());
			for (int k = i; k < j; k++) free(intervalList[k]);
			intervalList.erase(intervalList.begin()+i, intervalList.begin()+j);
			intervalList.insert(intervalList.begin()+i, new Interval(minStartTime, maxEndTime));
			return;
		}
		if ((startTime <= intervalList[i]->getStartTime()) && (endTime >= intervalList[i]->getStartTime())){
			int minStartTime = startTime;
			int j = i + 1;
			while ((j < intervalList.size()) && (endTime >= intervalList[j]->getStartTime()))
				j++;
			int maxEndTime = dummy.max(endTime, intervalList[j-1]->getEndTime());
			for (int k = i; k < j; k++) free(intervalList[k]);
			intervalList.erase(intervalList.begin()+i, intervalList.begin()+j);
			intervalList.insert(intervalList.begin()+i, new Interval(minStartTime, maxEndTime));
			return;
		}
	}
	intervalList.push_back(new Interval(startTime, endTime));
}

int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
    if (src == dst) return 1;



	int numSrcStops = buildStops(src, true, startTime, endTime, dst, c, fractional);
	if (numSrcStops == 0) return 0;
	if (numSrcStops == -1) return 1;
	int numDstStops = buildStops(dst, false, startTime, endTime, src, c, fractional);
	if (numDstStops == -1) return 1;


	int answer = 0;

    	std::vector<Interval*> intervalList;


    	qsort(srcStops, numSrcStops, sizeof(Stop), stopCompare);
	qsort(dstStops, numDstStops, sizeof(Stop), stopCompare);

    int srcIndex = 0, dstIndex = 0;
    while (srcIndex < numSrcStops && dstIndex < numDstStops){
        int sNode = srcStops[srcIndex].getNodeId(), dNode = dstStops[dstIndex].getNodeId();
        if (sNode == -1) srcIndex++;
        else if (dNode == -1) dstIndex++;
        else if (sNode < dNode) srcIndex++;
        else if (dNode < sNode) dstIndex++;
        else if (sNode == dNode){
            int endSIndex = srcIndex, endDIndex = dstIndex;
            while (endSIndex < numSrcStops && srcStops[endSIndex].getNodeId() == sNode) endSIndex++;
            while (endDIndex < numDstStops && dstStops[endDIndex].getNodeId() == dNode) endDIndex++;
            //if (endSIndex == numSrcStops) endSIndex = numSrcStops - 1;
            //if (endDIndex == numDstStops) endDIndex = numDstStops - 1;

            for (int sI = srcIndex; sI < endSIndex; sI++){
                for (int dI = dstIndex; dI < endDIndex; dI++){
                    int tlength = srcStops[sI].intersect(dstStops[dI]);
                    if (tlength > 0){
                        int s = srcStops[sI].getStartTime(), e = srcStops[sI].getEndTime();
                        if (s < dstStops[dI].getStartTime()) s = dstStops[dI].getStartTime();
                        if (dstStops[dI].getEndTime() != -1){
                                if (e == -1) e = dstStops[dI].getEndTime();
                                else if (e > dstStops[dI].getEndTime()) e = dstStops[dI].getEndTime();
                        }
                        addInterval(intervalList, s, e);
                    }
                }
            }


            srcIndex = endSIndex + 1;
            dstIndex = endDIndex + 1;
        }
	}
	int tlength = 0;

	for (int i = 0; i < intervalList.size(); i++){
		int intervalLength = intervalList[i]->getEndTime() - intervalList[i]->getStartTime() + 1;
		if (fractional) tlength += intervalLength;	//closed intervals
		else if (tlength < intervalLength) tlength = intervalLength;
	}
	if (tlength >= c) answer = 1;
	for (int j = 0; j < intervalList.size(); j++){
			delete intervalList[j];
    }

    intervalList.clear();
    return answer;
}
