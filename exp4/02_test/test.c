#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
 
#define MEM_CLEAR 0x01
#define CHAR_DEV_NAME "/dev/chr_dev"
 
int main()
{
    int ret;
    int fd;
    int index;
    char buf[32];

    /* open device */
    fd = open(CHAR_DEV_NAME, O_RDWR | O_NONBLOCK);
    if(fd < 0)
    {
        printf("open failed!\n");
        return -1;
    }

    /* set buffer data, which will be stored into device */
    for(index = 0; index < 32; index ++)
    {
        buf[index] = index;
    }

    /* write data */
    write(fd, buf, 32);
    memset(buf, 0, 32);

    /* read data */
    lseek(fd, 0, SEEK_SET);
    read(fd, buf, 32);
    for(index = 0; index < 32; index ++)
    {
        printf("data[%d] = %d\n", index, buf[index]);
    }

    /* reset all data to zero, read it and check whether it is ok */
    lseek(fd, 0, SEEK_SET);
    read(fd, buf, 32);
    for(index = 0; index < 32; index ++)
    {
        printf("data[%d] = %d\n", index, buf[index]);
    }

    close(fd);
    return 0;
}