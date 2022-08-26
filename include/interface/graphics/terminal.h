#ifndef TERMINAL_H
#define TERMINAL_H

#include <cstdarg>
#include <kernel/memory.h>
#include <interface/graphics/interface_graphics.h>

namespace Terminal{
    void Setup();
    void Println(char* s, ...);
    void Print(char* s, int column, int row, ...);
    void Print(char* s, int column, int row, va_list args);
    void Clear();

    void AddInputCharacter(char c);
    void RemoveInputCharacter();
}

#endif