#if 0






/* SDRAM is 32MByte Start address 0x80000000 */

#define m_flash_config_start           0x60000000  /* SPI Flash */
#define m_flash_config_size            0x00001000  /* 4K */

#define m_ivt_start                    0x60001000  /* SPI Flash */
#define m_ivt_size                     0x00001000  /* 4K */

#define m_interrupts_start             0x60002000  /* SPI Flash */
#define m_interrupts_size              0x00000400  /* 1K */

#define m_text_start                   0x60002400  /* SPI Flash */
#define m_text_size                    0x007FDC00  /* 8M - 1K - 4K - 4K */

#define m_data_start                   0x20000000  /* DTCM */
#define m_data_size                    0x00020000  /* 128K */

#define m_data2_start                  0x20200000  /* OCRAM */
#define m_data2_size                   0x00040000  /* 256K */

#define m_ncache_start                 0x81E00000  /* SDRAM */
#define m_ncache_size                  0x00200000  /* 2M */

/* Sizes */
#if (defined(__stack_size__))
	#define Stack_Size                   __stack_size__
#else
	#define Stack_Size                   0x1000
#endif

#if (defined(__heap_size__))
	#define Heap_Size                    __heap_size__
#else
	#define Heap_Size                    0x0400
#endif

#define RTT_HEAP_SIZE 0x800//(m_data_size-ImageLength(RW_m_data)-ImageLength(ARM_LIB_HEAP)-ImageLength(ARM_LIB_STACK))

#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
LR_m_text m_flash_config_start m_text_start+m_text_size-m_flash_config_start  ; load region size_region
{   
	RW_m_config_text m_flash_config_start FIXED m_flash_config_size ; load address = execution address
	{ 
		* (.boot_hdr.conf, +FIRST) 
	}

	RW_m_ivt_text m_ivt_start FIXED m_ivt_size  ; load address = execution address
	{ 
		* (.boot_hdr.ivt, +FIRST) 
		* (.boot_hdr.boot_data)
		* (.boot_hdr.dcd_data)
	}
#else
LR_m_text m_interrupts_start m_text_start+m_text_size-m_interrupts_start  ; load region size_region
{   
#endif
	VECTOR_ROM m_interrupts_start FIXED m_interrupts_size   ; load address = execution address
	{ 
		* (RESET,+FIRST)
	}
	
	ER_m_text m_text_start FIXED m_text_size  ; load address = execution address
	{ 
		* (InRoot$$Sections)
		.ANY (+RO)
	}
	
	RW_m_data m_data_start m_data_size-Stack_Size-Heap_Size-RTT_HEAP_SIZE ; RW data
	{ 
		.ANY (+RW +ZI)
		* (NonCacheable.init)
		* (NonCacheable)		
	}
  
  ARM_LIB_HEAP  +0 EMPTY Heap_Size {}  ; Heap region growing up    malloc用到的堆
  ARM_LIB_STACK +0 EMPTY Stack_Size{}  ; Stack region growing down  函数内部用的栈
  RTT_HEAP      +0 EMPTY RTT_HEAP_SIZE{}   /* Rt-Thread 的堆栈 */
  
  ; ncached
  RW_m_ncache m_ncache_start m_ncache_size
  {
	* (NonCacheable.init)
	* (NonCacheable)
  }
}











#endif


