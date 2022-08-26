#include <kernel/memory.h>

void memset(void* dst, int8_t val, size_t count){
    //int c = ALIGN_DOWN(count, sizeof(int));

    for(int i = 0; i < count; i++){
        *((int8_t*)dst + i) = val;
    }

    /*
    for(int i = c; i < count; i++){
        *((uint8_t*)dst + i) = (val >> ((i-c)*8)) & 0xFF;
    }*/
}

void* memcpy(void* dst, void* src, size_t count){
    int c = ALIGN_DOWN(count, sizeof(uint64_t));

    for(int i = 0; i < c / sizeof(uint64_t); i++){
        *((uint64_t*)dst + i) = *((uint64_t*)src + i);
    }

    for(int i = c; i < count; i++){
        *((uint8_t*)dst + i) = *((uint8_t*)src + i);
    }
}