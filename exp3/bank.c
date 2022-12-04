#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define true 1
#define false 0

#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 3
#define CRMatrix [NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES]
#define LOG_LENGTH 5000
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
  enum AllocateStatus status;
  int safeList[NUMBER_OF_CUSTOMERS];
} AllocateLog;

int available[NUMBER_OF_RESOURCES];
int maximum CRMatrix;
int allocation CRMatrix;
int need CRMatrix;
AllocateLog logList[LOG_LENGTH];
int logListIdx = 0;
pthread_mutex_t mutexLock;

int returnWithEffect(enum ModifyType type, enum AllocateStatus status, int *safeList)
{
  if (type == Request && status == Success)
  {
    logList[logListIdx++] = (AllocateLog){type, status, *safeList};
    free(safeList);
  }
  else
  {
    logList[logListIdx++] = (AllocateLog){type, status};
  }
  pthread_mutex_unlock(&mutexLock);
  return status;
}

void printInit(int available[], int maximum CRMatrix)
{
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
      printf("%9d", maximum[i][j]);
    }
    printf("\n");
  }
}

void printLog(AllocateLog logList[])
{
  for (int i = 0; i < LOG_LENGTH; i++)
  {
    if (logList[i].type == 0)
    {
      break;
    }
    printf("Type: %s, Status: %s\n",
           logList[i].type == Request ? "Request" : "Release",
           logList[i].status == Success ? "Success" : "Failure");
  }
}

void modifyResources(int customer_num, int request[], enum ModifyType type)
{
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
  {
    available[i] += type * request[i];
    allocation[customer_num][i] -= type * request[i];
    need[customer_num][i] += type * request[i];
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
  int work[NUMBER_OF_RESOURCES];
  int finish[NUMBER_OF_CUSTOMERS];
  int *_safeList = malloc(sizeof(int) * NUMBER_OF_CUSTOMERS);
  int _safeListIdx = 0;
  memcpy(work, available, sizeof(int) * NUMBER_OF_RESOURCES);
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
  {
    work[i] -= request[i];
  }
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
  {
    finish[i] = false;
  }
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
  {
    if (!finish[i] && isA1AllGreaterA2(work, need[i], NUMBER_OF_RESOURCES))
    {
      for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
      {
        work[j] += allocation[i][j];
      }
      finish[i] = true;
      _safeList[_safeListIdx++] = i;
      i = -1;
    }
  }
  if (sum(finish, NUMBER_OF_CUSTOMERS) == NUMBER_OF_CUSTOMERS)
  {
    return _safeList;
  }
  else
  {
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
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
  {
    if (request[i] > need[customer_num][i] || request[i] > available[i])
    {
      return (RequestResponse){Failure, NULL};
    }
  }
  int *isSafePtr = isSafe(request);
  if (isSafePtr == NULL)
  {
    return (RequestResponse){Failure, NULL};
  }
  modifyResources(customer_num, request, Request);
  return (RequestResponse){Success, isSafePtr};
}

enum AllocateStatus __release_resources(int customer_num, int release[])
{
  // 检查释放合法性
  for (int i = 0; i < NUMBER_OF_RESOURCES; i++)
  {
    if (release[i] > allocation[customer_num][i])
    {
      return Failure;
    }
  }
  modifyResources(customer_num, release, Release);
  return Success;
}

int request_resources(int customer_num, int request[])
{
  pthread_mutex_lock(&mutexLock);
  RequestResponse res = __request_resources(customer_num, request);
  return returnWithEffect(Request, res.status, res.safeList);
}

int release_resources(int customer_num, int request[])
{
  pthread_mutex_lock(&mutexLock);
  return returnWithEffect(Release, __release_resources(customer_num, request), NULL);
}

int main(int argc, char *argv[])
{
  for (int i = 1; i < argc; i++)
  {
    // available[i - 1] = atoi(argv[i]);
  }
  available[0] = 10;
  available[1] = 20;
  available[2] = 13;
  srand(5);
  pthread_mutex_init(&mutexLock, NULL);
  for (int i = 0; i < NUMBER_OF_CUSTOMERS; i++)
  {
    for (int j = 0; j < NUMBER_OF_RESOURCES; j++)
    {
      maximum[i][j] = rand() % (available[j] + 1);
      need[i][j] = maximum[i][j];
    }
  }
  printInit(available, maximum);
  request_resources(0, (int[]){1, 2, 0});
  printLog(logList);
  return 0;
}