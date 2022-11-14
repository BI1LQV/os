#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <time.h>
#include <sys/time.h>

#define TOTAL_THREADS 8
#define TOTAL_POINTS (int)2e8

int generates(int total)
{
  int inside = 0;
  unsigned int seed = omp_get_thread_num();
  for (int i = 0; i < total; i++)
  {
    float x = rand_r(&seed) / (double)RAND_MAX;
    float y = rand_r(&seed) / (double)RAND_MAX;
    if (x * x + y * y < 1)
    {
      inside++;
    }
  }
  return inside;
}

int main()
{
  struct timeval t1, t2;
  gettimeofday(&t1, NULL);
  int inside[TOTAL_THREADS] = {0};

#pragma omp parallel num_threads(TOTAL_THREADS)
  {
    inside[omp_get_thread_num()] = generates(TOTAL_POINTS / TOTAL_THREADS);
  }

  int sum = 0;
  for (int i = 0; i < TOTAL_THREADS; i++)
  {
    sum += inside[i];
  }
  gettimeofday(&t2, NULL);
  printf("pi=%.10f; inside=%d; outside=%d\n", 4 * sum / (double)TOTAL_POINTS, sum, TOTAL_POINTS);
  double timeCost = (t2.tv_sec - t1.tv_sec) * 1000.0 + (t2.tv_usec - t1.tv_usec) / 1000.0;
  printf("time cost: %f ms\n", timeCost);
  return 0;
}