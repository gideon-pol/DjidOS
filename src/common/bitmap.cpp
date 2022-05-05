#include <common/bitmap.h>

Bitmap::Bitmap(uint8_t* map, uint64_t count, uint64_t size){
    Map = map;
    Count = count;
    Size = size;
}

bool Bitmap::operator[](uint64_t index){
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    return Map[byteIndex] & (0b10000000 >> bitIndex);
}

void Bitmap::Set(uint64_t index, bool value){
    uint64_t byteIndex = index / 8;
    uint8_t bitIndex = index % 8;
    uint8_t mask = 0b10000000 >> bitIndex;

    Map[byteIndex] &= ~mask;
    if(value){
        Map[byteIndex] |= mask;
    }
}