#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

/*----------------------------------------------------------------------------*/

static struct kobject * sol_kobj;
int read_count = 0;

/*----------------------------------------------------------------------------*/

static ssize_t sol_show(struct kobject * kobj, struct kobj_attribute * attr,
                        char * buf)
{

    read_count++;
    sprintf(buf, "%d\n", read_count);

    return strlen(buf);
}

/*----------------------------------------------------------------------------*/

static ssize_t sol_store(struct kobject * kobj, struct kobj_attribute * attr,
                         const char * buf, size_t count)
{
    return -EAGAIN;
}

/*----------------------------------------------------------------------------*/

static struct kobj_attribute gr_attr = 
    __ATTR(my_sys, 0664, sol_show, sol_store);

static struct attribute * attrs[] =
{
    &gr_attr.attr,
    NULL,
};

static struct attribute_group attr =
{
    .attrs = attrs,
};

/*----------------------------------------------------------------------------*/

static int __init sol_init(void)
{
    int ret;

    sol_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
    if (!sol_kobj)
        return -1;

    ret = sysfs_create_group(sol_kobj, &attr);
    if (ret)
    {
        kobject_put(sol_kobj);
        return ret;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/

static void __exit sol_exit(void)
{
    sysfs_remove_group(sol_kobj, &attr);
    kobject_put(sol_kobj);
}

/*----------------------------------------------------------------------------*/

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Yustitskii <inst: yustitskii_kirill>");
MODULE_DESCRIPTION("Stepik solution");
