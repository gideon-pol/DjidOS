#ifndef ALLOC_H
#define ALLOC_H

#include <common/common.h>
#include <kernel/memory/pmm.h>
#include <kernel/memory/vmm.h>
#include <interface/graphics/interface_graphics.h>

void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

#endif
