#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <pthread.h>
#include <omp.h>

#include "const.h"
int generates(int total, unsigned int *seed)
{
  int count = 0;
  for (int i = 0; i < total; i++)
  {
    float x = rand_r(seed) / (double)RAND_MAX;
    float y = rand_r(seed) / (double)RAND_MAX;
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
  int totalCount[TOTAL_THREADS] = {0};
  unsigned int seed = omp_get_thread_num();

#pragma omp parallel num_threads(TOTAL_THREADS)
  {
    int *resTarget = res + omp_get_thread_num();
    int *totalTarget = totalCount + omp_get_thread_num();
    // int *resSnapshot = malloc(sizeof(int) * TOTAL_THREADS);
    // int *totalSnapshot = malloc(sizeof(int) * TOTAL_THREADS);
    while (fabs(pi - ACC_PI) > DELTA && *totalTarget < 2e8 / TOTAL_THREADS)
    {
      *resTarget += generates(BLOCK_SIZE, &seed);
      *totalTarget += BLOCK_SIZE;
      // memcpy(resSnapshot, res, sizeof(int) * TOTAL_THREADS);
      // memcpy(totalSnapshot, totalCount, sizeof(int) * TOTAL_THREADS);
      // int curInside = 0;
      // int curTotal = 0;
      // for (int i = 0; i < TOTAL_THREADS; i++)
      // {
      //   curInside += resSnapshot[i];
      //   curTotal += totalSnapshot[i];
      // }
      // curInside += inside;
      // curTotal += total;
      // total = curTotal;
      // inside = curInside;
      // pi = 4 * curInside / (double)curTotal;
    }
  }
}

int main()
{
  struct timeval t1, t2;
  gettimeofday(&t1, NULL);

  calc();

  gettimeofday(&t2, NULL);
  printf("pi=%.10f; inside=%d; outside=%d\n", pi, inside, total);
  double timeCost = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;
  printf("time cost: %f ms\n", timeCost);
  return 0;
}