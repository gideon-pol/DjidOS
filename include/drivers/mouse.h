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

class MouseDriver// : public Driver 
{
public:
    MouseDriver(){};
     void Initialize();
    void HandleInterrupt();
};

extern MouseDriver mouseDriver;

#endif