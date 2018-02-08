#include <iostream>
#include <vector>

#define initialEntrySize 10
#define initialTableSize 10
#define incrEntrySize 10
#define incrTableSize 10

using namespace std;


/*Utility Functions*/
//get the bit of bitIndex MSB - helper to labelToIndex
int checkNthBit(int num, int bitIndex){
	int bitToCheck = sizeof(int)*8 - bitIndex;
	int bitStatus = (num >> bitToCheck) & 1;
	if (bitStatus == 0) return 0;
	return 1;
}

unsigned djb_hash(void *key, int len){
	unsigned char *p = (unsigned char*)key;
	unsigned h = 0;
	int i;
	for (i = 0; i < len; i++){
		h = 33 * h + p[i];
	}
	return h % initialTableSize;	//Table size never changes
}

unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed )

{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.
	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value
	unsigned int h = seed ^ len;

	// Mix 4 bytes at a time into the hash
	const unsigned char * data = (const unsigned char *)key;
	while(len >= 4)
	{
		unsigned int k = *(unsigned int *)data;
		k *= m;
		k ^= k >> r;
		k *= m;

		h *= m;
		h ^= k;

		data += 4;
		len -= 4;
	}

	// Handle the last few bytes of the input array
	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.
	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h % initialTableSize;

}

int setLabel(int label, int numBits){
	int numBitsInt = sizeof(int)*8;
	if (numBits == numBitsInt) return label;
	int mask = 1;
	for (int i = 1; i < numBitsInt - numBits; i++)
		mask = (mask << 1) | 1;
	mask = ~mask;
	return (label & mask);
}

int getHash(int label, int numBits){
	int intLabel[2];
	intLabel[1] = numBits;
	intLabel[0] = setLabel(label, numBits);
	int hashValue = MurmurHash2(&intLabel[0], sizeof(int)*2, 23471);
/*

	char strLabel[sizeof(int)*8+1];
	for (int i = 0; i < numBitsInt; i++) strLabel[i] = 'X';
	strLabel[numBitsInt] = '\0';
	int bitIndex = 0;
	while (bitIndex < numBits){
		int bitStatus = checkNthBit(label,bitIndex);
		if (bitStatus == 1) strLabel[bitIndex] = '1';
		else strLabel[bitIndex] = '0';
		bitIndex += 1;
	}
	int hashValue = MurmurHash2(&strLabel[0], numBitsInt,23471);
*/
	return hashValue;
}

int getCommonLabel(int startTime, int endTime){
	int x = (startTime ^ endTime);
	//Count the number of uninterrupted 1's
	int mask = 1, latest = 0;
	for (int i = 1; i <= sizeof(int)*8; i++){
		if ((x & mask) != 0) latest = i;
		mask = mask << 1;
	}
	int numBits = sizeof(int)*8 - latest;
	int ll = sizeof(int);
	//ll = ll *8;
	return numBits;
}

/**********************************************************/


struct TrieTableEntry{
	int *edgeList;
	int numEdges;
	int listSize;

	int label, numBits;
	TrieTableEntry *next;	//for chaining

	TrieTableEntry(){
		edgeList = NULL;
		numEdges = 0;
		listSize = 0;
		label = -1;
		numBits = -1;
		next = NULL;
	}

	void initialize(){
		edgeList = NULL;
		numEdges = 0;
		listSize = 0;
		label = -1;
		numBits = -1;
		next = NULL;
	}


	void allocate(int isLabel, int isNumBits){
		edgeList = (int*)malloc(sizeof(int)*initialEntrySize);
		numEdges = 0;
		listSize = initialEntrySize;
		label = setLabel(isLabel, isNumBits);
		numBits = isNumBits;
		next = NULL;
	}

	void deallocate(){
		free(edgeList);
		listSize = 0;
		numEdges = 0;
		numBits = -1;
		label = -1;
	}

	void copy(TrieTableEntry *other){
		edgeList = other->edgeList;
		numEdges = other->numEdges;
		listSize = other->listSize;
		label = other->label;
		numBits = other->numBits;
		next = other->next;
	}

	void addEdge(int destId){
		if (listSize > numEdges){
			edgeList[numEdges] = destId;
			numEdges += 1;
		}
		else{
			int *temp = (int*)malloc(sizeof(int)*(listSize + incrEntrySize));
			for (int i = 0; i < listSize; i++)
				temp[i] = edgeList[i];
			free(edgeList);
			edgeList = temp;
			listSize += incrEntrySize;
			edgeList[numEdges] = destId;
			numEdges += 1;
		}
	}

	int removeEdge(int destId){
		int destIndex = -1;
		for (int i = 0; i < numEdges; i++){
			if (edgeList[i] == destId){
				destIndex = i;
				break;
			}
		}
		if (destIndex == -1) return numEdges;	//nothing to delete
		if (destIndex < numEdges - 1)
			edgeList[destIndex] = edgeList[numEdges - 1];
		numEdges -= 1;
		return numEdges;
	}

	bool compareLabel(int otherLabel, int otherNumBits){
		if (numBits != otherNumBits) return false;
		if (label != otherLabel) return false;
/*		int bitIndex = 0;
		while (bitIndex < numBits){
			if (checkNthBit(label,bitIndex) != checkNthBit(otherLabel,bitIndex)) return false;
			bitIndex += 1;
		}
*/
		return true;
	}

	int randomNeighbor(int presentSample, int numPresentEdges){
		cout << "randomNeighbor: over here1"<< endl;
		if (numEdges <= 0) return presentSample;
		cout << "randomNeighbor: over here , numEdges = " << numEdges << endl;
		int z = rand() % (numEdges + numPresentEdges);
		if (z < numEdges) return edgeList[rand() % numEdges];
		return presentSample;
	}

