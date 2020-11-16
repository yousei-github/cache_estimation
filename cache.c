#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// user interface
#define LINT_SIZE_B (4)
#define POINTER_SIZE_B (4)
#define NPAD (31)
#define BASIC_CYCLE (4)
#define MIN_KB (1)
#define MAX_MB (128)

// internel use
#define PRECISION (100)
#define ZOOM_OUT (4)
#define SCALE (1024)
#define KB (SCALE)
#define MB (SCALE * SCALE)
#define LIST_SIZE_B (POINTER_SIZE_B + NPAD * LINT_SIZE_B)
#define ARRAY_LENGTH (MAX_MB * MB / LIST_SIZE_B)
#define CALCULATION_MULTIPLE (1000000 * (NPAD + 1))
#define NANO_PER_SECOUND (1000000000)
//#define ARRAY_LENGTH (MIN_KB * KB / LIST_SIZE_B)

typedef unsigned int uint;
typedef unsigned long int ulong;

struct list
{
  struct list *next;
#if (NPAD)
  long int pad[NPAD];
#endif
};

struct list *list_initialization(struct list *head, int length);
struct list *list_set(struct list *head, int length, long int n);

int main()
{
  double spend_time;
  double spend_time_per_read;
  double min_spend_time_per_read;
  double min_spend_time;

  //uint cycle;
  float cycle;
  uint max_multiple;
  uint list_read_elements;

  clock_t start;
  clock_t stop;

  struct list *head = NULL;
  volatile struct list *read;

  printf("SCALE is [%d], ARRAY_LENGTH is [%d], LIST_SIZE_B is [%d]\n", SCALE, ARRAY_LENGTH, LIST_SIZE_B);
  printf("int is [%ld], long int is [%ld], pointer (struct list *) is [%ld], sizeof(struct list) is [%ld]\n",
         sizeof(int), sizeof(long int), sizeof(struct list *), sizeof(struct list));

  head = list_initialization(head, ARRAY_LENGTH);

  for (uint i = MIN_KB; i <= MAX_MB * SCALE; i = i * 2)
  {
    max_multiple = 1 + CALCULATION_MULTIPLE / i;
    list_read_elements = i * SCALE / LIST_SIZE_B;

    // here to keep the cache have same content before start each test
    head = list_set(head, ARRAY_LENGTH, i);

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
    //cycle = (uint)(spend_time_per_read * BASIC_CYCLE / min_spend_time_per_read);
    //cycle = (((spend_time_per_read * PRECISION / min_spend_time_per_read) - PRECISION) / ZOOM_OUT) + BASIC_CYCLE;
    cycle = (spend_time_per_read - min_spend_time_per_read) * ZOOM_OUT + BASIC_CYCLE;
    //cycle = (spend_time_per_read - min_spend_time_per_read) / 0.1 + BASIC_CYCLE;

    //printf("%dKB: spend_time is [%.8fs], cycle is [%d], spend_time_per_read is [%.3fns] \n", i, spend_time, cycle - BASIC_CYCLE, spend_time_per_read);
    printf("%dKB: spend_time is [%.8fs], cycle is [%f], spend_time_per_read is [%.3f]ns \n", i, spend_time, cycle, spend_time_per_read);
  }

  return 0;
}

struct list *list_initialization(struct list *head, int length)
{
  uint i;
  struct list *new;
  struct list *end;
  printf("Start list initialization... head is [0x%lx]\n", (ulong)head);

  new = (struct list *)malloc(sizeof(struct list));
  end = new;
  new->next = head;

#if (NPAD)
  if (NPAD)
    new->pad[0] = 5;
#endif

  head = new;

  for (i = 1; i < length; i++)
  {
    new = (struct list *)malloc(sizeof(struct list));
    new->next = head;

#if (NPAD)
    if (NPAD)
      new->pad[0] = 5;
#endif

    //printf("i = [%d],new [%x], new->next [%x]\n", i, (uint) new, (uint) new->next);
    head = new;
  }
  end->next = new;
  printf("Finish list initialization, for length [%d], new head is [0x%lx]\n", i, (ulong) new);

  return new;
}

struct list *list_set(struct list *head, int length, long int n)
{
  uint i;
  struct list *go = head;
  //printf("Start list set... head is [0x%x], n is [%ld]\n", (uint)head, n);

  for (i = 0; i < length; i++)
  {

#if (NPAD)
    if (NPAD)
      go->pad[0] = n;
#endif

    go = go->next;
  }
  //printf("Finish list set\n");
  return head;
}