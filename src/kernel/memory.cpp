#include <kernel/memory.h>
#include <common/common.h>
#include <stdio.h>
#include <unistd.h>
#include <interface.h>

typedef struct free_block free_block;

struct free_block {
    size_t size;
    free_block* next;
};

typedef struct {
    size_t size;
} used_block;

free_block* freeList = NULL;
int freesSinceMerge = 0;

#define ASSERT(x) { if(!(x)){ Terminal::Println(">>> Assertion \"%s\" failed in %s, line %d\n", #x, __PRETTY_FUNCTION__, __LINE__); while(true); } }
#define ALIGN(num, bound) ((num + bound-1) & ~(bound-1))
#define BATCH_SIZE PAGE_SIZE
#define OVERHEAD sizeof(used_block)

free_block* getFreeBlock(size_t s, free_block** prevBlock){
    free_block* cur = freeList;
    *prevBlock = NULL;
    while(cur != NULL && cur->size < s){
        *prevBlock = cur;
        cur = cur->next;
    }

    return cur;
}

free_block* getLastBlock(){
    free_block* cur = freeList;
    while(cur != NULL && cur->next != NULL){
        cur = cur->next;
    }

    return cur;
}

bool isConnectedToSbrk(free_block* block){
    return (uintptr_t)block + block->size + OVERHEAD == (uintptr_t)VMM::SBRK(0);
}

free_block* getPreviousLinkedBlock(free_block* block){
    free_block* cur = freeList;
    free_block* prev = NULL;

    while(cur != NULL && cur != block){
        prev = cur;
        cur = cur->next;
    }

    if(cur == block) return prev;
    else return NULL;
}

void splitBlock(free_block* freeBlock, size_t s, free_block* prevBlock){
    if(freeBlock->size >= (s + OVERHEAD * 2)){
        free_block* newBlock = (free_block*)((uint64_t)freeBlock + s + OVERHEAD);
        ASSERT((int64_t)freeBlock->size - s - OVERHEAD > 0);
        newBlock->size = freeBlock->size - s - OVERHEAD;
        freeBlock->size = s;

        newBlock->next = freeBlock->next;
        freeBlock->next = newBlock;

        ASSERT((int64_t)newBlock->size > 0);
        ASSERT((int64_t)freeBlock->size > 0);
        ASSERT(((uintptr_t)newBlock - (uintptr_t)freeBlock) == s + OVERHEAD);
    }

    if(prevBlock != NULL){
        prevBlock->next = freeBlock->next;
    } else {
        freeList = freeBlock->next;
    }
}

void *malloc(size_t size){
    if(size == 0) size = 1;
    size = ALIGN(size, 8);

    free_block* prevBlock = NULL;
    free_block* freeBlock = getFreeBlock(size, &prevBlock);

    if(freeBlock == NULL){
        if(prevBlock == NULL || !isConnectedToSbrk(prevBlock)){
            size_t batchSize = ALIGN(size + OVERHEAD, BATCH_SIZE);
            if((intptr_t)(freeBlock = (free_block*)VMM::SBRK(batchSize)) == -1) return NULL;

            if(prevBlock == NULL){
                freeList = freeBlock;
            } else {
                prevBlock->next = freeBlock;
            }

            freeBlock->size = batchSize - OVERHEAD;
            freeBlock->next = NULL;
        } else {
            ASSERT(prevBlock->next == NULL);
            size_t batchSize = ALIGN(size - prevBlock->size, BATCH_SIZE);
            freeBlock = prevBlock;
            freeBlock->size += batchSize;
            ASSERT((intptr_t)VMM::SBRK(batchSize) != -1);
            if((intptr_t)VMM::SBRK(batchSize) == -1) return NULL;

            prevBlock = getPreviousLinkedBlock(freeBlock);
        }
    }


    
    splitBlock(freeBlock, size, prevBlock);
    used_block* used = (used_block*)freeBlock;

    ASSERT(VMM::SBRK(0) >= (uint8_t*)used + OVERHEAD + size);
    
    return (void*)used + OVERHEAD;
}


void* calloc(size_t nmemb, size_t size){
    //return memset(malloc(nmemb * size), 0, nmemb * size);
}

void merge(){
    free_block* cur = freeList;

    while(cur) {
        while(cur->next == (free_block*)((uintptr_t)cur + cur->size + OVERHEAD)){
            cur->size += cur->next->size + OVERHEAD;
            //ASSERT((uint8_t*)cur + cur->size + OVERHEAD == (uint8_t*)connectingBlock + connectingBlock->size + OVERHEAD);
            cur->next = cur->next->next;

            //if(isConnectedToSbrk(connectingBlock)) ASSERT(isConnectedToSbrk(cur));
        }

        cur = cur->next;
    }
}

void free(void* ptr){
    if(ptr == NULL) return;

    ASSERT(ptr < VMM::SBRK(0));

    free_block* block = (free_block*)(ptr - OVERHEAD);
    ASSERT((int64_t)block->size > 0);

    free_block* cur = freeList;

    while(cur != NULL && cur->next != NULL && cur->next < block){
        cur = cur->next;
    }

    if(cur == NULL || cur > block){
        block->next = freeList;
        freeList = block;
    } else {
        block->next = cur->next;
        cur->next = block;
    }

    ASSERT(block->next == NULL || block->next > block);
    freesSinceMerge++;

    if(freesSinceMerge >= 10){
        merge();
        
        free_block* lastFreeBlock = getLastBlock();
        
        if(lastFreeBlock && isConnectedToSbrk(lastFreeBlock) && lastFreeBlock->size > BATCH_SIZE){
            ASSERT((uint8_t*)lastFreeBlock + lastFreeBlock->size + OVERHEAD == VMM::SBRK(0));
            /* IMPORTANT TODO:
                change this sbrk call to move down by entire pages as that's the only way SBRK can map
                */
            VMM::SBRK(-(ALIGN_DOWN(lastFreeBlock->size, BATCH_SIZE)));
            lastFreeBlock->size = BATCH_SIZE;
            ASSERT(isConnectedToSbrk(lastFreeBlock));
        }

        freesSinceMerge = 0;
    }
}

void* realloc(void *ptr, size_t size){

    if(ptr == NULL) return malloc(size);
    if(size == 0) free(ptr);

    void* freeMem = malloc(size);
    //memcpy(freeMem, ptr, size);
    free(ptr);

    return freeMem;
}