	void printEntry(){
		for (int i = 0; i < numEdges; i ++){
			cout << edgeList[i] << ", ";
		}
		cout << endl;
	}
};

struct TrieTable{
	TrieTableEntry *table;

	TrieTable(){
		table = (TrieTableEntry*)malloc(sizeof(TrieTableEntry)*initialTableSize);
		for (int i = 0; i < initialTableSize; i++){
			table[i].initialize();
		}
	}

	void printTable(){
		for (int i = 0; i < initialTableSize; i++){
			cout << i << ":" << endl;
			TrieTableEntry *ttE = &table[i];
			if (ttE->label != -1){
				while (ttE){
					ttE->printEntry();
					ttE = ttE->next;
				}
			}
		}
	}


	TrieTableEntry *getEntryForLabel(int label, int numBits){
		int hashIndex = getHash(label, numBits);
		TrieTableEntry *temp = &table[hashIndex];
		while (temp){
//			cout << "GetEntryForLabel: 1" << temp << endl << flush;
			if (temp->compareLabel(label,numBits)){
				return temp;
			}
//			cout << "GetEntryForLabel: 2" << temp << endl << flush;
			temp = temp->next;
//			cout << "GetEntryForLabel: 3 " << temp << endl << flush;
		}
		return NULL;
	}

	void addEdge(int destId, int label, int numBits){
		int hashIndex = getHash(label,numBits);
		if ((table[hashIndex].label == -1)&&(table[hashIndex].numBits == -1)){
			table[hashIndex].allocate(label, numBits);
			table[hashIndex].addEdge(destId);
		}
		else if (!table[hashIndex].compareLabel(label,numBits)){
			TrieTableEntry *t = table[hashIndex].next, *pt;
			pt = &(table[hashIndex]);
			int foundFlag = 0;
			while (t){
				if (t->compareLabel(label,numBits)){
					t->addEdge(destId);
					foundFlag = 1;
					break;
				}
				pt = t;
				t = t->next;
			}
			if (foundFlag == 0){
				pt->next = new TrieTableEntry();
				t = pt->next;
				t->allocate(label, numBits);
				t->addEdge(destId);
			}
		}
		else table[hashIndex].addEdge(destId);
	}

	int removeEdge(int destId, int label, int numBits){
		int hashIndex = getHash(label,numBits);
//		cout << "DestId = " << destId << ", label = " << label << ", numBits = " << numBits << ", hashValue = " << hashIndex << endl;
		if ((table[hashIndex].label == -1) && (table[hashIndex].numBits == -1)) return 0;	//edge not found
		TrieTableEntry *head = &(table[hashIndex]);
		TrieTableEntry *temp = head, *pt = NULL;
		int numE = -1;
		while (temp){
			if (temp->compareLabel(label,numBits)){
				numE = temp->removeEdge(destId);
				break;
			}
			pt = temp;
			temp = temp->next;
		}
		if (numE != 0) return 0;	//edge not found or no entry to delete
		if (temp == head){	//or check if pt is NULL
			temp = head->next;
			head->deallocate();
			if (temp){
				head->copy(temp);
				free(temp);
			}
		}
		else{

			pt->next = temp->next;
			temp->deallocate();
			free(temp);

		}
		return 1;	//entryRemoved

	}

	int parentLabel(int label, int numBits){
		int mask = 1 << (sizeof(int)*8 - numBits);
		int nB = numBits;
		while (nB > 0){
			nB -= 1;
			if ((label & mask) == 0) break;
		}
		return nB;
	}

	void shiftEdges(TrieTableEntry *t, int *presentEdges, int numPresentEdges, int numLevels){
		//determine parent(index) to shift to
		int parentLabelBits = parentLabel(t->label, t->numBits);
		//for each edge, see if it is alive - can be very inefficient!
		int i = 0;
		while (i < t->numEdges){
			int destId = t->edgeList[i], aliveFlag = 0;
			for (int j = 0; j < numPresentEdges; j++){
				if (destId == presentEdges[j]){
					aliveFlag = 1;
					break;
				}
			}

			if (aliveFlag == 1){
				//if yes, delete from t, insert into parent
				int entryRemoved = removeEdge(destId, t->label, t->numBits);
				if (entryRemoved == 1) return;

				addEdge(destId, t->label, parentLabelBits);

			}
			else i++;
		}
	}

	void traverseRightmostPath(int timestep, int numLevels, int *presentEdges, int numPresentEdges){
		int t = timestep - 1, nB = sizeof(int)*8, ctr = 0;
		while ((nB > 0) && (ctr < numLevels)){
//			cout << "traverseRightmostPath CP 1" << endl << flush;
			TrieTableEntry *ttE = getEntryForLabel(t, nB);
//			cout << "traverseRightmostPath CP 1.2" << endl << flush;
			if (ttE){
//				cout << "traverseRightmostPath CP 1.5" << endl << flush;
				shiftEdges(ttE, presentEdges, numPresentEdges, numLevels);
//				cout << "traverseRightmostPath CP 1.6" << endl << flush;
			}
//			cout << "traverseRightmostPath CP 2" << endl << flush;
			nB -= 1;
			ctr += 1;
		}
	}

	int randomNeighbor(int label, int numBits, int presentSample, int numPresentEdges){
		TrieTableEntry *ttE = getEntryForLabel(label, numBits);
		if (ttE == NULL) return presentSample;
		return ttE->randomNeighbor(presentSample, numPresentEdges);
	}

};
