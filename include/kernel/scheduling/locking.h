#ifndef LOCKING_H
#define LOCKING_H

class Spinlock{
public:
    void Acquire();
    void Unlock();
private:
    volatile bool obtained = false;
};

#endif
