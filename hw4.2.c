#include <stdio.h>
#include <omp.h>
#include <sys/time.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#define THREAD_NUM 7
struct lockNode
{
  int turn;
  int flag[2];
};
int treeSize;
struct lockNode *root;
int **routerPath;
struct lockNode *create_lock(int size) //创建一个数组形状二叉树
{
  int depth = (int)sqrt((double)size) + 2;
  treeSize = pow(2.0, (double)depth);
  struct lockNode *root = malloc(sizeof(struct lockNode) * (treeSize));
  for (int i = 0; i < treeSize; i++)
  {
    root[i] = (struct lockNode){0, {0, 0}};
  }
  routerPath = malloc(sizeof(int *) * treeSize);
  return root;
}

int *getPath(int thread_id) //返回从树根到子叶的路径 其中path[0]是路径长度
{
  if (routerPath[thread_id])
  {
    return routerPath[thread_id];
  }
  int *path = malloc(sizeof(int) * 100);
  int i = 1;
  int id = treeSize / 2 + thread_id;
  while (id > 1)
  {
    path[i] = id - 1;
    i++;
    id /= 2;
  }
  path[0] = i;
  routerPath[thread_id] = path;
  return path;
}
void __enter_lock(struct lockNode root[], int directions[], int cur)
{
  if (cur == 1)
  {
    return;
  }

  int direction = (directions[cur - 1] - 1) % 2;
  root[directions[cur]].flag[direction] = 1;
  root[directions[cur]].turn = direction;
  while (root[directions[cur]].turn == direction && (root[directions[cur]].flag[1 - direction] == 1))
    ;
  __enter_lock(root, directions, cur - 1);
}

void __exit_lock(struct lockNode root[], int directions[], int cur)
{
  if (cur == directions[0]) //到根了
  {
    return;
  }
  int sibling;
  int direction;
  if (directions[cur] % 2 == 0)
  {
    sibling = directions[cur] - 1;
    direction = 1;
  }
  else
  {
    sibling = directions[cur] + 1;
    direction = 0;
  }
  int father = (directions[cur] - 1) / 2;
  root[father].flag[direction] = 0;
  __exit_lock(root, directions, cur + 1);
}

void enter_lock(struct lockNode root[], int thread_id)
{
  int *path = getPath(thread_id);
  __enter_lock(root, path, path[0]);
}

void exit_lock(struct lockNode root[], int thread_id)
{
  int *path = getPath(thread_id);
  __exit_lock(root, path, 1);
}

int i = 0;
int counter[THREAD_NUM] = {0};
int swap = 0;
int swapCount = 0;

void spinning(int id)
{
  while (1)
  {
    enter_lock(root, id);
    if (i >= 1000000)
    {
      break;
    }
    i++;
    counter[id]++;
    if (swap != id)
    {
      swap = id;
      swapCount++;
    }
    exit_lock(root, id);
  }
  exit_lock(root, id);
}

int main()
{
  struct timeval startTime;
  struct timeval endTime;
  gettimeofday(&startTime, NULL);
  root = create_lock(THREAD_NUM);
#pragma omp parallel num_threads(THREAD_NUM)
  {
    spinning(omp_get_thread_num());
  }
  gettimeofday(&endTime, NULL);
  int sum = 0;
  for (int i = 0; i < THREAD_NUM; i++)
  {
    sum += counter[i];
  }
  printf("%d\n", sum);
  printf("spent %ld ms\n",
         (endTime.tv_sec - startTime.tv_sec) * 1000 +
             (endTime.tv_usec - startTime.tv_usec) / 1000);
  return 0;
}