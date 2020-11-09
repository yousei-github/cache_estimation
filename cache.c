#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// user interface
#define LINT_SIZE_B (4)
#define POINTER_SIZE_B (4)
#define NPAD (0)
#define BASIC_CYCLE (10)
#define MIN_KB (1)
#define MAX_MB (128)

// internel use
#define SCALE (1024)
#define KB (SCALE)
#define MB (SCALE * SCALE)
#define LIST_SIZE_B (POINTER_SIZE_B + NPAD * LINT_SIZE_B)
#define ARRAY_LENGTH (MAX_MB * MB / LIST_SIZE_B)
#define CALCULATION_MULTIPLE (5000 * (NPAD + 1))
#define NANO_PER_SECOUND (1000000000)
//#define ARRAY_LENGTH (MIN_KB * KB / LIST_SIZE_B)

typedef unsigned int uint;

struct list
{
  struct list *next;
  long int pad[NPAD];
};

struct list *list_initialization(struct list *head, int length)
{
  uint i;
  struct list *new;
  printf("Start list initialization... head is [0x%x]\n", (uint)head);
  for (i = 0; i < length; i++)
  {
    new = (struct list *)malloc(sizeof(struct list));
    new->next = head;
    new->pad[0] = 5;
    //printf("i = [%d],new [%x], new->next [%x]\n", i, (uint) new, (uint) new->next);
    head = new;
  }
  printf("Finish list initialization, for length [%d], new head is [0x%x]\n", i, (uint) new);

  return new;
}

int main()
{
  double spend_time;
  double spend_time_per_read;
  double min_spend_time_per_read;
  double min_spend_time;

  uint cycle;
  uint max_multiple;
  uint multiple_factor = 1;
  uint list_read_elements;

  clock_t start;
  clock_t stop;

  struct list end = {NULL, 2};
  struct list *head = &end;
  struct list *read;

  printf("SCALE is [%d], ARRAY_LENGTH is [%d], LIST_SIZE_B is [%d]\n", SCALE, ARRAY_LENGTH, LIST_SIZE_B);
  printf("int is [%ld], long int is [%ld], pointer (struct list *) is [%ld], sizeof(struct list) is [%d]\n",
         sizeof(int), sizeof(long int), sizeof(struct list *), sizeof(struct list));
  //printf("pointer is [%ld], sizeof(array) is [%ld]\n", sizeof(struct list *), sizeof(array));

  head = list_initialization(head, ARRAY_LENGTH);

  for (uint i = MIN_KB; i <= MAX_MB * SCALE; i = i * 2)
  {
    read = head;

    multiple_factor = 1;
    if (8 * SCALE <= i)
    {
      multiple_factor = CALCULATION_MULTIPLE / 2;
    }
    if (4 * SCALE <= i)
    {
      multiple_factor = CALCULATION_MULTIPLE / 4;
    }
    if (2 * SCALE <= i)
    {
      multiple_factor = CALCULATION_MULTIPLE / 8;
    }
    if (SCALE <= i)
    {
      multiple_factor = CALCULATION_MULTIPLE / 16;
    }
    max_multiple = CALCULATION_MULTIPLE / multiple_factor;
    list_read_elements = i * SCALE / LIST_SIZE_B;

    start = clock();
    for (uint time = 0; time < max_multiple; time++)
    {
      read = head;
      for (uint j = 0; j < list_read_elements; j++)
      {
        read = read->next;
      }
    }
    stop = clock();

    spend_time = ((double)(stop - start)) / CLOCKS_PER_SEC;
    spend_time_per_read = (spend_time * NANO_PER_SECOUND) / max_multiple / list_read_elements;
    if (i == MIN_KB)
    {
      min_spend_time = spend_time;
      min_spend_time_per_read = spend_time_per_read;
    }
    cycle = (uint)(spend_time_per_read * BASIC_CYCLE / min_spend_time_per_read);

    printf("%dB: spend_time is [%.8fs], cycle is [%d], spend_time_per_read is [%.3fns] \n", i, spend_time, cycle, spend_time_per_read);
  }

  return 0;
}
