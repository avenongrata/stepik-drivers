#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/device.h>
#include "checker.h"

/*----------------------------------------------------------------------------*/

void * void_p;
int  * int_p;
struct device * dev_p;

/*----------------------------------------------------------------------------*/

static int __init sol_init(void)
{

    void_p = kmalloc(get_void_size(), GFP_KERNEL);
    submit_void_ptr(void_p);

    int_p = kmalloc(get_int_array_size() * sizeof(int), GFP_KERNEL);
    submit_int_array_ptr(int_p);

    dev_p = kmalloc(sizeof(struct device), GFP_KERNEL);
    submit_struct_ptr(dev_p);

    return 0;
}

/*----------------------------------------------------------------------------*/

static void __exit sol_exit(void)
{
    checker_kfree(void_p);
    checker_kfree(int_p);
    checker_kfree(dev_p);

}

/*----------------------------------------------------------------------------*/

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Yustitskii <inst: yustitskii_kirill>");
MODULE_DESCRIPTION("Stepik solution");
