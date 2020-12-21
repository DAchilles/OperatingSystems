#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>

//当模块安装的时候执行
static int test_init(void)
{
        printk("init\n");
        return 0;
}
//当模块卸载的时候执行
static void test_exit(void)
{
        printk("exit\n");
        return ;
}
//注册这两个函数
module_init(test_init);
module_exit(test_exit);
//声明开源规则
MODULE_LICENSE("GPL");