#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <omp.h>
#define true 1
#define false 0

int NUMBER_OF_CUSTOMERS;
int NUMBER_OF_RESOURCES;

#define LOG_LENGTH 500
#define NUMBER_OF_TRY_COUNT 5
enum ModifyType
{
  Request = -1,
  Release = 1
};

enum AllocateStatus
{
  Success = 0,
  Failure = -1
};

typedef struct AllocateLog
{
  enum ModifyType type;
  int customId;
  enum AllocateStatus status;
  int *request;
  int *safeList;
  int *curAvailable;
} AllocateLog;

int *available;
int *maximum;
int *allocation;
int *need;
AllocateLog logList[LOG_LENGTH];
int logListIdx = 0;
pthread_mutex_t mutexLock;

int returnWithEffect(enum ModifyType type, int customer_num,
                     enum AllocateStatus status,
                     int *request, int *safeList)
{
  int *curAvailable = malloc(sizeof(int) * NUMBER_OF_RESOURCES);
  memcpy(curAvailable, available, sizeof(int) * NUMBER_OF_RESOURCES);

  logList[logListIdx++] = (AllocateLog){type, customer_num,
                                        status, request,
                                        safeList, curAvailable};

  pthread_mutex_unlock(&mutexLock);
  return status;
}

void printInit(int available[], int *maximum)
{
  printf("=====start print init info=====\n");
  printf("资源类型   :");
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
  {
    printf("        %c", 'A' + i);
  }
  printf("\n");

  printf("原始资源量 :");
  for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
  {
    printf("%9d", available[j]);
  }
  printf("\n");

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
  {
    printf("消费者%d上限:", i + 1);
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
    {
      printf("%9d", maximum[i * NUMBER_OF_RESOURCES + j]);
    }
    printf("\n");
  }
}

void printArray(int *arr, int length)
{
  printf("[");
  for (int j = 0; j < length; j++)
  {
    printf("%d", arr[j]);
    if (j != length - 1)
    {
      printf(", ");
    }
  }
  printf("]");
}

void printLog(AllocateLog logList[])
{
  printf("=====start print log=====\n");
  for (int i = 0; i < LOG_LENGTH; i++)
  {
    if (logList[i].type == 0)
    {
      break;
    }
    printf("Customer: %d, Type: %s, Status: %s",
           logList[i].customId,
           logList[i].type == Request ? "Request" : "Release",
           logList[i].status == Success ? "Success" : "Failure");
    printf(", Request: ");
    printArray(logList[i].request, NUMBER_OF_RESOURCES);
    printf(", CurAvailable: ");
    printArray(logList[i].curAvailable, NUMBER_OF_RESOURCES);

    if (logList[i].type == Request && logList[i].status == Success)
    {
      printf(", SafeList: ");
      printArray(logList[i].safeList, NUMBER_OF_CUSTOMERS);
    }
    printf("\n");
  }
}

void modifyResources(int customer_num, int request[], enum ModifyType type)
{
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
  {
    available[i] += type * request[i];
    allocation[customer_num * NUMBER_OF_RESOURCES + i] -= type * request[i];
    need[customer_num * NUMBER_OF_RESOURCES + i] += type * request[i];
  }
}
int sum(int arr[], int length)
{
  int sum = 0;
  for (int i = 0; i < length; i++)
  {
    sum += arr[i];
  }
  return sum;
}

int isA1AllGreaterA2(int arr1[], int arr2[], int length)
{
  for (int i = 0; i < length; i++)
  {
    if (arr1[i] < arr2[i])
    {
      return false;
    }
  }
  return true;
}

int *isSafe(int *request)
{
  //记录safeList
  int *safeList = malloc(sizeof(int) * NUMBER_OF_CUSTOMERS);
  int safeListIdx = 0;
  // step1 初始化
  int *work = malloc(NUMBER_OF_RESOURCES * sizeof(int));
  int *finish = malloc(NUMBER_OF_CUSTOMERS * sizeof(int));
  // work=available
  memcpy(work, available, sizeof(int) * NUMBER_OF_RESOURCES);
  // finish[i]=false
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
  {
    finish[i] = false;
  }
  // step2 查找满足的i
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
  {
    if (!finish[i] &&
        isA1AllGreaterA2(work, need + i * NUMBER_OF_RESOURCES,
                         NUMBER_OF_RESOURCES))
    {
      // step 3.1 work=work+allocation[i]
      for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
      {
        work[j] += allocation[i * NUMBER_OF_RESOURCES + j];
      }
      // step 3.2 finish[i]=true
      finish[i] = true;
      //额外记录一下safeList
      safeList[safeListIdx++] = i;
      //返回到第二步
      i = -1;
    }
  }
  // step4 如果对所有的i finish[i]=true 那么系统处于安全状态
  if (sum(finish, NUMBER_OF_CUSTOMERS) == NUMBER_OF_CUSTOMERS)
  {
    return safeList;
  }
  else
  {
    //否则返回null失败
    return NULL;
  }
}

