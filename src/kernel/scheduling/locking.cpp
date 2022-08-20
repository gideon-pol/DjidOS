#include <kernel/scheduling/locking.h>

void Spinlock::Acquire(){
    while(!__sync_bool_compare_and_swap(&obtained, false, true)){
		asm("pause");
	}
}

void Spinlock::Unlock(){
    obtained = false;
}