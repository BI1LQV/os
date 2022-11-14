#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <omp.h>

#include "const.h"
int generates(int total)
{
  int count = 0;
  for (int i = 0; i < total; i++)
  {
    float x = rand() / (double)RAND_MAX;
    float y = rand() / (double)RAND_MAX;
    count += x * x + y * y < 1;
  }
  return count;
}

int inside = 0;
int total = 0;
double pi = 0;

pthread_mutex_t mutexLock;
/**
 * mutex多线程
 */
void calc()
{
  while (fabs(pi - ACC_PI) > DELTA && total < MAX_ITER)
  {
    int res = generates(BLOCK_SIZE);
    pthread_mutex_lock(&mutexLock);
    inside += res;
    total += BLOCK_SIZE;
    pi = 4 * inside / (double)total;
    pthread_mutex_unlock(&mutexLock);
  }
}

int main()
{
  struct timeval t1, t2;
  gettimeofday(&t1, NULL);

  pthread_mutex_init(&mutexLock, NULL);

  srand(SEED);

#pragma omp parallel num_threads(TOTAL_THREADS)
  {
    calc();
  }

  gettimeofday(&t2, NULL);
  printf("pi=%.10f; inside=%d; outside=%d\n", pi, inside, total);
  double timeCost = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;
  printf("time cost: %f ms\n", timeCost);
  return 0;
}