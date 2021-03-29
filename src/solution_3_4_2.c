#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/sysfs.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#define DRIVER_NAME "stepik"
#define BUF_LEN 256

static int sol_major = 240;
module_param(sol_major, int, 0644);

static int sol_minor = 0;

static int get_digit_count(int digit)
{
	int len = 1;

    while (digit /= 10)
        len++;

    return len;
}

struct sol_dev {
	dev_t devt;
	struct cdev cdev;
	int session_count;
	struct mutex sol_mutex;
} sdev;

struct sol_st {
	int cur_session;
	int last_byte;
	char *buf;
	int buf_size;
	int read_flag;
	char *f_read;
	struct sol_dev *dev;
};

static int sol_open(struct inode *inode, struct file *filp)
{
	struct sol_dev *dev;
	struct sol_st *sol_st;
	int i;

	sol_st = kmalloc(sizeof(struct sol_st), GFP_KERNEL);
	dev = container_of(inode->i_cdev, struct sol_dev, cdev);
	sol_st->dev = dev;
	filp->private_data = sol_st;
	
	mutex_lock(&sdev.sol_mutex);
	sol_st->cur_session = sdev.session_count;
	sdev.session_count++;
	mutex_unlock(&sdev.sol_mutex);

	sol_st->buf = NULL;
	sol_st->f_read = NULL;
	sol_st->buf_size = BUF_LEN;
	sol_st->read_flag = 0;
	sol_st->buf = kmalloc(sol_st->buf_size, GFP_KERNEL);	// not optimized -> malloc mem in write method
	for (i = 0; i < sol_st->buf_size; i++)
		sol_st->buf[i] = '\0';		// in real module i haven't do it, but here on stepik ...

	sol_st->buf[0] = sol_st->cur_session + '0';
	filp->f_pos ++;
	sol_st->last_byte = 1;

	return 0;
}

static int sol_release(struct inode *inode, struct file *filp)
{
	struct sol_st *sol_st;

	sol_st = filp->private_data;
	if (sol_st->buf != NULL)
		kfree(sol_st->buf);
	if (sol_st->f_read != NULL)
		kfree(sol_st->f_read);
	kfree(sol_st);

	return 0;
}

ssize_t sol_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	struct sol_st *solst;
	int digit_count = 0;
	unsigned long buf_strlen;
	int i_buf;
	int ret_count = 0;

	solst = filp->private_data;

	if (*pos >= solst->buf_size - 1)		// buffer = 255, w. \0 -> 256
		return 0;

	if (count + *pos >= solst->buf_size - 1)
		count = solst->buf_size - *pos - 1;

	if (!solst->read_flag) {
		put_user(solst->buf[0], buf);
		solst->read_flag = 1;
		//solst->buf[0] = '\0';
		//*pos = 0;
		return 1;
	}

	buf_strlen = strlen(solst->buf + *pos);
	if (!buf_strlen)
		return buf_strlen;

	copy_to_user(buf, solst->buf + *pos, buf_strlen);

	*pos += buf_strlen;

	return buf_strlen;
}

ssize_t sol_write(struct file *filp, const char __user *buf, size_t count, loff_t *pos)
{
	struct sol_st *solst;

	solst = filp->private_data;

	if (*pos >= solst->buf_size - 1)		// no free space
		return -1;

	if (count + *pos >= solst->buf_size - 1)
		count = solst->buf_size - *pos - 1;

	copy_from_user(solst->buf + *pos, buf, count);
	*pos = strlen(solst->buf);

	return count;
} 

loff_t sol_llseek(struct file *filp, loff_t offset, int whence)
{
	struct sol_st *sol_st;
	loff_t newpos;
	unsigned long len = 0;

	sol_st = filp->private_data;
	len = strlen(sol_st->buf + filp->f_pos);
	
	switch(whence) {
	case 0: /* SEEK_SET */
		newpos = offset;
		break;
	case 1: /* SEEK_CUR */
		newpos = filp->f_pos + offset;
		break;
	case 2: /* SEEK_END */
		newpos = filp->f_pos + len + offset;
		break;
	default: 
		return -EINVAL;
	}

	if (newpos < 0) 
		return -EINVAL;

	filp->f_pos = newpos;
	return newpos;
}

struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = sol_open,
	.read = sol_read,
	.write = sol_write,
	.llseek = sol_llseek,
	.release = sol_release,
};

static int __init sol_init(void)
{
	int ret;

	if (sol_major == 240) {
		sdev.devt = MKDEV(sol_major, sol_minor);
		ret = register_chrdev_region(sdev.devt, 1, DRIVER_NAME);
	} else {
		ret = alloc_chrdev_region(&sdev.devt, sol_minor, 1, DRIVER_NAME);
		sol_major = MAJOR(sdev.devt);
	}

	if (ret < 0)
		return ret;

	cdev_init(&sdev.cdev, &fops);
	sdev.cdev.owner = THIS_MODULE;
	ret = cdev_add(&sdev.cdev, sdev.devt, 1);
	if (ret < 0)
		return ret;

	mutex_init(&sdev.sol_mutex);

	return 0;
}

static void __exit sol_exit(void)
{
	cdev_del(&sdev.cdev);
	unregister_chrdev_region(sdev.devt, 1);
}

module_init(sol_init);
module_exit(sol_exit);

MODULE_LICENSE("GPL");
