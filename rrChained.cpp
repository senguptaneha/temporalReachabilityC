#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <math.h>
#include <map>

#include "TemporalGraph.h"

void TemporalGraph::addEdgeToIndex(int src, int dst, int startTime){
}

void TemporalGraph::removeEdgeFromIndex(int src, int dst, int startTime, int endTime){
}
//last 3 params not actually used, only for compatibility across methods.
int TemporalGraph::buildStops(int src, bool direction, int startTime, int endTime, int dst, int c, bool fractional){
	//cout << "ARROW log for Query " << src << " to " << dst << endl;
	Stop *stops = srcStops;
	if (!direction) stops = dstStops;
	for (int i = 0; i <= numStops; i++) stops[i].reset();

	int stopIndex = 0;
//	int budget = 2 * numStops;
	stops[stopIndex].setNodeId(src);
	if (direction)stops[stopIndex].setStartTime(startTime);
	else stops[stopIndex].setStartTime(endTime);

	bool flagSeenDest = false;
	for (int i = 0; i < numWalks; i++){
		int budget = 2 * walkLength;
		int currNode = src;
		bool noEdges = false;

		int t = startTime;
		if (!direction) t = endTime;

		for (int j = 0; j < walkLength; j++){
			TemporalNode *n = nodes[currNode];
			TemporalEdge *e;
			bool foundEdge = false;
			while (budget > 0){
				budget -= 1;
				e = n->sampleEdge(direction);

				if (e == NULL){
					noEdges = true;
					break;
				}
				if (direction){
					if ((e->getStartTime() >= t) && (e->getEndTime() <= endTime)){
						foundEdge = true;
						break;
					}
				}
				else{
					if ((e->getStartTime() >= startTime) && (e->getEndTime() <= t)){
						foundEdge = true;
						break;
					}
				}
			}
			if (foundEdge){
				stopIndex += 1;
				if (stopIndex <= numStops){
					stops[stopIndex].setNodeId(e->getDestId());
					if (direction)
						stops[stopIndex].setStartTime(e->getEndTime());
					else
						stops[stopIndex].setStartTime(e->getStartTime());
					if (stops[stopIndex].getNodeId() == dst) flagSeenDest = true;
					/*cout << "Traversed edge (" << currNode << ", " << e->getDestId() << " : [" << e->getStartTime() << ", " << e->getEndTime() << "] ";
					cout << "EarliestArrival[v] / LatestDeparture[v] = " << stops[stopIndex].getStartTime() << endl;*/

				}
				currNode = e->getDestId();
				if (direction) t = e->getEndTime();
				else t = e->getStartTime();
			}
			if ((budget == 0) || (noEdges)) break;
		}

//		if (budget == 0) break;
	}
	if (flagSeenDest)return -1;
	return stopIndex+1;
}

