#include <linux/module.h>
#include <linux/init.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/kdev_t.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <asm/uaccess.h> 

/*----------------------------------------------------------------------------*/

#define DRIVER_NAME "stepik_solution"

/*----------------------------------------------------------------------------*/

static int sol_major = 240;
module_param(sol_major, int, 0644);
static int sol_minor = 0;

/*----------------------------------------------------------------------------*/

struct sol_dev
{
	int open_count;
	unsigned long data_count;
    char * buf;

	dev_t devt;
	struct cdev cdev;
} sdev;

/*----------------------------------------------------------------------------*/

static int get_count(int digit)
{
	int len = 1;

	while (digit /= 10)
		len++;

	return len;
}

/*----------------------------------------------------------------------------*/

static int sol_open(struct inode * inode, struct file * filp)
{
    struct sol_dev * dev;

	dev = container_of(inode->i_cdev, struct sol_dev, cdev);
	filp->private_data = dev;

	sdev.open_count++;

	return 0;
}

/*----------------------------------------------------------------------------*/

static int sol_release(struct inode * inode, struct file * filp)
{
	return 0;
}

/*----------------------------------------------------------------------------*/

ssize_t sol_read(struct file * filp, char __user * buf,
                 size_t count, loff_t * pos)
{
    struct sol_dev * dev;
	int tmp_count;
	int mem_shift = 0;

    /*------------------------------------------------------------------------*/

	if (*pos != 0)
		return 0;

    /*----------------------------------------------------------------------------*/

	dev = filp->private_data;

	dev->buf = kmalloc(1000, GFP_KERNEL);
	sprintf(dev->buf, "%d ", sdev.open_count);

	tmp_count = get_count(sdev.open_count) + 1;
	sprintf((dev->buf)+tmp_count, "%ld\n", sdev.data_count);
	tmp_count = tmp_count + get_count(sdev.data_count) + 2;

	dev->buf[tmp_count] = '\0';

    /*------------------------------------------------------------------------*/
	
    while (*(dev->buf))
    {
		put_user(*(dev->buf++), buf++);
		mem_shift++;
	}

	dev->buf -= mem_shift;
	kfree(dev->buf);

	*pos += tmp_count;

    /*------------------------------------------------------------------------*/

	return tmp_count;
}

/*----------------------------------------------------------------------------*/

ssize_t sol_write(struct file * filp, const char __user * buf,
                  size_t count, loff_t * pos)
{
    struct sol_dev * dev;
	
	dev = filp->private_data;
	dev->data_count += count;

	return count;
}

/*----------------------------------------------------------------------------*/

struct file_operations fops =
{
    .open    = sol_open,
    .read    = sol_read,
    .write   = sol_write,
	.release = sol_release,
};

/*----------------------------------------------------------------------------*/

static int __init sol_init(void)
{
	int ret;

    /*------------------------------------------------------------------------*/

    if (sol_major == 240)
    {
		sdev.devt = MKDEV(sol_major, sol_minor);
		ret = register_chrdev_region(sdev.devt, 1, DRIVER_NAME);
    }
    else
    {
		ret = alloc_chrdev_region(&sdev.devt, sol_minor, 1, DRIVER_NAME);
		sol_major = MAJOR(sdev.devt);
	}

	if (ret < 0)
		return ret;

    /*------------------------------------------------------------------------*/

	cdev_init(&sdev.cdev, &fops);
	sdev.cdev.owner = THIS_MODULE;

    sdev.buf        = NULL;
	sdev.data_count = 0;
	sdev.open_count = 0;

	ret = cdev_add(&sdev.cdev, sdev.devt, 1);

	if (ret < 0)
		return ret;

    /*------------------------------------------------------------------------*/

	return 0;
}

/*----------------------------------------------------------------------------*/

static void __exit sol_exit(void)
{
	cdev_del(&sdev.cdev);
	unregister_chrdev_region(sdev.devt, 1);
}

/*----------------------------------------------------------------------------*/

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Yustitskii <inst: yustitskii_kirill>");
MODULE_DESCRIPTION("Stepik solution");
