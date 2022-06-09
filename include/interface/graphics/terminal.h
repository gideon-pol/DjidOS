#ifndef TERMINAL_H
#define TERMINAL_H

#include <cstdarg>
#include <interface/graphics/interface_graphics.h>

namespace Terminal{
    void Setup();
    void PrintLine(char* s...);

}

#endif