#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <linux/device.h>

/*----------------------------------------------------------------------------*/

#define IRQ_NUMBER      8
#define IRQ_DEV_NAME    "stepik"
#define SOL_KOBJ_NAME   "my_kobject"

/*----------------------------------------------------------------------------*/

struct sol_irq
{
    int irq_counter;
    int irq_ret;
    struct kobject * kobj;
} solirq;

/*----------------------------------------------------------------------------*/

ssize_t sol_show(struct kobject * kobj, struct kobj_attribute * attr,
                 char * buf)
{
    return sprintf(buf, "%d", solirq.irq_counter);
}

/*----------------------------------------------------------------------------*/

ssize_t sol_store(struct kobject * kobj, struct kobj_attribute * attr,
                  const char * buf, size_t count)
{
    return 0;
}

/*----------------------------------------------------------------------------*/

struct kobj_attribute sol_attr = 
                __ATTR(my_sys, 0644, sol_show, sol_store);

struct attribute * attrs[] =
{
    &sol_attr.attr,
    NULL,
};

const struct attribute_group grp =
{
    .attrs = attrs,
};

/*----------------------------------------------------------------------------*/

static irqreturn_t irq_handler(int irq, void * dev_id)
{
    if (dev_id != &solirq.irq_ret)
        return IRQ_NONE;

    solirq.irq_counter++;

    return IRQ_HANDLED;
}

/*----------------------------------------------------------------------------*/

static int __init sol_init(void)
{

    solirq.irq_counter = 0;

    /* request irq */
    if (request_irq(IRQ_NUMBER, irq_handler, IRQF_SHARED, IRQ_DEV_NAME,
                    &solirq.irq_ret))
    {
        printk(KERN_INFO "Can't request irq line\n");

        return -1;
    }

    /*------------------------------------------------------------------------*/

    /* make kobject */
    solirq.kobj = kobject_create_and_add(SOL_KOBJ_NAME, kernel_kobj);
    if (!solirq.kobj)
    {
        printk(KERN_INFO "Can't create kobject\n");

        free_irq(IRQ_NUMBER, &solirq.irq_ret);

        return -1;
    }

    /*------------------------------------------------------------------------*/

    if (sysfs_create_group(solirq.kobj, &grp))
    {
        printk(KERN_INFO "Can't create group\n");

        kobject_put(solirq.kobj);
        free_irq(IRQ_NUMBER, &solirq.irq_ret);

        return -1;
    }

    /*------------------------------------------------------------------------*/

    return 0;
}

/*----------------------------------------------------------------------------*/

static void __exit sol_exit(void)
{
    free_irq(IRQ_NUMBER, &solirq.irq_ret);
    kobject_put(solirq.kobj);
}

/*----------------------------------------------------------------------------*/

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Yustitskii <inst: yustitskii_kirill>");
MODULE_DESCRIPTION("Stepik solution");
