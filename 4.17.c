#include <stdlib.h>
#include <stdio.h>

#define RAND_N 100000
const int TOTAL_POINT = 1000000;
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
  int inside = 0;
#pragma omp parallel
  {
    inside = generates(TOTAL_POINT);
  }
  printf("%f\n", 4 * inside / (float)TOTAL_POINT);
  return 0;
}