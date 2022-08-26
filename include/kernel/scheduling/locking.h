#ifndef LOCKING_H
#define LOCKING_H

#include <common/common.h>
#include <interface/graphics/interface_graphics.h>

class Spinlock{
public:
    void Acquire();
    void Unlock();
private:
    volatile bool obtained = false;
};

#endif
