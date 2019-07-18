#if 0

1 rtthread spi flash 模式下 
Linker Misc controls 在env重新生成代码后需要在 Linker->Misc controls中添加
--remove 
--keep=*(.boot_hdr.ivt) 
--keep=*(.boot_hdr.boot_data) 
--keep=*(.boot_hdr.conf) 
--predefine="-DXIP_BOOT_HEADER_ENABLE=1"

CPU_MIMXRT1062DVL6A, XIP_EXTERNAL_FLASH=1, RT_USING_ARM_LIBC, XIP_BOOT_HEADER_ENABLE=1,FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1,

  rtthread_sdram / rtthread_ram 模式下
Linker Misc controls 在env重新生成代码后需要在 Linker->Misc controls中添加
--remove 
Utilities 中 Update Target before Debugging要取消勾选
C/C++ Define : CPU_MIMXRT1062DVL6A,FSL_SDK_ENABLE_DRIVER_CACHE_CONTROL=1,

2 Rt-thread program flow chart

	Reset_Handler          -- startup_MIMXRT1062.s
	
	int $Sub$$main(void)   -- components.c

	rtthread_startup(void) -- components.c

3 更改 ulog_deg.h 
  #define ULOG_NEWLINE_SIGN              "\r\n"
  改为
  #define ULOG_NEWLINE_SIGN              ""





#endif 

