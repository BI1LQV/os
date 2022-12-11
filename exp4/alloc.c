#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum boolean
{
  true = 1,
  false = 0
} boolean;

typedef struct PageLog
{
  int *pageList;
  boolean isPageFault;
} PageLog;

int REQUEST_NUM = 0;
int PAGE_NUM = 0;

int *REQUEST_TARGET_LIST;

PageLog *pageLogList;

void init()
{
  float b;

  // init REQUEST_NUM,PAGE_NUM
  printf("请输入请求数量：");
  scanf("%d", &REQUEST_NUM);
  printf("请输入页面帧数量(1-7):");
  scanf("%d", &PAGE_NUM);
  // init REQUEST_TARGET_LIST
  REQUEST_TARGET_LIST = malloc(sizeof(int) * REQUEST_NUM);
  for (int i = 0; i < REQUEST_NUM; i++)
  {
    *(REQUEST_TARGET_LIST + i) = rand() % 10;
  }
  // init pageLogList
  pageLogList = malloc(sizeof(PageLog) * REQUEST_NUM);
}

typedef struct FifoNode
{
  int page;
  struct FifoNode *next;
} FifoNode;

void FIFO()
{
  // init cycle link list
  FifoNode *curNode = malloc(sizeof(FifoNode));
  *curNode = (FifoNode){0, NULL};
  FifoNode *firstNode = curNode;
  for (int i = 1; i < PAGE_NUM; i++)
  {
    FifoNode *newNode = malloc(sizeof(FifoNode));
    *newNode = (FifoNode){i, curNode};
    curNode = newNode;
  }
  firstNode->next = curNode;

  for (int i = 0; i < REQUEST_NUM; i++)
  {
    printf("%d\n", REQUEST_TARGET_LIST[i]);
  }
}

int main()
{
  init();
  FIFO();
  return 0;
}

// void FIFO() // FIFO算法
// {
//   init();
//   int i, j = 1, n = 20, k, f, m;
//   stack[0][0] = numbers[0];

//   for (i = 1; i < 20; i++)
//   {
//     f = 0;
//     for (m = 0; m < nums; m++)
//     {
//       stack[i][m] = stack[i - 1][m];
//     }
//     for (k = 0; k < nums; k++)
//     {
//       if (stack[i][k] == numbers[i])
//       {
//         n--;
//         f = 1;
//         break;
//       }
//     }
//     if (f == 0)
//     {
//       stack[i][j] = numbers[i];
//       j++;
//     }
//     if (j == nums)
//       j = 0;
//   }
//   printf("\n");
//   printf("FIFO算法：\n");
//   print();
//   printf("缺页错误数目为：%d\n", n);
// }

// void LRU() // LRU算法
// {
//   int i, j, m, k, sum = 1;
//   int sequence[7] = {0}; // 记录序列
//   init();
//   stack[0][0] = numbers[0];
//   sequence[0] = nums - 1;
//   for (i = 1; i < nums; i++) // 前半部分，页面空置的情况
//   {
//     for (j = 0; j < nums; j++)
//     {
//       stack[i][j] = stack[i - 1][j];
//     }
//     for (j = 0; j < nums; j++)
//     {
//       if (sequence[j] == 0)
//       {
//         stack[i][j] = numbers[i];
//         break;
//       }
//     }
//     for (j = 0; j < i; j++) // 将之前的优先级序列都减1
//     {
//       sequence[j]--;
//     }
//     sequence[i] = nums - 1; // 最近使用的优先级列为最高
//     sum++;
//   }
//   for (i = nums; i < 20; i++) // 页面不空，需要替换的情况
//   {
//     int f;
//     f = 0;
//     for (j = 0; j < nums; j++)
//     {
//       stack[i][j] = stack[i - 1][j];
//     }
//     for (j = 0; j < nums; j++) // 判断输入串中的数字，是否已经在栈中
//     {
//       if (stack[i][j] == numbers[i])
//       {
//         f = 1;
//         k = j;
//         break;
//       }
//     }
//     if (f == 0) // 如果页面栈中没有，不相同
//     {
//       for (j = 0; j < nums; j++) // 找优先序列中为0的
//       {
//         if (sequence[j] == 0)
//         {
//           m = j;
//           break;
//         }
//       }
//       for (j = 0; j < nums; j++)
//       {
//         sequence[j]--;
//       }
//       sequence[m] = nums - 1;
//       stack[i][m] = numbers[i];
//       sum++;
//     }
//     else // 如果页面栈中有，替换优先级
//     {
//       if (sequence[k] == 0) // 优先级为最小优先序列的
//       {
//         for (j = 0; j < nums; j++)
//         {
//           sequence[j]--;
//         }
//         sequence[k] = nums - 1;
//       }
//       else if (sequence[k] == nums - 1) // 优先级为最大优先序列的
//       {
//         // 无需操作
//       }
//       else // 优先级为中间优先序列的
//       {
//         for (j = 0; j < nums; j++)
//         {
//           if (sequence[k] < sequence[j])
//           {
//             sequence[j]--;
//           }
//         }
//         sequence[k] = nums - 1;
//       }
//     }
//   }
//   printf("\n");
//   printf("LRU算法：\n");
//   print();
//   printf("缺页错误数目为：%d\n", sum);
// }

// void OPT() // OPT算法
// {
//   int i, j, k, sum = 1, f, q, max;
//   int seq[7] = {0}; // 记录序列
//   init();
//   stack[0][0] = numbers[0];
//   seq[0] = nums - 1;
//   for (i = 1; i < nums; i++) // 前半部分，页面空置的情况
//   {
//     for (j = 0; j < nums; j++)
//     {
//       stack[i][j] = stack[i - 1][j];
//     }
//     for (j = 0; j < nums; j++)
//     {
//       if (seq[j] == 0)
//       {
//         stack[i][j] = numbers[i];
//         break;
//       }
//     }
//     for (j = 0; j < i; j++) // 将之前的优先级序列都减1
//     {
//       seq[j]--;
//     }
//     seq[i] = nums - 1; // 最近使用的优先级列为最高
//     sum++;
//   }
//   for (i = nums; i < 20; i++) // 后半部分，页面栈中没有空的时候情况
//   {
//     // k=nums-1;//最近的数字的优先级
//     for (j = 0; j < nums; j++) // 前面的页面中内容赋值到新的新的页面中
//     {
//       stack[i][j] = stack[i - 1][j];
//     }
//     for (j = 0; j < nums; j++)
//     {
//       f = 0;
//       if (stack[i][j] == numbers[i])
//       {
//         f = 1;
//         break;
//       }
//     }
//     if (f == 0) // 页面中没有，需要替换的情况
//     {
//       for (q = 0; q < nums; q++) // 优先级序列中最大的就是最久未用的，有可能出现后面没有在用过的情况
//       {
//         seq[q] = 20;
//       }
//       for (j = 0; j < nums; j++) // 寻找新的优先级
//       {
//         for (q = i + 1; q < 20; q++)
//         {
//           if (stack[i][j] == numbers[q])
//           {
//             seq[j] = q - i;
//             break;
//           }
//         }
//       }
//       max = seq[0];
//       k = 0;
//       for (q = 0; q < nums; q++)
//       {
//         if (seq[q] > max)
//         {
//           max = seq[q];
//           k = q;
//         }
//       }
//       stack[i][k] = numbers[i];
//       sum++;
//     }
//     else
//     {
//       // 页面栈中有需要插入的数字，无需变化，替换的优先级也不需要变化
//     }
//   }
//   printf("\n");
//   printf("OPT算法：\n");
//   print();
//   printf("缺页错误数目为：%d\n", sum);
// }