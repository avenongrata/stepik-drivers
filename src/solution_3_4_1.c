#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/device.h>

/*----------------------------------------------------------------------------*/

static char * node_name = "stepik";
module_param(node_name, charp, 0644);

/*----------------------------------------------------------------------------*/

struct class *cl;

/*----------------------------------------------------------------------------*/

struct sol_cdev
{
	dev_t devt;
	struct cdev cdev;
	int major;

    char * path;
    struct device * device;
} sdev;

/*----------------------------------------------------------------------------*/

static char * make_path(void)
{
    char * dev   = "/dev/";
    char * p     = NULL;
    char * tmp_p = NULL;
	int i = 0;

    /*------------------------------------------------------------------------*/

	p = kmalloc(1000, GFP_KERNEL);
	tmp_p = p;

	while (dev[i] != '\0')
		*p++ = dev[i++];

    /*------------------------------------------------------------------------*/

	i = 0;
	while (node_name[i] != '\0')
		*p++ = node_name[i++];

	*p = '\0';

    /*------------------------------------------------------------------------*/

	return tmp_p;
}

/*----------------------------------------------------------------------------*/

static char * make_msg(int major)
{
    char * p = NULL;

	p = kmalloc(1000, GFP_KERNEL);
	sprintf(p, "%d\n", major);

	return p;
}

/*----------------------------------------------------------------------------*/

static int sol_open(struct inode * inode, struct file * filp)
{
    struct sol_cdev * dev;

	dev = container_of(inode->i_cdev, struct sol_cdev, cdev);
	filp->private_data = dev;

	return 0;
}

/*----------------------------------------------------------------------------*/

static int sol_release(struct inode * inode, struct file * filp)
{
	return 0;
} 

/*----------------------------------------------------------------------------*/

static ssize_t sol_read(struct file * filp, char __user * buf,
                        size_t count, loff_t * pos)
{
    char * msg    = NULL;
	int mem_shift = 0;
    struct sol_cdev * dev;

    /*------------------------------------------------------------------------*/

	if (*pos != 0)
		return 0;

    /*------------------------------------------------------------------------*/

	dev = filp->private_data;
	msg = make_msg(dev->major);

    while (*msg)
    {
		put_user(*(msg++), buf++);
		mem_shift++;
	}

	msg -= mem_shift;
    kfree(msg);
    *pos += mem_shift;

    /*------------------------------------------------------------------------*/

	return mem_shift;
}

/*----------------------------------------------------------------------------*/

struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = sol_open,
	.release = sol_release,
    .read    = sol_read,
};

/*----------------------------------------------------------------------------*/

static int __init sol_init(void)
{
	int ret;

    /*------------------------------------------------------------------------*/

	ret = alloc_chrdev_region(&sdev.devt, 0, 1, "stepik");
	if (ret < 0)
		return ret;

    /*------------------------------------------------------------------------*/

	cl = class_create(THIS_MODULE, "stepik");
	if (!cl)
		return -1;

    /*------------------------------------------------------------------------*/

    sdev.major = MAJOR(sdev.devt);

	sdev.device = device_create(cl, NULL, sdev.devt, NULL, "%s", node_name);
    if (!sdev.device)
    {
		unregister_chrdev_region(sdev.devt, 1);
		return -1;
    }

    /*------------------------------------------------------------------------*/

	cdev_init(&sdev.cdev, &fops);
	sdev.cdev.owner = THIS_MODULE;
	sdev.path = NULL;
    ret = cdev_add(&sdev.cdev, sdev.devt, 1);

    if (ret < 0)
    {
		device_destroy(cl, sdev.devt);
		class_destroy(cl);
        unregister_chrdev_region(sdev.devt, 1);

		return ret;
	}

    /*------------------------------------------------------------------------*/

	return 0;
}

/*----------------------------------------------------------------------------*/

static void __exit sol_exit(void)
{
	if (sdev.path != NULL)
		kfree(sdev.path);

	device_destroy(cl, sdev.devt);
	class_destroy(cl);
	unregister_chrdev_region(sdev.devt, 1);
}

/*----------------------------------------------------------------------------*/

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Yustitskii <inst: yustitskii_kirill>");
MODULE_DESCRIPTION("Stepik solution");
