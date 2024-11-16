#ifndef HEADER_H
#define HEADER_H

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define META_SIZE sizeof(struct block_meta)

// Definition of struct block_meta
struct block_meta
{
    size_t size;
    struct block_meta *next;
    int free;
    int magic;
};

// Function declarations
void *nofree_malloc(size_t size);
void *calloc(size_t nelem, size_t elsize);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
struct block_meta *request_space(struct block_meta *last, size_t size);
struct block_meta *find_free_block(struct block_meta **last, size_t size);
struct block_meta *get_block_ptr(void *ptr);

#endif // HEADER_H
