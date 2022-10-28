#include <stdio.h>
#include <stdlib.h>
#define MIN_PID 300
#define MAX_PID 5000

char *PID_POOL;
enum BIT_STATUS
{
  FREE = 0,
  ALLOCATED = 1
};

void putBit(char *map, int index, enum BIT_STATUS val)
{
  if (val == ALLOCATED)
  {
    map[index / 8] |= (1 << (index % 8));
  }
  else
  {
    map[index / 8] &= ~(1 << (index % 8));
  }
}

int getBit(char *map, int index)
{
  return map[index / 8] & (1 << (index % 8));
}

int allocate_map(void)
{
  int size = MAX_PID - MIN_PID + 1;
  PID_POOL = (char *)malloc(size);
  if (PID_POOL == NULL) //分配失败
  {
    return -1;
  }
  return 1;
}
int allocate_pid(void)
{
  for (int i = MIN_PID; i <= MAX_PID; i++)
  {
    if (!getBit(PID_POOL, i - MIN_PID))
    {
      putBit(PID_POOL, i - MIN_PID, ALLOCATED);
      return i;
    }
  }
  return -1;
}

void release_pid(int pid)
{
  putBit(PID_POOL, pid - MIN_PID, FREE);
}

int main() //袁嘉昊 2019010070
{
  allocate_map();
  int pid = allocate_pid();
  if (pid == -1)
  {
    printf("无可用pid\n");
  }
  else
  {
    printf("分配pid为%d\n", pid);
    release_pid(pid);
  }
  for (int i = 0; i < 10000; i++)
  {
    allocate_pid(); //耗尽pid
  }
  printf("此处应该为-1：%d\n", allocate_pid());
  return 0;
}
