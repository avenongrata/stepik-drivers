#include <linux/module.h>
#include <linux/init.h>
#include "checker.h"

/*----------------------------------------------------------------------------*/

static int __init sol_init(void)
{
	call_me("Hello from my module!");
	return 0;
}

/*----------------------------------------------------------------------------*/

static void __exit sol_exit(void)
{

}

/*----------------------------------------------------------------------------*/

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Yustitskii <inst: yustitskii_kirill>");
MODULE_DESCRIPTION("Stepik solution");
