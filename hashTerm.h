#pragma once
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "xorshift.h"

using namespace std;

extern char unknownChar;
extern char separateChar;
extern char joinChar;

typedef unsigned short TABLE_COUNT_TYPE;
typedef unsigned int TOTAL_COUNT_TYPE;
typedef unsigned short TERM_LEN;

typedef struct table{
    TABLE_COUNT_TYPE c;
    struct table *next;
} Table;

class Term{
public:
    const char *x;
    const char *y;
    TERM_LEN lenx;
    TERM_LEN leny;
    Term *next;
};

class PredictTerm : public Term{
public:
    float p;
};

class TrainTerm : public Term{
public:
    TOTAL_COUNT_TYPE c;
    TABLE_COUNT_TYPE t;
    Table table;
    TrainTerm(void);
    ~TrainTerm(void);
    bool add(float theta, float d, float th, float p);
    bool decrease();
};

class HashTerm
{
	unsigned int hashSize;
	vector<char *> allStringVec;
};

class HashTrainTerm : public HashTerm
{
	Term **hashWithlen
				(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);

public:
	TrainTerm **TermTable;
	HashTrainTerm(unsigned int size);
    ~HashTrainTerm(void);
	
	TrainTerm *regist(const char *targetx, const char *targety);
	TrainTerm *registWithMemory(const char *targetx, unsigned short storeSizex, const char *targety, unsigned short storeSizey);
	TrainTerm *refer(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);

	void delTerm(const char *targetx, const char *targety);

	void writeTerms(const char *write);
};

class HashPredictTerm : public HashTerm
{
	Term **hashWithlen
				(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);

public:
	PredictTerm **TermTable;
    HashPredictTerm(unsigned int size);
    ~HashPredictTerm(void);
	
	PredictTerm *regist(const char *targetx, const char *targety);
	PredictTerm *registWithMemory(const char *targetx, unsigned short storeSizex, const char *targety, unsigned short storeSizey);
	PredictTerm *refer(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);

	void delTerm(const char *targetx, const char *targety);

	void writeTerms(const char *write);
};

