#ifndef MOUSE_H
#define MOUSE_H

#include <common/common.h>
#include <drivers/driver.h>
#include <kernel/io.h>
#include <kernel/interrupt.h>
#include <interface.h>

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