/*
int TemporalGraph::buildStops(int src, bool direction, int startTime, int endTime, int dst, int c, bool fractional){
	Stop *stops = srcStops;
	if (!direction) stops = dstStops;
	for (int i = 0; i <= numStops; i++) stops[i].reset();
	int stopIndex = 0, numProcessed = 0;

	stops[stopIndex].setNodeId(src);
	if (direction) stops[stopIndex].setStartTime(startTime);
	else stops[stopIndex].setStartTime(endTime);

	stops[stopIndex].numWalks = numWalks;
	stops[stopIndex].walkLength = walkLength;
	stopIndex++;


	while (numProcessed < stopIndex){
		Stop y = stops[numProcessed];
		numProcessed += 1;
		if (y.walkLength <= 0) continue;
		//cout << "--------------ITERATION START----------------" << endl;
		//cout << "Stop " << y.getNodeId() << ", " << y.getStartTime() << ", " << y.numWalks << ", " << y.walkLength << endl;
		//cout << "num Processed = " << numProcessed << ", stopIndex = " << stopIndex << endl;

		int currNode = y.getNodeId();

		TemporalNode *n = nodes[currNode];
		if (n->getNumEdges(direction) < y.numWalks){
			//cout << "Case 1;" << endl;

			vector<Stop> candidates;
			for (int i = 0; i < n->getNumEdges(direction); i++){
				TemporalEdge *e = n->getEdgeAt(i, direction);
				//cout << "e = " << e->getStartTime() << ", " << e->getEndTime() << endl;
				Stop tempS = Stop();
				tempS.setNodeId(e->getDestId());
				if ((direction)&&(e->getStartTime() >= y.getStartTime())&&(e->getEndTime() <= endTime)){
					tempS.setStartTime(e->getEndTime());
					candidates.push_back(tempS);
				}
				else if((!direction)&& ( e->getEndTime()<= y.getStartTime())&&(e->getStartTime() >= startTime)){
					tempS.setStartTime(e->getStartTime());
					candidates.push_back(tempS);
				}
			}
			int k = candidates.size(), l = 0;
			if (k > 0){
				for (int j = 0; j < k; j++){
					candidates[j].numWalks = y.numWalks/k;
					l += candidates[j].numWalks;
				}
				int j = 0;
				while (l < y.numWalks){
					candidates[j].numWalks += 1;
					l += 1;
					j += 1;
				}
				j = 0;
				while ((j < k) && (stopIndex < numStops)){
					Stop c = candidates[j];
					stops[stopIndex].setNodeId(c.getNodeId());
					stops[stopIndex].setStartTime(c.getStartTime());
					stops[stopIndex].numWalks = c.numWalks;
					stops[stopIndex].walkLength = y.walkLength - 1;
					stopIndex += 1;
				}
			}
		}
		else{
			//cout << "Case 2;" << endl;
			bool noEdges, foundEdge;
			int numSamples = 0;
			map<TemporalEdge, int> samples;
			for (int k = 0; k < c_budget * y.walkLength * y.numWalks; k++){
				foundEdge = false;
				TemporalEdge *e = n->sampleEdge(direction);
				if (e == NULL){
					noEdges = true;
					break;
				}
				if (direction){
					if ((e->getStartTime() >= y.getStartTime()) && (e->getEndTime() <= endTime)){
						numSamples += 1;
						foundEdge = true;
					}
				}
				else{
					if ((e->getStartTime() >= startTime) && (e->getEndTime() <= y.getStartTime())){
						numSamples += 1;
						foundEdge = true;
					}
				}
				if (foundEdge){
					samples[*e]++;
				}
				if (noEdges || (numSamples >= y.numWalks)) break;
			}
			for (map<TemporalEdge,int>::iterator it = samples.begin(); it != samples.end() && stopIndex < numStops; ++it){
				TemporalEdge e = it->first;
				int nw = it->second;
				stops[stopIndex].setNodeId(e.getDestId());
				stops[stopIndex].setStartTime(direction? e.getEndTime() : e.getStartTime());
				stops[stopIndex].numWalks = nw;
				stops[stopIndex].walkLength = y.walkLength - 1;
				stopIndex += 1;

			}

		}
		//cout << "--------------ITERATION END----------------" << endl;
	}
	return stopIndex;
}
*/

int TemporalGraph::isReachable(int src, int dst, int startTime, int endTime, int c, bool fractional){
	if (src == dst) return 1;

	int numSrcStops = buildStops(src, true, startTime, endTime, dst, c, fractional);
	cout << "numSRCStops = " << numSrcStops << endl;
	if (numSrcStops == -1) return 1;
	int numDstStops = buildStops(dst, false, startTime, endTime, src, c, fractional);
	if (numDstStops == -1) return 1;

	qsort(srcStops, numSrcStops, sizeof(Stop), stopCompare);
	qsort(dstStops, numDstStops, sizeof(Stop), stopCompare);
	//cout << "numSrcStops = " << numSrcStops << ", numDstStops = " << numDstStops << endl;
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
					if (srcStops[sI].getStartTime() <= dstStops[dI].getStartTime()){
						/*cout << sNode << ", " << srcStops[sI].getNodeId() << ", " << srcStops[sI].getStartTime() << ", " <<
						dNode << ", " << dstStops[dI].getNodeId() << ", " << dstStops[dI].getStartTime() << endl;*/
						return 1;
					}
                }
            }


            srcIndex = endSIndex + 1;
            dstIndex = endDIndex + 1;
        }
	}

	return 0;
}
