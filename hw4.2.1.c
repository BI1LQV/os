#include <stdio.h>
#include <omp.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>
#define THREAD_NUM 7
struct lockNode
{
  int *level;
  int *waiting;
  int maxLevel;
};
struct lockNode *root;
struct lockNode *create_lock(int size)
{
  struct lockNode *locker = malloc(sizeof(struct lockNode) * size);
  locker->level = (int *)malloc(sizeof(int) * size);
  locker->waiting = (int *)malloc(sizeof(int) * (size - 1));
  locker->maxLevel = size;
  return locker;
}

void enter_lock(struct lockNode *lock, int thread_id)
{
  for (int lev = 0; lev < lock->maxLevel - 1; ++lev)
  {
    lock->level[thread_id] = lev;
    lock->waiting[lev] = thread_id;
    while (lock->waiting[lev] == thread_id)
    {
      int k = 0;
      for (; k < lock->maxLevel; k++)
      {
        if (lock->level[k] >= lev && k != thread_id)
        {
          break;
        }
        if (lock->waiting[lev] != thread_id)
        {
          break;
        }
      }
      if (k == lock->maxLevel)
      {
        break;
      }
    }
  }
}

void exit_lock(struct lockNode *lock, int thread_id)
{
  lock->level[thread_id] = -1;
}

int i = 0;
int counter[THREAD_NUM] = {0};
int swap = 0;
int swapCount = 0;

void spinning(int id)
{
  while (1)
  {
    enter_lock(root, id);
    if (i >= 1000000)
    {
      break;
    }
    i++;
    counter[id]++;
    if (swap != id)
    {
      swap = id;
      swapCount++;
    }
    exit_lock(root, id);
  }
  exit_lock(root, id);
}

int main()
{
  struct timeval startTime;
  struct timeval endTime;
  gettimeofday(&startTime, NULL);
  root = create_lock(THREAD_NUM);
#pragma omp parallel num_threads(THREAD_NUM)
  {
    spinning(omp_get_thread_num());
  }
  gettimeofday(&endTime, NULL);
  int sum = 0;
  for (int i = 0; i < THREAD_NUM; i++)
  {
    sum += counter[i];
  }
  printf("%d\n", sum);
  printf("spent %ld ms\n",
         (endTime.tv_sec - startTime.tv_sec) * 1000 +
             (endTime.tv_usec - startTime.tv_usec) / 1000);
  return 0;
}