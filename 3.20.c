#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4096
int main(int argc, char *argv[]) //袁嘉昊 2019010070
{
  int piper[2];
  pipe(piper);
  int pipeWriter, pipeReader;
  pipeReader = piper[0];
  pipeWriter = piper[1];
  char *from = argv[1];
  char *to = argv[2];

  pid_t pid = fork();

  if (pid == 0) //子线程写文件
  {
    int writeHandler = open(to, O_WRONLY);
    char buffer[BUFFER_SIZE] = "";
    read(pipeReader, buffer, BUFFER_SIZE);
    close(pipeReader);
    write(writeHandler, buffer, BUFFER_SIZE);
    close(writeHandler);
  }
  else
  { //父线程发文件
    int readHandler = open(from, O_RDONLY);
    char buffer[BUFFER_SIZE] = "";
    read(readHandler, buffer, BUFFER_SIZE);
    close(readHandler);
    write(pipeWriter, buffer, BUFFER_SIZE);
    close(pipeWriter);
    wait(NULL);
  }
  return 0;
}