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

/**
 * 无锁多线程
 */
void calc()
{
  int res[TOTAL_THREADS] = {0};
  while (fabs(pi - ACC_PI) > DELTA && total < 2e8)
  {
#pragma omp parallel num_threads(TOTAL_THREADS)
    {
      res[omp_get_thread_num()] = generates(BLOCK_SIZE);
    }
    total += BLOCK_SIZE * TOTAL_THREADS;
    for (int i = 0; i < TOTAL_THREADS; i++)
    {
      inside += res[i];
    }
    pi = 4 * inside / (double)total;
  }
}

int main()
{
  struct timeval t1, t2;
  gettimeofday(&t1, NULL);

  srand(SEED);
  omp_set_max_active_levels(2);

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