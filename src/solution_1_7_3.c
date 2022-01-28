#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include "checker.h"

/*----------------------------------------------------------------------------*/

static int __init sol_init(void)
{
	int i, j;
	char buf[1000];
	short *arr = NULL;
	int sum, tmp_sum = 0;

	CHECKER_MACRO;

    for (i = 2; i < 15; i++)
    {
		arr = kmalloc(i, GFP_KERNEL);

		for (j = 0; j < i; j++)
			arr[j] = 10;

		sum = array_sum(arr, i);

		for (j = 0; j < i; j++) 
			tmp_sum += arr[j];

		generate_output(sum, arr, i, buf);

		if (tmp_sum == sum) 
        {
            printk(KERN_INFO "%s", buf);
        }
		else
        {
            printk(KERN_ERR "%s", buf);
        }

		kfree(arr);
		tmp_sum = 0;
	}

	return 0;
}

/*----------------------------------------------------------------------------*/

static void __exit sol_exit(void)
{
	CHECKER_MACRO;
}

/*----------------------------------------------------------------------------*/

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Yustitskii <inst: yustitskii_kirill>");
MODULE_DESCRIPTION("Stepik solution");
