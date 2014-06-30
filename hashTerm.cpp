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

#include "hashTerm.h"
#include <iostream>
#include <fstream>


TrainTerm::TrainTerm(){
    table.c=0;
    table.next=NULL;
}

TrainTerm::~TrainTerm(){
	Table *tmp, *p=table.next;
	for(;p!=NULL;p=tmp){
		tmp=p->next;
		free(p);
	}
}

Table *TrainTerm::add(float theta, float d, float th, float p){
    if(t==0){
        ++c;
        ++t;
        table.c=1;
        return &table;
    }

    float denominator = c-d*t + (theta+d*th)*p;

    ++c;
    float rand = xorshift();
    Table *tp=&table;
    do{
        rand-=((float)tp->c - d)/denominator;
        if(rand<=0){
            tp->c++;        
            return tp;
        }

        if(tp->next==NULL){
            break;
        }

        tp=tp->next;
    }while(1);
    
    ++t;
	if((tp->next=(Table *)malloc(sizeof(Table)))==NULL){
        cerr << "ERROR:Can not get memory in malloc.\nYou must need more memory.\n";
		exit(EXIT_FAILURE);
	}

    tp=tp->next;
    tp->c=1;
    tp->next=NULL;
    return tp;
}

bool TrainTerm::decrease(){
    if(t==1){
        --c;
        --table.c;
        if(c==0){
            t=0;
            return true;
        }
        return false;      
    }

    Table *p;
    float rand = xorshift();
    rand-=((float)table.c)/c;
    if(rand<=0){
        --c;
        --table.c;
        if(table.c==0){
            --t;
            table.c=(table.next)->c;
            p=table.next;
            table.next=(table.next)->next;
            free(p);
            return true;
        }
        return false;      
    }

    Table *tmp=&table;
    p=table.next;
    do{
        rand-=((float)p->c)/c;
        if(rand<=0 || p->next==NULL){
            break;
        }
        tmp=p;
        p=p->next;
    }while(1);

    --c;
    --(p->c);
    if(p->c==0){
        --t;
        tmp->next=p->next;
        free(p);
        return true;
    }

    return false;
}

HashTerm::HashTerm() : TermTable(NULL){
}

HashTerm::~HashTerm(){
	if(TermTable==NULL){
		return;
	}

	vector<char *>::iterator it=allStringVec.begin();
	vector<char *>::iterator endit=allStringVec.end();
	for(;it!=endit;++it){
		free(*it);
	}

	Term *p,*tmp;
	unsigned int i;
	for(i=0;i<hashSize;++i){
		for(p=TermTable[i];p!=NULL;p=tmp){
			tmp=p->next;
			delete p;
		}
	}
	free(TermTable);
}

TERM_LEN HashTerm::termlen(const char *target){
	TERM_LEN len=0;
	while(*target!='\0'){
		if(*target==joinChar || *target==unknownChar){
			++len;
		}
		++target;
	}

	++len;
	return len;
}

Term **HashTerm::hashWithlen(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety){
	unsigned int value=0;
	const char *tmp=targetx;
	char c;
	while(lenTargetx){
		--lenTargetx;
		while((c=*targetx)!=unknownChar && c!=joinChar 
			&& c!=separateChar && c!='\0'){
			value +=  (c * c) * (127 << lenTargetx );
			++targetx;
		}
		++targetx;
		value/=7;
	}
	while(targetx!=tmp){
		while((c=*tmp)!=unknownChar && c!=joinChar 
			&& c!=separateChar && c!='\0'){
			value +=  (c * c) * (127 << lenTargetx );
			++tmp;
		}
		value/=3;
		++lenTargetx;
		++tmp;
	}

	return &(TermTable[value%hashSize]);
}

int HashTerm::keyequalWithlen(const char *x, 
					const char *targetx, TERM_LEN lenTargetx){
	// have already verified that two length is equal.

	char c;
	while(lenTargetx){
		while((c=*targetx)!=unknownChar && c!=joinChar
			&& c!=separateChar && c!='\0'){
			if(*x!=c){
				return 0;
			}
			++x;
			++targetx;
		}
		if(*x!=unknownChar && *x!=joinChar
			&& *x!=separateChar && *x!='\0'){
			return 0;
		}
		++x;
		++targetx;
		--lenTargetx;
	}

	return 1;
}

