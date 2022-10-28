#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
int main() //袁嘉昊 2019010070
{
  int start = -1;
  printf("输入起始值:");
  scanf("%d", &start);
  pid_t pid;
  pid = fork();
  if (pid == 0)
  { //子进程
    while (start != 1)
    {
      if (start % 2 == 0)
      {
        start /= 2;
      }
      else
      {
        start = start * 3 + 1;
      }
      printf("%d\n", start);
    }
  }
  else
  {
    if (wait(NULL) == -1)
    {
      printf("子进程结束异常\n");
    }
    else
    {
      printf("子进程正常结束\n");
    }
  }
  return 0;
}
