#pragma once
#include "hashTerm.h"

/*
 BSTNode (Back Suffix Tree Node) provides n-gram information.
*/
class BSTNode{
public:
    char *h; // where h means a single term history in this node 
    HashTerm *terms;
    BSTNode *parent;
    HashBSTNode *children;
}

class PredictBSTNode : public BSTNode{
public:
    float Beta; // back-off coefficient
}

class TrainBSTNode : public BSTNode{
public:
    TOTAL_COUNT_TYPE c; // c(*|h) where h means term histories until this node
    TOTAL_COUNT_TYPE t; // t_{h*} where h is the same above
}

