#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/sched/signal.h>

int count = 0;

void dfsPrint(struct task_struct *root)
{
  struct task_struct *task;
  struct list_head *list;
  char state[7];
  char initd[7] = "      ";
  int i;
  list_for_each(list, &root->children)
  {
    task = list_entry(list, struct task_struct, sibling);
    memcpy(state, initd, sizeof(char) * 7);
    i = 5;
    if ((task->__state & TASK_RUNNING) == TASK_RUNNING)
    {
      state[--i] = 'R';
    }
    if ((task->__state & TASK_INTERRUPTIBLE) == TASK_INTERRUPTIBLE)
    {
      state[--i] = 'S';
    }
    if ((task->__state & TASK_UNINTERRUPTIBLE) == TASK_UNINTERRUPTIBLE)
    {
      state[--i] = 'D';
    }
    //后面的是自己造的缩写（因为不知道也没查到
    if ((task->__state & TASK_NOLOAD) == TASK_NOLOAD)
    {
      state[--i] = 'Z';
    }
    if ((task->__state & TASK_PARKED) == TASK_PARKED)
    {
      state[--i] = 'P';
    }
    if ((task->__state & TASK_WAKEKILL) == TASK_WAKEKILL)
    {
      state[--i] = 'K';
    }
    if ((task->exit_state & EXIT_ZOMBIE) == EXIT_ZOMBIE)
    {
      state[--i] = 'Z';
    }
    if ((task->exit_state & EXIT_DEAD) == EXIT_DEAD)
    {
      state[--i] = 'A';
    }
    count++;
    printk(KERN_INFO "%6d %s%20s\n", task->pid, state, task->comm);
    dfsPrint(task);
  }
}

static int ps_init(void)
{
  printk(KERN_INFO "   PID  STATE                NAME\n");
  dfsPrint(&init_task);
  printk(KERN_INFO "count %d\n", count);
  return 0;
}

static void ps_exit(void)
{
  printk(KERN_INFO "module removed\n");
}

module_init(ps_init);
module_exit(ps_exit);

MODULE_LICENSE("MIT");
MODULE_DESCRIPTION("task printer");
MODULE_AUTHOR("Yuan0070");
