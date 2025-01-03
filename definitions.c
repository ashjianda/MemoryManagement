#include "header.h"

void *nofree_malloc(size_t size)
{
    void *p = sbrk(0);
    void *request = sbrk(size);
    if (request == (void *)-1)
    {
        return NULL; // sbrk failed
    }
    else
    {
        assert(p == request);
        return p;
    }
}

void *global_base = NULL;
// first firt algo
// struct block_meta *find_free_block(struct block_meta **last, size_t size)
// {
//   struct block_meta *current = global_base;
//   while (current && !(current->free && current->size >= size))
//   {
//     *last = current;
//     current = current->next;
//   }
//   return current;
// }

// best fit
struct block_meta *find_free_block(struct block_meta **last, size_t size)
{
    struct block_meta *current = global_base;
    struct block_meta *best = NULL;
    while (current)
    {
        // if current is free and better than best
        if (current->free && current->size >= size)
        {
            // if first node or current is smaller than best
            if (!best || current->size < best->size)
            {
                best = current;
            }
        }
        current = current->next;
    }
    return best;
}

struct block_meta *request_space(struct block_meta *last, size_t size)
{
    struct block_meta *block;
    block = sbrk(0);
    void *request = sbrk(size + META_SIZE);
    assert((void *)block == request); // Not thread safe.
    if (request == (void *)-1)
    {
        return NULL; // sbrk failed.
    }

    if (last)
    { // NULL on first request.
        last->next = block;
    }
    block->size = size;
    block->next = NULL;
    block->free = 0;
    block->magic = 0x12345678;
    return block;
}

void *malloc(size_t size)
{
    struct block_meta *block;

    if (size <= 0)
    {
        return NULL;
    }

    if (!global_base)
    { // First call.
        block = request_space(NULL, size);
        if (!block)
        {
            return NULL;
        }
        global_base = block;
    }
    else
    {
        struct block_meta *last = global_base;
        block = find_free_block(&last, size);
        if (!block)
        { // Failed to find free block.
            block = request_space(last, size);
            if (!block)
            {
                return NULL;
            }
        }
        else
        { // Found free block
            block->free = 0;
            block->magic = 0x77777777;
        }
    }

    return (block + 1);
}

void *calloc(size_t nelem, size_t elsize)
{
    size_t size = nelem * elsize;
    void *ptr = malloc(size);
    memset(ptr, 0, size);
    return ptr;
}

struct block_meta *get_block_ptr(void *ptr)
{
    return (struct block_meta *)ptr - 1;
}

void free(void *ptr)
{
    if (!ptr)
    {
        return;
    }

    struct block_meta *block_ptr = get_block_ptr(ptr);
    assert(block_ptr->free == 0);
    assert(block_ptr->magic == 0x77777777 || block_ptr->magic == 0x12345678);
    block_ptr->free = 1;
    block_ptr->magic = 0x55555555;
}

void *realloc(void *ptr, size_t size)
{
    if (!ptr)
    {
        return malloc(size);
    }

    struct block_meta *block_ptr = get_block_ptr(ptr);
    if (block_ptr->size >= size)
    {
        return ptr;
    }

    void *new_ptr;
    new_ptr = malloc(size);
    if (!new_ptr)
    {
        return NULL;
    }
    memcpy(new_ptr, ptr, block_ptr->size);
    free(ptr);
    return new_ptr;
}