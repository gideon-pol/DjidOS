#ifndef IO_H
#define IO_H

#include <common/common.h>

namespace IO{
    uint32_t In32(uint16_t port);
    void Out32(uint16_t port, uint32_t value);

    uint16_t In16(uint16_t port);
    void Out16(uint16_t port, uint16_t value);

    uint8_t In(uint16_t port);
    void Out(uint16_t port, uint8_t value);

    void Wait();
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