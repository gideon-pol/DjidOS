#ifndef MOUSE_H
#define MOUSE_H

#include <common/common.h>
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
};

extern MouseDriver mouseDriver;

#endif