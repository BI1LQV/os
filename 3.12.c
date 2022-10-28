#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
int main() //袁嘉昊 2019010070
{
  pid_t pid;
  pid = fork();
  if (pid != 0)
  { //父进程 睡觉
    sleep(100);
    wait(NULL);
  }
  return 0;
}