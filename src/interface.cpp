#include "interface.h"
#include <kernel/memory.h>

IChar* VID_MEM = (IChar*)(KERNEL_OFFSET + 0xb8000);

uint8_t print(uint8_t index, uint64_t number, uint8_t row){
    IChar d = IChar(number % 10 + 48);
    number /= 10;
    if(number != 0) index = print(index+1, number, row);
    else index = 0;
    *(VID_MEM + index + row * INTERFACE_COLUMNS) = d;
    return index + 1;
}