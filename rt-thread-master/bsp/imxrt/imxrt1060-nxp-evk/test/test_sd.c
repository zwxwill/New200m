#include <test_sem.h>
#include <rtthread.h>

#define DRV_DEBUG
#define LOG_TAG             "test.sd"
#include <drv_log.h>

#define SD_DEVICE_NAME    "sd0"

#ifdef FINSH_USING_MSH
static void fill_buffer(rt_uint8_t *buff, rt_uint32_t buff_length)
{
    rt_uint32_t index;

    for (index = 0; index < buff_length; index++)
    {
        //buff[index] = ((rt_uint8_t)rand()) & 0xff;
		buff[index] = (index) & 0xff;
    }
}

static int sd_test(void)
{
	rt_err_t ret;
	rt_device_t sd_device;
	struct rt_device_blk_geometry geo;
	rt_uint8_t *write_buff, *read_buff;
	rt_uint8_t block_num;
	rt_uint8_t sec_num;
	
    sd_device = rt_device_find(SD_DEVICE_NAME);
    if (sd_device == RT_NULL)
    {
        rt_kprintf("find device %s failed!\n", SD_DEVICE_NAME);
        return RT_ERROR;
    }
	else
	{
		rt_kprintf("find device %s ok!\n", "sd0");
	}
	
    ret = rt_device_open(sd_device, RT_DEVICE_OFLAG_RDWR);
    if (ret != RT_EOK)
    {
        rt_kprintf("open device %s failed!\n", SD_DEVICE_NAME);
        return ret;
    }	
	else
	{
		rt_kprintf("open device %s ok!\n", SD_DEVICE_NAME);
	}
	
	
    ret = rt_device_control(sd_device, RT_DEVICE_CTRL_BLK_GETGEOME, &geo);
    if (ret != RT_EOK)
    {
        rt_kprintf("control device %s failed!\n", SD_DEVICE_NAME);
        return ret;
    }	
	else
	{
		rt_kprintf("control device %s ok!\n", SD_DEVICE_NAME);
	}
	
    rt_kprintf("device information:\n");
    rt_kprintf("sector  size : %d byte\n", geo.bytes_per_sector);
    rt_kprintf("sector count : %d \n", geo.sector_count);
    rt_kprintf("block   size : %d byte\n", geo.block_size);

	/* allocate memory */
    read_buff = rt_malloc_align(geo.block_size, 32);
    if (read_buff == RT_NULL)
    {
        rt_kprintf("no memory for read buffer!\n");
        return RT_ERROR;
    }
    write_buff = rt_malloc_align(geo.block_size, 32);
    if (write_buff == RT_NULL)
    {
        rt_kprintf("no memory for write buffer!\n");
        rt_free(read_buff);
        return RT_ERROR;
    }

	fill_buffer(write_buff, geo.block_size);
    block_num = rt_device_write(sd_device, 0, write_buff, 1);
    if (1 != block_num)
    {
        rt_kprintf("write device %s failed!\n", SD_DEVICE_NAME);
		return RT_ERROR;
    }	
	else
	{
		rt_kprintf("write device %s ok!\n", SD_DEVICE_NAME);
	}

    block_num = rt_device_read(sd_device, 0, read_buff, 1);
    if (1 != block_num)
    {
        rt_kprintf("read %s device failed!\n", SD_DEVICE_NAME);
		return RT_ERROR;
    }
	else
	{
		rt_kprintf("read %s device ok!\n", SD_DEVICE_NAME);
	}
	
    if (rt_memcmp(write_buff, read_buff, geo.block_size) == 0)
    {
        rt_kprintf("Block test OK!\n");
    }
    else
    {
        rt_kprintf("Block test Fail!\n");
    }	
	
    rt_free_align(read_buff);
    rt_free_align(write_buff);
	
	/***********************************************/
	/* allocate memory */
	sec_num = 5;
    read_buff = rt_malloc_align(geo.block_size * sec_num, 32);
    if (read_buff == RT_NULL)
    {
        rt_kprintf("no memory for read buffer!\n");
        return RT_ERROR;
    }
    write_buff = rt_malloc_align(geo.block_size * sec_num, 32);
    if (write_buff == RT_NULL)
    {
        rt_kprintf("no memory for write buffer!\n");
        rt_free(read_buff);
        return RT_ERROR;
    }

	fill_buffer(write_buff, geo.block_size * sec_num);
    block_num = rt_device_write(sd_device, 0, write_buff, sec_num);
    if (sec_num != block_num)
    {
        rt_kprintf("write device %s failed!\n", SD_DEVICE_NAME);
		return RT_ERROR;
    }	
	else
	{
		rt_kprintf("write device %s ok!\n", SD_DEVICE_NAME);
	}

    block_num = rt_device_read(sd_device, 0, read_buff,  sec_num);
    if (sec_num != block_num)
    {
        rt_kprintf("read %s device failed!\n", SD_DEVICE_NAME);
		return RT_ERROR;
    }
	else
	{
		rt_kprintf("read %s device ok!\n", SD_DEVICE_NAME);
	}
	
    if (rt_memcmp(write_buff, read_buff, geo.block_size) == 0)
    {
        rt_kprintf("Block test OK!\n");
    }
    else
    {
        rt_kprintf("Block test Fail!\n");
    }	
	
    rt_free_align(read_buff);
    rt_free_align(write_buff);	
	
	
    return RT_EOK;	
}
MSH_CMD_EXPORT(sd_test, sd device test);

#endif


