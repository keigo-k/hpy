/*
Slearp (structured learning and prediction) is the structured learning and predict toolkit for tasks such as g2p conversion, based on discriminative leaning.
Copyright (C) 2013, 2014 Keigo Kubo

Slearp is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

Slearp is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with slearp.  If not, see <http://www.gnu.org/licenses/>.

date:   2014/3/03
author: Keigo Kubo
belong: Nara Institute Sience and Technology (NAIST)
e-mail: keigo.kubo89{@}gmail.com   << Please transform {@} into @
*/

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
    Table *add(float theta, float d, float th, float p);
    bool decrease();
};

class HashTerm
{
	unsigned int hashSize;
	vector<char *> allStringVec;

	TERM_LEN termlen(const char *target);

	Term **hashWithlen
				(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);

	int keyequalWithlen(const char *x, const char *targetx, TERM_LEN lenTargetx);

public:
	Term **TermTable;
	unsigned int numTerm;
	TERM_LEN maxlenx;
	TERM_LEN maxleny;

	HashTerm(void);
	~HashTerm(void);   

	void initialize(unsigned int size);
	
	Term *registTrain(const char *targetx, const char *targety);
	Term *registPredict(const char *targetx, unsigned short storeSizex, const char *targety, unsigned short storeSizey);

	Term *refer(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety);
	void writeTerms(const char *write);
};

