#include "BSTNode.h"
#include <iostream>
#include <fstream>

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

