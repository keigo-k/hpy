#pragma once
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "xorshift.h"
#include "hashTerm.h"

using namespace std;

class BSTNode
{
	unsigned int hashSize;
    const char *x;
    const char *y;
    TERM_LEN lenx;
    TERM_LEN leny;
    BSTNode *next;
};

class TrainBSTNode : public BSTNode{
    HashTrainTerm terms;
	TrainBSTNode **BSTNodeTable;
	TrainTerm **hashWithlen
				(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);

public:
    TOTAL_COUNT_TYPE c; // total count for the history
    TABLE_COUNT_TYPE t; // table count for the history

	TrainBSTNode(unsigned int BSTNodeSize, unsigned int TermSize);
    ~TrainBSTNode(void);

	TrainBSTNode *regist(const char *targetx, const char *targety);
	TrainBSTNode *refer(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);
	void delNode(const char *targetx, const char *targety);
};

class PredictBSTNode : public BSTNode{
    HashPredictTerm terms;
	PredictBSTNode **BSTNodeTable;
	PredictTerm **hashWithlen
				(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);

public:
    float b; // backoff coefficient

	PredictBSTNode(unsigned int BSTNodeSize, unsigned int TermSize);
    ~PredictBSTNode(void);

	PredictBSTNode *regist(const char *targetx, const char *targety);
	PredictBSTNode *refer(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);
	void delNode(const char *targetx, const char *targety);
};

