#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init sol_init(void)
{
	return 0;
}

static void __exit sol_exit(void)
{

}

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");