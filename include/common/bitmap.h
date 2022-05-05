#ifndef BITMAP_H
#define BITMAP_H

#include <common.h>

class Bitmap{
public:
    uint64_t Count;
    uint8_t* Map;
    uint64_t Size;

    Bitmap(){
        Count = 0;
        Map = nullptr;
        Size = 0;
    }
    Bitmap(uint8_t* map, uint64_t count, uint64_t size);
    bool operator[](uint64_t index);
    void Set(uint64_t index, bool value);
};

#endif