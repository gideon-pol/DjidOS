#include <common/common.h>

void crash(){
    asm("int $0x1");
}

/*
double fmod(double x, double y) {
  return x - trunc(x / y) * y;
}

double floor(double d){
    return d - (d % 1);
}

double ceil(double d){
    return (1 - d) + (d % 1);
}

double trunc(double d){ 
    return (d>0) ? floor(d) : ceil(d);
}

double abs(double d){
    if(d < 0) return -d;
    return d;
}*/

uint32_t strlen(char* str){
    uint32_t c = 0;
    while(str[c] != 0){
        c++;
    }
    return c;
}


void __cxa_pure_virtual(){

}