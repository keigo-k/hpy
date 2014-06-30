#pragma once
#include <limits.h>

class Xorshift
{
private:
    unsigned long x;
    unsigned long y;
    unsigned long z;
    unsigned long w;
public:
    Xorshift(unsigned long x=123456789, unsigned long y=362436069, unsigned long z=521288629, unsigned long w=88675123) : x(x), y(y), z(z), w(w) {};
    ~Xorshift(void){};
    void seed(unsigned long _x=123456789, unsigned long _y=362436069, unsigned long _z=521288629, unsigned long _w=88675123);
    unsigned long xor128();
    float fxor128(); 
};

float xorshift(){
    static unsigned long sx=123456789;
    static unsigned long sy=362436069;
    static unsigned long sz=521288629;
    static unsigned long sw=88675123;
    unsigned long t;
    t=(sx^(sx<<11));sx=sy;sy=sz;sz=sw;
    return ((float)( sw=(sw^(sw>>19))^(t^(t>>8)) ))/(ULONG_MAX);
}