void HashTerm::initialize(unsigned int size){
	unsigned int i;

	numTerm=0;
	maxlenx=0;
	maxleny=0;
	hashSize=size;

	if((TermTable=(Term **)malloc(size*sizeof(Term *)))==NULL){
        cerr << "ERROR:Can not get memory in malloc.\nYou must need more memory.\n";
		exit(EXIT_FAILURE);
	}
	
	for(i=0;i<size;++i){
		TermTable[i]=NULL;
	}
}

Term *HashTerm::registTrain(const char *targetx, const char *targety){
	TERM_LEN lenTargetx=termlen(targetx);
	TERM_LEN lenTargety=termlen(targety);
	Term **pp=hashWithlen(targetx, lenTargetx, targety, lenTargety);
	Term *p=*pp;

	for(;p!=NULL;p=p->next){
		if(p->lenx==lenTargetx
            && p->leny==lenTargety
			&& keyequalWithlen(p->x, targetx, lenTargetx)
			&& keyequalWithlen(p->y, targety, lenTargety)){
			
			return p; // already regist
		}
	}

	p=new TrainTerm;
	p->next=*pp;
	*pp=p;

	p->x=targetx;
	p->lenx=lenTargetx;
	if(lenTargetx>maxlenx){
		maxlenx=lenTargetx;
	}

	p->y=targety;
	p->leny=lenTargety;
	if(lenTargety>maxleny){
		maxleny=lenTargety;
	}

	++numTerm;
    return p;
}

Term *HashTerm::registPredict(const char *targetx, unsigned short storeSizex, const char *targety, unsigned short storeSizey){
	TERM_LEN lenTargetx=termlen(targetx);
	TERM_LEN lenTargety=termlen(targety);

	Term **pp=hashWithlen(targetx, lenTargetx, targety, lenTargety);
	Term *p=*pp;

	for(;p!=NULL;p=p->next){
		if(p->lenx==lenTargetx
            && p->leny==lenTargety
			&& keyequalWithlen(p->x, targetx, lenTargetx)
			&& keyequalWithlen(p->y, targety, lenTargety)){
			
			return p; // already regist
		}
	}

	p=new PredictTerm;
	p->next=*pp;
	*pp=p;

	char *pstr;
	if((pstr=(char *)malloc(storeSizex+storeSizey)) == NULL) {
		cerr << "ERROR:Can not get memory in malloc.\nYou must need more memory.\n";
		exit(EXIT_FAILURE);
	}
	allStringVec.push_back(pstr);

	memcpy(pstr, targetx, storeSizex);
	p->x=pstr;
	p->lenx=lenTargetx;
	if(lenTargetx>maxlenx){
		maxlenx=lenTargetx;
	}

	memcpy(pstr+storeSizex, targety, storeSizey);
    p->y=pstr+storeSizex;
    p->leny=lenTargety;
	if(lenTargety>maxleny){
		maxleny=lenTargety;
	}

	++numTerm;
	return p;
}

Term *HashTerm::refer(const char *targetx, TERM_LEN lenTargetx, const char *targety, TERM_LEN lenTargety){
	Term **pp=hashWithlen(targetx, lenTargetx, targety, lenTargety);
	Term *p=*pp;

	for(;p!=NULL;p=p->next){
		if(p->lenx==lenTargetx
		    && p->leny==lenTargety
			&& keyequalWithlen(p->x, targetx, lenTargetx)
			&& keyequalWithlen(p->y, targety, lenTargety)){
			return p;
		}
	}
	return NULL;
}

void HashTerm::writeTerms(const char *write){
	ofstream WRITE;
	string writeFile(".term");
	writeFile=write+writeFile;
	WRITE.open(writeFile.c_str(), ios_base::trunc);
	if (!WRITE)
	{
		cerr << "ERROR:Can not write term: " << writeFile << endl;
		exit(EXIT_FAILURE);
	}else{
		Term *p;
		unsigned int i;
		for(i=0;i<hashSize;++i){
			for(p=TermTable[i];p!=NULL;p=p->next){
				WRITE << p->x;
				WRITE << "\t";
				WRITE << p->y;
				WRITE << "\n";
			}
		}
		WRITE.close();
	}
}

