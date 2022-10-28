#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#define RAND_N 100000
#define TOTAL_POINT 1000000
#define TOTAL_THREADS 8
float genRand()
{
  return ((rand() % RAND_N) / (float)RAND_N) * 2 - 1;
  //我相信这里编译器会优化的（笑
}

int generates(int total)
{
  int inside = 0;
  for (int i = 0; i < total; i++)
  {
    float x = genRand();
    float y = genRand();
    if (x * x + y * y < 1)
    {
      inside++;
    }
  }
  return inside;
}

int main()
{
  int inside[TOTAL_THREADS] = {0};
#pragma omp parallel num_threads(TOTAL_THREADS)
  {
    inside[omp_get_thread_num()] = generates(TOTAL_POINT / TOTAL_THREADS);
  }
  int sum = 0;
  for (int i = 0; i < TOTAL_THREADS; i++)
  {
    sum += inside[i];
  }
  printf("%f\n", 4 * sum / (float)TOTAL_POINT);
  return 0;
}