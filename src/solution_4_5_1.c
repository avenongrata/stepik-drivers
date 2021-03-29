#include <linux/module.h>
#include <linux/init.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/string.h>
#include <linux/slab.h>

#define true 1

struct kobject *sol_kobj;

struct sol_array {
	char *array[100];
	int count;
};


static int sol_compare(int elem_index, char **array, int count)
{
	int i, ret;
	int new_index = -1;

	for (i = 0; i < count; i++) {
		if (elem_index == count)	// same string
			continue;

		if (array[i] == NULL)		// empty string
			continue;

		ret = strcmp(array[elem_index], array[i]);
		if (!ret || ret  < 0)		// equal or smaller
			continue;
		else {
			new_index = i;
			break;
		}
	}

	if (new_index == -1)
		return elem_index;
	else
		return new_index;
}


static struct sol_array * fill_array_with_strings(void)
{
	struct list_head *lst;
	struct module *obj;
	int count = 0;
	struct sol_array *solarray = NULL;
	unsigned int len;

	solarray = kmalloc(sizeof(struct sol_array), GFP_KERNEL);
	if (!solarray) {
		printk(KERN_INFO "kernel_mooc: can't alloc memmory for solarray\n");
		return NULL;
	}

	list_for_each(lst, THIS_MODULE->list.prev) {
		obj = list_entry(lst, struct module, list);
		len = strlen(obj->name) + 1;
		solarray->array[count] = kmalloc(len, GFP_KERNEL);
		strcpy(solarray->array[count], obj->name);
		count++;
	}
	solarray->array[count] = NULL;
	solarray->count = count;

	return solarray;
}


static struct sol_array * sol_sort(void)
{
	struct sol_array *solarray, *sorted = NULL;
	int i;
	unsigned int len;
	int index, prev_index;
	int count = 0;

	sorted = kmalloc(sizeof(struct sol_array), GFP_KERNEL);
	if (!sorted) {
		printk(KERN_INFO "kernel_mooc: can't alloc memmory for sorted\n");
		return NULL;
	}

	solarray = fill_array_with_strings();
	if (!solarray) 
		return NULL;

	for (i = 0; i < solarray->count; i++) {
		if (solarray->array[i] == NULL)
			continue;

		index = i;
		while (true) {
			prev_index = index;
			index = sol_compare(index, solarray->array, solarray->count);
			if (index == prev_index) {		// was already the smallest string
				len = strlen(solarray->array[index]) + 1;
				sorted->array[count] = kmalloc(len, GFP_KERNEL);
				strcpy(sorted->array[count], solarray->array[index]);
				count ++;
				kfree(solarray->array[index]);	// delete from array
				solarray->array[index] = NULL;
				i = -1;
				break;
			}
		}
	}
	sorted->count = solarray->count;

	/*
		free allocated memmory
	*/
	kfree(solarray);	// was allocated in fill_array_with_strings()
	
	return sorted;
}


ssize_t sol_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	int count, tmp_count;
	struct sol_array *array;
	int i;

	array = sol_sort();
	if (!array)
		return 0;

	tmp_count = 0;
	count = 0;
	for (i = 0; i < array->count; i++) {
		tmp_count = sprintf(buf, "%s\n", array->array[i]) + 1;
		buf += tmp_count;
		count += tmp_count;
	}


	/*
		free allocated memmory
	*/
	for (i = 0; i < array->count; i++)
		kfree(array->array[i]);	 
	kfree(array);		// was allocated in sol_sort()

	return count;
}


ssize_t sol_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
	return 0;
}


static struct kobj_attribute attr = 
	__ATTR(my_sys, 0644, sol_show, sol_store);

static struct attribute *attrs[] = {
	&attr.attr,
	NULL,
};


static struct attribute_group grp = {
	.attrs = attrs,
};


static int __init sol_init(void)
{

	sol_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
	if (!sol_kobj) {
		printk(KERN_INFO "kernel_mooc: Can't create kobject\n");
		return -1;
	} else 
		printk(KERN_INFO "kernel_mooc: kobject created\n");

	if (sysfs_create_group(sol_kobj, &grp)) {
		printk(KERN_INFO "kernel_mooc: Can't create attr group in my_kobject\n");
		kobject_put(sol_kobj);
		return -1;
	} else
		printk(KERN_INFO "kernel_mooc: Attr group created\n");


	return 0;
}


static void __exit sol_exit(void)
{
	kobject_put(sol_kobj);
}

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");	
