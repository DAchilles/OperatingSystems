#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <asm/io.h>
#include <asm/switch_to.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
 
#define CHRMEM_SIZE 0x1000
#define MEM_CLEAR   0x1
 
static int chr_major;
 
struct chr_dev
{
	struct cdev cdev;
	unsigned char mem[CHRMEM_SIZE];
};
 
struct chr_dev* char_devp;
 
int chr_open(struct inode* inode, struct file* filp)
{
	filp->private_data = char_devp;
	return 0;
}
 
int chr_release(struct inode* inode, struct file* filp)
{
	return  0;
}
 
static ssize_t chr_read(struct file* filp, char __user* buf, size_t size, loff_t* ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct chr_dev* dev = filp->private_data;
	
	if(p >= CHRMEM_SIZE)
	{
		return 0;
	}
	
	if(count > CHRMEM_SIZE - p)
	{
		return 0;
	}
	
	if(copy_to_user(buf, (void*)(dev->mem + p), count))
	{
		return -EINVAL;
	}
	else
	{
		*ppos += count;
		ret = count;
	}
	
	return ret;
}
 
static ssize_t chr_write(struct file* filp, const char __user* buf, size_t size, loff_t *ppos)
{
	unsigned long p = *ppos;
	unsigned int count = size;
	int ret = 0;
	struct chr_dev* dev = filp->private_data;
	
	if(p >= CHRMEM_SIZE)
	{
		return 0;
	}
	
	if(count > CHRMEM_SIZE - p)
	{
		count = CHRMEM_SIZE - p;
	}
	
	if(copy_from_user(dev->mem + p, buf, count))
	{
		ret = -EINVAL;
	}
	else
	{
		*ppos += count;
		ret = count;
	}
	
	return ret;
}
 
static loff_t chr_llseek(struct file* filp, loff_t offset, int orig)
{
	loff_t ret = 0;
	
	/* orig can be SEEK_SET, SEEK_CUR, SEEK_END */
	switch(orig)
	{
		case 0:
			if(offset < 0)
			{
				ret = -EINVAL;
				break;
			}
			
			if((unsigned int) offset > CHRMEM_SIZE)
			{
				ret = -EINVAL;
				break;
			}
			
			filp->f_pos = (unsigned int) offset;
			ret = filp->f_pos;
			break;
			
		case 1:
			if((filp->f_pos + offset) > CHRMEM_SIZE)
			{
				ret = -EINVAL;
				break;
			}
			
			if((filp->f_pos + offset) < 0)
			{
				ret = -EINVAL;
				break;
			}
			
			filp->f_pos += offset;
			ret = filp->f_pos;
			break;
			
		default:
			ret = - EINVAL;
			break;
	}
	
	return ret;
}
 
static const struct file_operations chr_ops = 
{
	.owner    = THIS_MODULE,
	.llseek   = chr_llseek,
	.read     = chr_read,
	.write    = chr_write,
	.open     = chr_open,
	.release  = chr_release
};
 
static void chr_setup_cdev(struct chr_dev* dev, int index)
{
	int err;
	int devno = MKDEV(chr_major, index);
	
	cdev_init(&dev->cdev, &chr_ops);
	dev->cdev.owner = THIS_MODULE;
	
	err = cdev_add(&dev->cdev, devno, 1);
	if(err)
	{
		printk(KERN_NOTICE "Error happend!\n");
	}
}
 
int chr_init(void)
{
	int result;
	dev_t ndev;
 
	result = alloc_chrdev_region(&ndev, 0, 1, "chr_dev");  
    if(result < 0 )  
    {  
        return result;  
    } 	
	
	printk("chr_init(): major = %d, minor = %d\n", MAJOR(ndev), MINOR(ndev));
    chr_major = MAJOR(ndev);
	
	char_devp = kmalloc(sizeof(struct chr_dev), GFP_KERNEL);
	if(!char_devp)
	{
		result = -ENOMEM;
		goto final;
	}
	
	memset(char_devp, 0, sizeof(struct chr_dev));
	chr_setup_cdev(char_devp, 0);
	return 0;
	
final:
	unregister_chrdev_region(ndev, 1);
	return 0;
}
 
void chr_exit(void)
{
	cdev_del(&char_devp->cdev);
	kfree(char_devp);
	unregister_chrdev_region(MKDEV(chr_major, 0), 1);
}
 
module_init(chr_init);
module_exit(chr_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("feixiaoxing!163.com");
MODULE_DESCRIPTION("A simple device example!");