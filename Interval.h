#ifndef INTERVAL_H
#define INTERVAL_H

class Interval{

public:
	/*Constructors: each of them sets unspecified time values to -1, signifying Infinity*/
	Interval();
	Interval(int isStartTime);
	Interval(int isStartTime, int isEndTime);

	/**Returns a new interval that represents the intersection of this and other.
	 * Returns NULL if no overlap exists*/
	Interval *intersection(Interval *other);
	bool intersectionR(Interval *other, Interval *result);

	int compareTo(Interval *other);

	/**Returns true if other is a subInterval of this interval, false otherwise*/
	bool isSubInterval(Interval *other);
    bool isSubIntervalT(int sTime, int eTime);

	bool contains(int t);
	bool doesOverlap(Interval *other);
	void setEndTime(int isEndTime);
	void setStartTime(int isStartTime);

	int getStartTime();
	int getEndTime();

	int min(int x1, int x2);
	int max(int x1, int x2);
private:
	int startTime;
	int endTime;

};

#endif
