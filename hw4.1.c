#include <stdio.h>
#include <omp.h>
#include <sys/time.h>
#include <pthread.h>
pthread_mutex_t mutexLock;
// #define SYSLOCK 0
int turn;
int flag[2] = {0};
void enter_lock(int thread_id)
{
  flag[thread_id] = 1;
  turn = thread_id;
  while (turn == thread_id && (flag[1 - thread_id] == 1))
    ;
}
void exit_lock(int thread_id)
{
  flag[thread_id] = 0;
}

int i = 0;
int counter[2] = {0};
int swap = 0;
int swapCount = 0;
void up(int id)
{
#ifdef SYSLOCK
  enter_lock(id);
#else
  pthread_mutex_lock(&mutexLock);
#endif
}
void down(int id)
{
#ifdef SYSLOCK
  exit_lock(id);
#else
  pthread_mutex_unlock(&mutexLock);
#endif
}
void spinning(int id)
{
  while (1)
  {
    up(id);
    if (i >= 10000000)
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
    down(id);
  }
  down(id);
}
int main()
{
  struct timeval startTime;
  struct timeval endTime;
  pthread_mutex_init(&mutexLock, NULL);
  gettimeofday(&startTime, NULL);
#pragma omp parallel num_threads(2)
  {
    spinning(omp_get_thread_num());
  }
  gettimeofday(&endTime, NULL);
  printf("%d,%d,%d\n", counter[0], counter[1], swapCount);
  printf("spent %ld ms\n",
         (endTime.tv_sec - startTime.tv_sec) * 1000 +
             (endTime.tv_usec - startTime.tv_usec) / 1000);
  return 0;
}