typedef struct RequestResponse
{
  enum AllocateStatus status;
  int *safeList;
} RequestResponse;

RequestResponse __request_resources(int customer_num, int request[])
{
  // 检查申请合法性
  if (
      isA1AllGreaterA2(need + customer_num * NUMBER_OF_RESOURCES,
                       request, NUMBER_OF_RESOURCES) &&
      isA1AllGreaterA2(available, request, NUMBER_OF_RESOURCES))
  {
    //先分配去判断是否安全
    modifyResources(customer_num, request, Request);
    int *isSafePtr = isSafe(request);
    if (isSafePtr == NULL)
    { //不安全，回退
      modifyResources(customer_num, request, Release);
      return (RequestResponse){Failure, NULL};
    }
    return (RequestResponse){Success, isSafePtr};
  }
  return (RequestResponse){Failure, NULL};
}

enum AllocateStatus __release_resources(int customer_num, int release[])
{
  // 检查释放合法性
  if (isA1AllGreaterA2(allocation + customer_num * NUMBER_OF_RESOURCES,
                       release, NUMBER_OF_RESOURCES))
  {
    modifyResources(customer_num, release, Release);
    return Success;
  }
  return Failure;
}

int request_resources(int customer_num, int request[])
{
  pthread_mutex_lock(&mutexLock);
  RequestResponse res = __request_resources(customer_num, request);
  return returnWithEffect(Request, customer_num, res.status,
                          request, res.safeList);
}

int release_resources(int customer_num, int request[])
{
  pthread_mutex_lock(&mutexLock);
  return returnWithEffect(Release, customer_num,
                          __release_resources(customer_num, request),
                          request, NULL);
}

int main(int argc, char *argv[])
{
  if (argc - 1 != 2)
  { //命令行输入customer数和resource数 检查输入为2个
    printf("输入资源数量不正确,输入了%d个\n", argc - 1);
    return 1;
  }
  NUMBER_OF_CUSTOMERS = atoi(argv[1]);
  NUMBER_OF_RESOURCES = atoi(argv[2]);

  // init
  maximum = malloc(sizeof(int) * NUMBER_OF_CUSTOMERS * NUMBER_OF_RESOURCES);
  allocation = malloc(sizeof(int) * NUMBER_OF_CUSTOMERS * NUMBER_OF_RESOURCES);
  need = malloc(sizeof(int) * NUMBER_OF_CUSTOMERS * NUMBER_OF_RESOURCES);
  available = malloc(sizeof(int) * NUMBER_OF_RESOURCES);

  for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
  { //输入各个资源的初始量
    printf("请输入资源%c的初始量: ", 'A' + j);
    scanf("%d", &available[j]);
  }

  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
  { //输入各个线程的最大分配量
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
    {
      printf("请输入线程%d对资源%c最大需求: ", i + 1, 'A' + j);
      scanf("%d", &maximum[i * NUMBER_OF_RESOURCES + j]);
    }
  }

  pthread_mutex_init(&mutexLock, NULL);
  memcpy(need, maximum, sizeof(int) * NUMBER_OF_CUSTOMERS * NUMBER_OF_RESOURCES);

  printInit(available, maximum);
  omp_set_num_threads(NUMBER_OF_CUSTOMERS);
#pragma omp parallel
  {
    int customer_num = omp_get_thread_num();
    unsigned int seed = (unsigned int)omp_get_thread_num();
    for (int i = 0; i < NUMBER_OF_TRY_COUNT; i++)
    {
      if (rand_r(&seed) % 2)
      {
        //分配
        int *request = malloc(sizeof(int) * NUMBER_OF_RESOURCES);
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        {
          request[i] = rand_r(&seed) % (need[customer_num * NUMBER_OF_RESOURCES + i] + 1);
        }
        request_resources(customer_num, request);
      }
      else
      {
        //释放
        int *release = malloc(sizeof(int) * NUMBER_OF_RESOURCES);
        for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
        {
          release[i] = rand_r(&seed) % (allocation[customer_num * NUMBER_OF_RESOURCES + i] + 1);
        }
        release_resources(customer_num, release);
      }
    }
  }

  printLog(logList);
  return 0;
}