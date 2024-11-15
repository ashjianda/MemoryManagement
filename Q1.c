#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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

struct block_meta
{
  size_t size;
  struct block_meta *next;
  int free;
  int magic;
};

#define META_SIZE sizeof(struct block_meta)

void *global_base = NULL;

struct block_meta *find_free_block(struct block_meta **last, size_t size)
{
  struct block_meta *current = global_base;
  while (current && !(current->free && current->size >= size))
  {
    *last = current;
    current = current->next;
  }
  return current;
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

int main()
{
  // print out the heap start/end addresses and print out the memory leaks for 10 malloc and free calls.
  void *start = sbrk(0);
  void *end = sbrk(0);
  void *ptr;
  for (int i = 0; i < 10; i++)
  {
    ptr = malloc(sizeof(int));
    start = sbrk(0);
    end = sbrk(0);
    printf("start address: %d, end address: %d, memory leaks: %d\n", start, end, end - start);
    free(ptr);
  }
  return 0;
}