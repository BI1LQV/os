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

#pragma omp parallel num_threads(TOTAL_THREADS + 1)
  {
    int threadId = omp_get_thread_num();
    unsigned int seed = threadId;

    int *resTarget = res + threadId;
    int *totalTarget = totalCount + threadId;

    while (fabs(pi - ACC_PI) > DELTA && *totalTarget < MAX_ITER / TOTAL_THREADS)
    {
      if (TOTAL_THREADS == threadId) //最后一个线程
      {
        int _inside = 0;
        int _total = 0;
        for (int i = 0; i < TOTAL_THREADS; i++)
        {
          _inside += res[i];
          _total += totalCount[i];
        }
        inside = _inside;
        total = _total;
        pi = total > 0 ? 4 * _inside / (double)_total : 0;
      }
      else
      {
        *resTarget += generates(BLOCK_SIZE, &seed);
        seed += rand_r(&seed) / (double)RAND_MAX * 100;
        *totalTarget += BLOCK_SIZE;
      }
    }
  }
}

int main()
{
  struct timeval t1, t2;
  gettimeofday(&t1, NULL);
  calc();

  gettimeofday(&t2, NULL);
  printf("pi=%.10f; inside=%d; total=%d\n", pi, inside, total);
  double timeCost = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;
  printf("time cost: %f ms\n", timeCost);
  return 0;
}