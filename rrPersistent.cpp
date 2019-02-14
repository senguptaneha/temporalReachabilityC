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
	Stop *stops = srcStops;
	if (!direction) stops = dstStops;
	for (int i = 0; i <= numStops; i++) stops[i].reset();

	int stopIndex = 0;
	stops[stopIndex].setNodeId(src);
	stops[stopIndex].setStartTime(startTime);
	stops[stopIndex].setEndTime(endTime);

	for (int i = 0; i < numWalks; i++){
		int budget = c_budget * walkLength;
		Interval *currInterval = new Interval(startTime, endTime);
		int currNode = src;
		bool noEdges = false;
		for (int j = 0; j < walkLength; j++){
			TemporalNode *n = nodes[currNode];
			TemporalEdge *e;
			Interval *newInterval = NULL;
			/*cout << "Sampling from " << currNode << ", with budget = " << budget << ", d = " << direction << endl;
			cout << "currInterval is ( " << currInterval->getStartTime() << ", " << currInterval->getEndTime() << ") " << endl;*/
			while ((newInterval == NULL) && (budget > 0)){
				budget -= 1;
				e = n->sampleEdge(direction);

				if (e == NULL){
					n = nodes[src];
					currNode = src; //jump back to source, instead of ending the walk at a dead end.
					currInterval->setStartTime(startTime);
					currInterval->setEndTime(endTime);
				}
				else newInterval = currInterval->intersection(e->getInterval());
			}
			/*cout << "Sampled from " << currNode << ", with budget = " << budget << ", d = " << direction << endl;
			if (newInterval){
				cout << "Found Edge to " << e->getDestId() << ", ( " << e->getStartTime() << ", " << e->getEndTime() << ")" << endl;
				cout << "newInterval = ( " << newInterval->getStartTime() << ", " << newInterval->getEndTime() << ")" << endl;
			}
			*/
			if (n->getNumEdges(direction) == 1 && newInterval == NULL){   //this is also practically a dead-end - one edge that doesn't satisfy constraint
                n = nodes[src];
                currNode = src; //jump back to source, instead of ending the walk at a dead end.
                currInterval->setStartTime(startTime);
                currInterval->setEndTime(endTime);
			}

			if (newInterval){
				stopIndex += 1;
				if (stopIndex <= numStops){
					stops[stopIndex].setNodeId(e->getDestId());
					stops[stopIndex].setStartTime(newInterval->getStartTime());
					stops[stopIndex].setEndTime(newInterval->getEndTime());
				}
				currNode = e->getDestId();
				free(currInterval);
				currInterval = newInterval;
				newInterval = NULL;
			}
			if ((budget == 0) || (noEdges)) break;
		}
		free(currInterval);
	}
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
	int numDstStops = buildStops(dst, false, startTime, endTime, src, c, fractional);

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

/*
	for (int i = 0; i < numSrcStops; i++){
		if (srcStops[i].getNodeId() == -1) continue;
		for (int j = 0; j < numDstStops; j++){
			if (dstStops[j].getNodeId() == -1) continue;
			int tlength = srcStops[i].intersect(dstStops[j]);
			if (tlength == 0) continue;
			int s = srcStops[i].getStartTime(), e = srcStops[i].getEndTime();
			if (s < dstStops[j].getStartTime()) s = dstStops[j].getStartTime();
        		if (dstStops[j].getEndTime() != -1){
                		if (e == -1) e = dstStops[j].getEndTime();
                		else if (e > dstStops[j].getEndTime()) e = dstStops[j].getEndTime();
        		}
            addInterval(intervalList, s, e);
//			for (int t = s; t <= e; t++)
//				track[t] = true;
		}
	}
	*/

	for (int i = 0; i < intervalList.size(); i++){
		if (intervalList[i]->getStartTime() <= startTime && intervalList[i]->getEndTime() >= endTime){
            answer = 1;
            break;
        }
	}

	for (int j = 0; j < intervalList.size(); j++){
			delete intervalList[j];
    }
    intervalList.clear();
    return answer;
/*
	bool flag = true;
	for (int i = startTime; i<= endTime; i++)
		flag = flag & track[i];
	if (flag) return 1;
	return 0;
	*/
}
