#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <pwd.h>
#define MAX_LINE 80

char getLast(char *word)
//函数判断单词是否以&结尾，如果以&结尾就返回&的index
//如果是正常单词就返回-1
{
  int wordPtr = 0;
  while (word[wordPtr] != '\0')
  {
    wordPtr++;
  }
  if (word[wordPtr - 1] == '&')
  {
    return wordPtr - 1;
  }
  return -1; //正常单词
}

char **history[20] = {0}; //懒得自己维护lru了 每次满了往前腾10个空就行
int historyPtr = 0;
int count = 0;
int cleanCount = 0;
int main()
{
  char **args = (char **)malloc(sizeof(char *) * MAX_LINE);
  int bkPtr = 0;
  int should_run = 1;
  while (should_run)
  {
    printf("osh>");
    fflush(stdout);
    //以上是书本提供的代码模板
    int argsPtr = 0;      //没push方法 只能自己手动维护指针了
    char c;               //用来吃空格
    int isBackground = 0; //判断是否后台运行
    do
    {
      char *word = (char *)malloc(sizeof(char) * MAX_LINE); //每个单词的大小 共用maxline好了
      scanf("%s", word);                                    //读入一个词
      c = getchar();                                        //吃一个空格
      args[argsPtr++] = word;
      if (strcmp(word, "exit") == 0) // exit退出
      {
        should_run = 0;
      }
      else if (strcmp(word, "history") == 0)
      {
        for (int p = historyPtr - 1; p >= historyPtr - 10 && p >= 0; p--)
        {
          printf("[%d]", count - (historyPtr - p)); //打印序号
          int h = 0;
          while (history[p][h])
          {
            printf("%s ", history[p][h]); //打印每个参数
            h++;
          }
          printf("\n");
        }
      }
      else if (strcmp(word, "!!") == 0) //执行最近的历史记录
      {
        if (historyPtr == 0) //没历史记录 不能越界 直接抛错
        {
          printf("ERROR NO HISTORY!\n");
        }
        else
        {
          memcpy(args, history[historyPtr - 1], sizeof(char *) * MAX_LINE); //复制一份 最近的历史记录来执行
        }
      }
      else if (word[0] == '!')
      {
        word[0] = '0';
        int targetIdx = atoi(word);
        targetIdx = targetIdx - cleanCount * 10;
        if (targetIdx < 0 || targetIdx > historyPtr) //越界 抛错
        {
          printf("ERROR NO HISTORY!\n");
        }
        memcpy(args, history[targetIdx], sizeof(char *) * MAX_LINE); //也是复制来执行
      }
      else
      {
        int endAt = getLast(word);
        if (endAt != -1)
        {
          //结尾是&，去掉&
          //如果输入的是 xxx & 那args是["xxx",""]应该也是正确的
          word[endAt] = '\0';
          isBackground = 1;
        }
      }

    } while (c == ' ');

    args[argsPtr] = NULL; //给个结束符

    pid_t id = fork();
    if (id < 0)
    {
      printf("ERROR OCCURRED\n"); //出错
    }
    if (id == 0) //子进程中，执行命令
    {
      execvp(args[0], args); //书本给的
      exit(0);               //正常退出即可
    }
    else
    { //父进程
      if (isBackground)
      {
        printf("child running at pid %d\n", id); //提示后台执行中，不阻塞主线程
      }
      else
      {
        int status = 0;
        waitpid(id, &status, 0); //阻塞主线程
      }
    }
    char **bkArgs = (char **)malloc(sizeof(char *) * MAX_LINE); //拷贝一份 args会被覆盖的
    memcpy(bkArgs, args, sizeof(char *) * MAX_LINE);
    history[historyPtr++] = bkArgs;
    if (historyPtr == 20)
    { // history满了
      for (int p = 0; p < 10; p++)
      { //包括字符串的数组 和 字符串本身都释放掉
        int i = 0;
        while (history[p][i])
        {
          free(history[p][i]);
          i++;
        }
        free(history[p]);
      }
      for (int p = 0; p < 10; p++)
      { //把后面10个腾回到前面去
        history[p] = history[p + 10];
      }
      historyPtr = 10;
      cleanCount++;
    }
    count++;
  }
  return 0;
}
