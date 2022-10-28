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

static int birthday_init(void)
{
  static LIST_HEAD(birthday_list);
  for (int i = 0; i < 5; i++)
  {
    struct birthday *person;
    person = kmalloc(sizeof(*person), GFP_KERNEL);
    person->day = 2;
    person->month = 8 + i;
    person->year = 1995;
    INIT_LIST_HEAD(&person->list);
    list_add_tail(&person->list, &birthday_list);
  }
  struct birthday *ptr;
  list_for_each_entry(ptr, &birthday_init, list)
  {
    printk(KERN_INFO "%d\n", *ptr->month);
  }
  printk(KERN_INFO "Hi\n");
  return 0;
}

static void birthday_exit(void)
{

  printk(KERN_INFO "Removing Module\n");
}

module_init(birthday_init);
module_exit(birthday_exit);

MODULE_LICENSE("MIT");
MODULE_DESCRIPTION("birthday");
MODULE_AUTHOR("Yuan0070");
