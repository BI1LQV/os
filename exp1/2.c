#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/slab.h>

struct birthday
{
  int day;
  int month;
  int year;
  struct list_head list;
};
static LIST_HEAD(birthday_list);
static int birthday_init(void)
{
  struct birthday *ptr;
  int i = 0;
  for (; i < 5; i++)
  {
    struct birthday *person;
    person = kmalloc(sizeof(*person), GFP_KERNEL);
    person->day = 2;
    person->month = 8 + i;
    person->year = 1995;
    INIT_LIST_HEAD(&person->list);
    list_add_tail(&person->list, &birthday_list);
  }

  list_for_each_entry(ptr, &birthday_list, list)
  {
    printk(KERN_INFO "the person's birth month is %d\n", ptr->month);
  }
  return 0;
}

static void birthday_exit(void)
{
  struct birthday *ptr, *next;
  list_for_each_entry_safe(ptr, next, &birthday_list, list)
  {
    list_del(&ptr->list);
    kfree(ptr);
  }
  printk(KERN_INFO "module removed\n");
}

module_init(birthday_init);
module_exit(birthday_exit);

MODULE_LICENSE("MIT");
MODULE_DESCRIPTION("birthday");
MODULE_AUTHOR("Yuan0070");
