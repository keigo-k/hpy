#include "xorshift.h"

void Xorshift::seed(unsigned long _x, unsigned long _y, unsigned long _z, unsigned long _w) {
    x=_x;
    y=_y;
    z=_z;
    w=_w;
}

unsigned long Xorshift::xor128(){ 
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return( w=(w^(w>>19))^(t^(t>>8)) ); 
}

float Xorshift::fxor128(){
    unsigned long t;
    t=(x^(x<<11));x=y;y=z;z=w; return ((float)( w=(w^(w>>19))^(t^(t>>8)) ))/(ULONG_MAX); 
}

