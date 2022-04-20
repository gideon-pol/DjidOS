#ifndef MOUSE_H
#define MOUSE_H

#include <common.h>
#include <drivers/driver.h>

/*
class Driver{
public:
    virtual ~Driver(){};
    virtual void Initialize() = 0;
    virtual void HandleInterrupt() = 0;
};*/

enum MouseButton { Left, Right, Middle, Four, Five };

class MouseDriver// : public Driver 
{
public:
    MouseDriver(){};
    void Initialize();
    void HandleInterrupt();
private:
    uint8_t packetNumber = 0;
};

extern MouseDriver mouseDriver;

#endif