#include "header.h"

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: ./q1 <number of mallocs> <number of frees>\n");
    return 1;
  }
  int num_mallocs = atoi(argv[1]);
  int num_frees = atoi(argv[2]);
  int max = num_mallocs > num_frees ? num_mallocs : num_frees;
  if (num_mallocs < 10 || num_frees < 10)
  {
    printf("Number of mallocs and frees must be at least 10\n");
    return 1;
  }

  void *start, *end;
  void *ptrs[max];
  int space_allocated = 0;
  int space_freed = 0;
  for (int i = 0; i < num_mallocs; i++)
  {
    start = sbrk(0);
    ptrs[i] = malloc(sizeof(int));
    space_allocated += sizeof(int);
    end = sbrk(0);
    printf("Completed malloc %d, start address = %p, end address = %p\n", i + 1, start, end);
  }

  for (int i = 0; i < num_frees; i++)
  {
    if (i < num_mallocs)
    {
      struct block_meta *block_ptr = get_block_ptr(ptrs[i]);
      free(ptrs[i]);
      space_freed += block_ptr->size;
      printf("Completed free %d at address %p\n", i + 1, block_ptr);
    }
    else
    {
      printf("Nothing to free at index %d\n", i + 1);
    }
  }

  int leaked = (space_allocated - space_freed) > 0 ? (space_allocated - space_freed) : 0;
  printf("Memory leak: %d bytes\n", leaked);

  return 0;
}