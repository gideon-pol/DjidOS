#ifndef IO_H
#define IO_H

#include <common.h>

class IO{
public:
    static uint32_t In32(uint16_t port){
        uint32_t value = 0;
        asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }

    static void Out32(uint16_t port, uint32_t value){ asm volatile("outl %0, %1" : : "a"(value), "Nd"(port)); }

    static uint16_t In16(uint16_t port){
        uint16_t value = 0;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }
    static void Out16(uint16_t port, uint16_t value){  asm volatile("outb %0, %1" : : "a"(value), "Nd"(port)); }

    
    static uint8_t In(uint16_t port){
        uint8_t value = 0;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }
    static void Out(uint16_t port, uint8_t value){  asm volatile("outb %0, %1" : : "a"(value), "Nd"(port)); }

    static void Wait(){ asm volatile("outb %%al, $0x80" : : "a"(0)); }

};

/*
class Port{
    Port(uint16_t port);
    void Write(uint16_t value);
    uint16_t Read();
private:
    uint16_t port;
}

class Port32{
public:
    Port(uint32_t port);
    void Write(uint32_t value);
    uint32_t Read();
private:
    uint32_t port;
}*/

#endif