#if 0

1 Linker Misc controls 在env重新生成代码后需要在 Linker->Misc controls中添加
--remove 
--keep=*(.boot_hdr.ivt) 
--keep=*(.boot_hdr.boot_data) 
--keep=*(.boot_hdr.conf) 
--predefine="-DXIP_BOOT_HEADER_ENABLE=1"



2 Rt-thread program flow chart

	Reset_Handler          -- startup_MIMXRT1062.s
	
	int $Sub$$main(void)   -- components.c

	rtthread_startup(void) -- components.c








#endif 

