#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

static int a;
module_param(a, int, 0444);
MODULE_PARM_DESC(a, "Allows you to set own \"a\" variable");

static int b;
module_param(b, int, 0444);
MODULE_PARM_DESC(b, "Allows you to set own \"b\" variable");

static int c[5];
static int arr_len;
module_param_array(c, int, &arr_len, 0444);
MODULE_PARM_DESC(b, "Allows you to set own \"c\" array");

static int get_sum(void)
{
	int sum = 0;
	int i;

	sum += a;
	sum += b;

	for (i = 0; i < arr_len; i++)
		sum += c[i]; 

	return sum;
}

ssize_t sol_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int sum;

	sum = get_sum();
	sprintf(buf, "%d\n", sum);

	return strlen(buf);

}

ssize_t sol_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	return -EACCES;
}

struct kobject *sol_kobj;

static struct kobj_attribute gr_attr = 
	__ATTR(my_sys, 0664, sol_show, sol_store);

static struct attribute *attrs[] = {
	&gr_attr.attr,
	NULL,
};

static struct attribute_group attr = {
	.attrs = attrs,
};


static int __init sol_init(void)
{
	sol_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
	if (!sol_kobj)
		return -1;

	if (sysfs_create_group(sol_kobj, &attr)) {
		kobject_put(sol_kobj);
		return -1;
	}

	return 0;
}

static void __exit sol_exit(void)
{
	sysfs_remove_group(sol_kobj, &attr);
	kobject_put(sol_kobj);
}

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
