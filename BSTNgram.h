#pragma once
#include "BSTNode.h"

class BSTNgram
{
    char n;
    TERM_LEN maxlenx;
    TERM_LEN maxleny;
public:
	static TERM_LEN termlen(const char *target);
	static int keyequalWithlen(const char *x, const char *targetx, TERM_LEN lenTargetx);

    void add(const Hyp &hyp);
    void del(const Hyp &hyp);
    void prob(const Hyp &hyp);    
};

class TrainBSTNgram : public BSTNgram
{
    TrainBSTNode bst;
public:
    TrainBSTNgram(void);
    ~TrainBSTNgram(void);
    void add(const Hyp &hyp);
    void del(const Hyp &hyp);
    void prob(const Hyp &hyp);    
};



class PredictBSTNgram : public BSTNgram
{
    PredictBSTNode bst;
public:
    PredictBSTNgram(void);
    ~PredictBSTNgram(void);
    void read(const char *filename);
    void prob(const Hyp &hyp);
};


