#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

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
  printf("请输入请求数量: ");
  scanf("%d", &REQUEST_NUM);
  printf("请输入页面帧数量(1-7): ");
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

  printf("请求列表: ");
  for (int i = 0; i < REQUEST_NUM; i++)
  {
    printf("%d ", REQUEST_TARGET_LIST[i]);
  }
  printf("\n");
}

void printLog(char *type)
{
  printf("\n置换算法为: %s\n", type);
  int faultCount = 0;
  for (int i = 0; i < REQUEST_NUM; i++)
  {
    printf("请求%d,请求页%d,新页面帧:", i + 1, REQUEST_TARGET_LIST[i]);
    for (int j = 0; j < PAGE_NUM; j++)
    {
      printf("%d ", pageLogList[i]->pageList[j]);
    }
    printf(pageLogList[i]->isPageFault ? ",发生缺页\n" : "\n");
    if (pageLogList[i]->isPageFault)
    {
      faultCount++;
    }
  }
  printf("总缺页数: %d\n\n", faultCount);
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
          else
          {
            lastNode = searcher->prev;
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
      // swap lastNode and head
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
  int(*lastSeenMap)[10] = (int(*)[10])malloc(sizeof(int) * REQUEST_NUM * 10);
  for (int pageI = 0; pageI < 10; pageI++)
  {
    int lastSeen = INT32_MAX;
    for (int requestI = REQUEST_NUM - 1; requestI > -1; requestI--)
    {
      if (REQUEST_TARGET_LIST[requestI] == pageI)
      {
        lastSeen = requestI;
      }
      lastSeenMap[requestI][pageI] = lastSeen - requestI;
    }
  }

  int *optPages = malloc(sizeof(int) * PAGE_NUM);
  for (int i = 0; i < PAGE_NUM; i++)
  {
    optPages[i] = -1;
  }
  for (int i = 0; i < REQUEST_NUM; i++)
  {
    boolean found = false;
    for (int j = 0; j < PAGE_NUM; j++)
    {
      if (optPages[j] == REQUEST_TARGET_LIST[i])
      {
        found = true;
      }
    }
    if (!found)
    {
      int toReplace = -1;
      int maxLastSeen = 0;
      for (int j = 0; j < PAGE_NUM; j++)
      {
        if (maxLastSeen < lastSeenMap[REQUEST_TARGET_LIST[i]][optPages[j]])
        {
          toReplace = j;
          maxLastSeen = lastSeenMap[REQUEST_TARGET_LIST[i]][optPages[j]];
        }
        if (optPages[j] == -1)
        {
          toReplace = j;
          break;
        }
      }
      optPages[toReplace] = REQUEST_TARGET_LIST[i];
    }

    // take log
    pageLogList[i]->isPageFault = !found;
    memcpy(pageLogList[i]->pageList, optPages, sizeof(int) * PAGE_NUM);
  }
}

int main()
{
  init();
  FIFO();
  printLog("FIFO");
  LRU();
  printLog("LRU");
  OPT();
  printLog("OPT");
  return 0;
}
