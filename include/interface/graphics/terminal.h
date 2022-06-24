#ifndef TERMINAL_H
#define TERMINAL_H

#include <cstdarg>
#include <interface/graphics/interface_graphics.h>

namespace Terminal{
    void Setup();
    void Println(char* s...);
    void Clear();

    void AddInputCharacter(char c);
    void RemoveInputCharacter();
}

#endif