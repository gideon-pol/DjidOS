#include <kernel/io.h>

namespace IO{
    uint32_t In32(uint16_t port){
        uint32_t value = 0;
        asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }

    void Out32(uint16_t port, uint32_t value){ asm volatile("outl %0, %1" : : "a"(value), "Nd"(port)); }

    uint16_t In16(uint16_t port){
        uint16_t value = 0;
        asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    }
    void Out16(uint16_t port, uint16_t value){  asm volatile("outw %0, %1" : : "a"(value), "Nd"(port)); }
    
    uint8_t In(uint16_t port){
        uint8_t value = 0;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
        return value;
    } 
    void Out(uint16_t port, uint8_t value){  asm volatile("outb %0, %1" : : "a"(value), "Nd"(port)); }
    
    void Wait(){ asm volatile("outb %%al, $0x80" : : "a"(0)); }
}