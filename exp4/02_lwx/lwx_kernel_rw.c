#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("U201814883_lwx");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("3");
#define DEVICE_NAME "lwx_device"
#define MSG_BUFFER_LEN 100

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];

static char *msg_ptr_read;
static char *msg_ptr_write;

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
 .read = device_read,
 .write = device_write,
 .open = device_open,
 .release = device_release

};
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) 
{
    int bytes_read = 0, a=0,b=0,lena=0,lenb=0;
    char buf_a[20],buf_b[20]; 
    char *start=msg_ptr_read;
    printk("device_read\n");
    while (len && *msg_ptr_read &&(*msg_ptr_read!=' ')) 
    {
        a*=10;a+=(*msg_ptr_read-'0');
       msg_ptr_read++; len--;lena++;
    }
    if(*(msg_ptr_read))
    {
        strncpy(buf_a,start,lena);buf_a[lena]='.';buf_a[++lena]='\n';buf_a[++lena]=0;
        msg_ptr_read++; len--;
        start=msg_ptr_read;
        while((*msg_ptr_read)&&len&&(*msg_ptr_read)!='\n')
        {
            b*=10;b+=(*msg_ptr_read-'0');
            msg_ptr_read++;len--;lenb++;
        }
        strncpy(buf_b,start,lenb);buf_b[lenb]='.';buf_b[++lenb]='\n';buf_b[++lenb]=0;
        char buf[100];char *p=buf;
        strncpy(p, "Bigger num is  ", 20);
        if(a>b)  strncpy(p+15,buf_a,20);
        else if(a<b) strncpy(p+15,buf_b,20);
        else strncpy(p, "They are equal.\n", 20);

        bytes_read=strlen(p);
         while(*(p))    {
            put_user(*(p++), buffer++);
         }
    }
    return bytes_read;
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) 
{
    int bytes_write = 0; 
    printk("device_write\n");
    while(len)
    {
        get_user(*(msg_ptr_write++), buffer++);
        len--;
        bytes_write++;
    }
    return bytes_write;
}

static int device_open(struct inode *inode, struct file *file) 
{
    if (device_open_count) {    return -EBUSY; }
    device_open_count++;
    printk("\ndevice_open\n");
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) 
{

    device_open_count--;
    printk("device_release\n");
    printk("%d\n",device_open_count);
    module_put(THIS_MODULE);
    return 0;
}
static int __init lwx_kernel_rw_init(void) 
{   
    msg_ptr_write = msg_buffer;/* Set the msg_ptr to the buffer */
    msg_ptr_read = msg_buffer;
    major_num = register_chrdev(0,"lwx_device", &file_ops);/* Try to register character device */
    printk("\n\nstart.\n");
    if (major_num < 0) 
    { 
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    } 
    else 
    {
        printk(KERN_INFO "lwx_kernel_rw module loaded with device major number %d\n", major_num);
        return 0;
     }
}
static void __exit lwx_kernel_rw_exit(void) 
{
     /* Remember — we have to clean up after ourselves. Unregister the character device. */
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Goodbye, World!\n");
}
/* Register module functions */
module_init(lwx_kernel_rw_init);
module_exit(lwx_kernel_rw_exit);
