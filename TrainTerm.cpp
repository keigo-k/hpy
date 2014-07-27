#include "hashTerm.h"

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

bool *TrainTerm::add(float theta, float d, float th, float p){
    if(t==0){
        ++c;
        ++t;
        table.c=1;
        return true;
    }

    float denominator = c-d*t + (theta+d*th)*p;

    ++c;
    float rand = xorshift();
    Table *tp=&table;
    do{
        rand-=((float)tp->c - d)/denominator;
        if(rand<=0){
            tp->c++;        
            return false;
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
    return true;
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
            p=table.next;
            table.c=p->c;
            table.next=p->next;
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
