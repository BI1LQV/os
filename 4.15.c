#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int allocate_map(void);
int allocate_pid(void);
void release_pid(int pid);
int main()
{
  allocate_map();
#pragma omp parallel num_threads(100)
  {
    int pid = allocate_pid();
    printf("get pid=%d\n", pid);
    sleep(2 + (rand() / 100000000.0));
    release_pid(pid);
    printf("release pid=%d\n", pid);
  }
  return 0;
}