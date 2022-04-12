#pragma once

class Driver{
public:
    virtual ~Driver(){};
    virtual void Initialize() = 0;
    virtual void HandleInterrupt() = 0;
};