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
  boolean isPageFault;
  int pageList[0];
} PageLog;

int REQUEST_NUM = 0;
int PAGE_NUM = 0;

int *REQUEST_TARGET_LIST;

PageLog **pageLogList;

void init()
{
  // init REQUEST_NUM,PAGE_NUM
  printf("请输入请求数量：");
  scanf("%d", &REQUEST_NUM);
  printf("请输入页面帧数量(1-7):");
  scanf("%d", &PAGE_NUM);
  // init REQUEST_TARGET_LIST
  REQUEST_TARGET_LIST = malloc(sizeof(int) * REQUEST_NUM);
  for (int i = 0; i < REQUEST_NUM; i++)
  {
    REQUEST_TARGET_LIST[i] = rand() % 10;
  }
  // init pageLogList
  pageLogList = malloc(sizeof(PageLog *) * REQUEST_NUM);
  for (int i = 0; i < REQUEST_NUM; i++)
  {
    pageLogList[i] = malloc(sizeof(PageLog) + sizeof(int) * PAGE_NUM);
  }
  srand(0);
}

void printLog()
{
  for (int i = 0; i < REQUEST_NUM; i++)
  {
    printf("请求%d,请求页%d,新页面帧:", i + 1, REQUEST_TARGET_LIST[i]);
    for (int j = 0; j < PAGE_NUM; j++)
    {
      printf("%d ", pageLogList[i]->pageList[j]);
    }
    printf(pageLogList[i]->isPageFault ? ",发生缺页\n" : "\n");
  }
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
  *curNode = (FifoNode){-1, NULL};
  FifoNode *firstNode = curNode;
  for (int i = 1; i < PAGE_NUM; i++)
  {
    FifoNode *newNode = malloc(sizeof(FifoNode));
    *newNode = (FifoNode){-1, curNode};
    curNode = newNode;
  }
  firstNode->next = curNode;

  for (int i = 0; i < REQUEST_NUM; i++)
  {
    boolean found = false;
    FifoNode *searcher = curNode;
    for (int j = 0; j < PAGE_NUM; j++)
    {
      if (searcher->page == REQUEST_TARGET_LIST[i])
      {
        found = true;
        break;
      }
      searcher = searcher->next;
    }
    if (!found)
    {
      curNode->page = REQUEST_TARGET_LIST[i];
      curNode = curNode->next;
    }

    // take log
    pageLogList[i]->isPageFault = !found;
    for (int j = 0; j < PAGE_NUM; j++)
    {
      pageLogList[i]->pageList[j] = curNode->page;
      curNode = curNode->next;
    }
  }
}

typedef struct LruNode
{
  int page;
  struct LruNode *next;
  struct LruNode *prev;
} LruNode;

void LRU()
{
  // init lru link list
  LruNode *firstNode = malloc(sizeof(LruNode));
  LruNode *preNode = firstNode;
  firstNode->page = -1;
  firstNode->prev = NULL;
  for (int i = 1; i < PAGE_NUM; i++)
  {
    LruNode *newNode = malloc(sizeof(LruNode));
    newNode->page = -1;
    newNode->prev = preNode;
    newNode->next = NULL;
    preNode->next = newNode;
    preNode = newNode;
  }
  LruNode *lastNode = preNode;

  for (int i = 0; i < REQUEST_NUM; i++)
  {
    boolean found = false;
    LruNode *searcher = firstNode;
    for (int j = 0; j < PAGE_NUM; j++)
    {
      if (searcher->page == REQUEST_TARGET_LIST[i])
      {
        found = true;
        // swap searcher and head
        if (searcher != firstNode)
        {
          searcher->prev->next = searcher->next;
          if (searcher != lastNode)
          {
            searcher->next->prev = searcher->prev;
          }
          searcher->next = firstNode;
          searcher->prev = NULL;
          firstNode->prev = searcher;
          firstNode = searcher;
        }
        break;
      }
      searcher = searcher->next;
    }

    if (!found)
    {
      lastNode->page = REQUEST_TARGET_LIST[i];
      // swap lastnode and head
      lastNode->next = firstNode;
      firstNode->prev = lastNode;
      firstNode = lastNode;
      LruNode *tmp = lastNode->prev;
      lastNode->prev = NULL;
      lastNode = tmp;
    }

    // take log
    pageLogList[i]->isPageFault = !found;
    searcher = firstNode;
    for (int j = 0; j < PAGE_NUM; j++)
    {
      pageLogList[i]->pageList[j] = searcher->page;
      searcher = searcher->next;
    }
  }
}

void OPT()
{
  int **lastSeenMap = malloc(sizeof(int) * REQUEST_NUM * 10);
  for (int pageI; pageI < 10; pageI++)
  {
    for (int requestI; requestI < REQUEST_NUM; requestI++)
    {
      lastSeenMap[pageI][requestI];
    }
  }
}

int main()
{
  init();
  // FIFO();
  // printLog();
  // LRU();
  // printLog();
  return 0;
}

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