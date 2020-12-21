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

#define MEM_SIZE    0x1000
#define MEM_CLEAR   0x1

static int my_major;
struct my_dev
{
    struct cdev cdev;
    unsigned char mem[MEM_SIZE];
};
struct my_dev* my_devp;

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
//static loff_t device_llseek(struct file* filp, loff_t offset, int orig);

static struct file_operations file_flops = {
    .owner    = THIS_MODULE,
    .open     = device_open,
    .release  = device_release,    
    .read     = device_read,
    .write    = device_write
    //.llseek   = device_llseek
};

//打开设备
static int device_open(struct inode *inode, struct file *file)
{
    filp->private_data = my_devp;
	return 0;
}

//释放设备
static int device_release(struct inode *inode, struct file *file)
{
    return  0;
}

//TODO:读信息
static ssize_t device_read(struct file *, char *, size_t, loff_t *)
{

}

//写信息
static ssize_t device_write(struct file *filp, const char *buf, size_t size, loff_t *ppos)
{
    int ret=0;
    unsigned long p = *ppos;
    unsigned int count = size;
    struct my_dev *dev = filp->private_data;

    if(p >= MEM_SIZE)
        return 0;
    
    if(count > MEM_SIZE-p)
        count = MEM_SIZE-p;
    
    if(copy_from_user(dev->mem+p, buf, count))
        ret = -EINVAL;
    else
    {
        *ppos += count;
        ret = count;
    }
    return ret;
}

//初始化设备
static void device_setup_cdev(struct my_dev* dev, int index)
{
    int devno = MKDEV(my_major, index);

    cdev_init(&dev->cdev, &file_flops);
    dev->cdev.owner = THIS_MODULE;

    int err=cdev_add(&dev->cdev, devno, 1);
    if(err)
    {
        printk(KERN_NOTICE "Error happend!\n");
    }   
}

//注册
static int device_init(void)
{
    int result;
    dev_t ndev;

    result = alloc_chrdev_region(&ndev, 0, 1, "my_dev");
    if(result < 0 )  
    {  
        return result;  
    }

    printk("device_init()\n\tmajor: %d\tminor: %d\n", MAJOR(ndev), MINOR(ndev));
    my_major = MAJOR(ndev);

    my_devp = kmalloc(sizeof(struct chr_dev), GFP_KERNEL);
	if(!my_devp)
	{
		result = -ENOMEM;
		goto END;
	}
	
	memset(my_devp, 0, sizeof(struct my_dev));
	my_setup_cdev(my_devp, 0);
	return 0;
	
END:
	unregister_chrdev_region(ndev, 1);
	return 0;
}

//退出
static void device_exit(void)
{
    cdev_del(&char_devp->cdev);
	kfree(char_devp);
	unregister_chrdev_region(MKDEV(chr_major, 0), 1);
}

module_init(device_init);
module_exit(device_exit);