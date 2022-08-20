#ifndef COMMON_H
#define COMMON_H

#include <cstddef>
#include <cstdint>

uint32_t strlen(char* str);
extern "C" void __cxa_pure_virtual();

void crash();
/*
double fmod(double x, double y);
double floor(double d);
double trunc(double d);
double abs(double d);*/

template<class T> T max(T a, T b){
    return (a < b) ? b : a;
}

template<class T> T min(T a, T b){
    return (a < b) ? a : b;
}

//inline void* operator new(size_t s) throw() { return nullptr; }
//inline void operator delete(void* p, unsigned long s) throw() {}

#endif