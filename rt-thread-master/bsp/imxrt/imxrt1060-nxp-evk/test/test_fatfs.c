#include <test_sem.h>
#include <rtthread.h>

#define DRV_DEBUG
#define LOG_TAG             "test.fatfs"
#include <drv_log.h>

#include <rtthread.h>
#include <dfs_posix.h>   /* This head file should be included when you need use file operations */

static void readwrite_sample(void)
{
    int fd, size;
    char s[] = "RT-Thread Programmer!", buffer[80];

    rt_kprintf("Write string %s to test.txt.\n", s);

    fd = open("/text.txt", O_WRONLY | O_CREAT);
    if (fd >= 0)
    {
        write(fd, s, sizeof(s));
        close(fd);
        rt_kprintf("Write done.\n");
    }

    fd = open("/text.txt", O_RDONLY);
    if (fd>= 0)
    {
        size = read(fd, buffer, sizeof(buffer));
        close(fd);
        rt_kprintf("Read from file test.txt : %s \n", buffer);
        if (size < 0)
            return ;
    }
}

MSH_CMD_EXPORT(readwrite_sample, readwrite sample);





























