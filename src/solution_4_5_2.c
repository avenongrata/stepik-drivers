#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/sysfs.h>
#include <linux/fs.h>
#include <linux/slab.h>

/*----------------------------------------------------------------------------*/

#define DRIVER_NAME "stepik"

/*----------------------------------------------------------------------------*/

/* IOCTL */
#define IOC_MAGIC 'k'
#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char *)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char *)

/*----------------------------------------------------------------------------*/

static int sol_major = 240;
module_param(sol_major, int, 0644);

/*----------------------------------------------------------------------------*/

static int sol_minor = 0;

/*----------------------------------------------------------------------------*/

struct sol_st
{
	dev_t devt;
	struct cdev cdev;
	int str_len;
	int str_sum;
} solst;

/*----------------------------------------------------------------------------*/

static int sol_open(struct inode * inode, struct file * filp)
{
	return 0;
}

/*----------------------------------------------------------------------------*/

static int sol_release(struct inode * inode, struct file * filp)
{
	return 0;
}

/*----------------------------------------------------------------------------*/

static ssize_t sol_read(struct file * filp, char __user * buf, size_t count,
                        loff_t * pos)
{
    char * tbuf = NULL;

	if (*pos != 0)
		return 0;

	tbuf = kzalloc(1000, GFP_KERNEL);

	sprintf(tbuf, "%d %d\n", solst.str_len, solst.str_sum);
	copy_to_user(buf, tbuf, strlen(tbuf));
	*pos += strlen(tbuf);

	return strlen(tbuf);
}

/*----------------------------------------------------------------------------*/

static long sol_unlocked_ioctl(struct file * filp, unsigned int cmd,
                               unsigned long arg)
{	
	long tmp_num;
	char *buf = NULL;
	
	buf = (char *)arg;

    switch(cmd)
    {
	case SUM_LENGTH:
		solst.str_len += strlen(buf);
		return solst.str_len;
		break;	

	case SUM_CONTENT:
		kstrtol(buf, 10, &tmp_num);
		solst.str_sum += tmp_num;
		return solst.str_sum;
		break;

	default:
		break;
	}

	return 0;
}

/*----------------------------------------------------------------------------*/

struct file_operations fops =
{
    .owner          = THIS_MODULE,
    .read           = sol_read,
    .unlocked_ioctl = sol_unlocked_ioctl,
    .open           = sol_open,
    .release        = sol_release,
};

/*----------------------------------------------------------------------------*/

static int __init sol_init(void)
{
	int ret;

    /*------------------------------------------------------------------------*/

    if (sol_major == 240)
    {
		solst.devt = MKDEV(sol_major, sol_minor);
		ret = register_chrdev_region(solst.devt, 1, DRIVER_NAME);
    }
    else
    {
		ret = alloc_chrdev_region(&solst.devt, sol_minor, 1, DRIVER_NAME);
		sol_major = MAJOR(solst.devt);
	}

	if (ret < 0)
		return ret;

    /*------------------------------------------------------------------------*/

	cdev_init(&solst.cdev, &fops);
	solst.str_len = 0;
	solst.str_sum = 0;
	ret = cdev_add(&solst.cdev, solst.devt, 1);

    if (ret < 0)
    {
		unregister_chrdev_region(solst.devt, 1);
		return ret;
	}

    /*------------------------------------------------------------------------*/

	return 0;
}

/*----------------------------------------------------------------------------*/

static void __exit sol_exit(void)
{
	unregister_chrdev_region(solst.devt, 1);	
}

/*----------------------------------------------------------------------------*/

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Yustitskii <inst: yustitskii_kirill>");
MODULE_DESCRIPTION("Stepik solution");
