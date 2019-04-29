/*
*****************************************************************************
**
**  File        : stm32f2xx_flash_main.ld
**
**  Abstract    : Linker script for STM32F205VET Device with
**                512KByte FLASH, 128KByte SRAM
**
**                Set heap size, stack size and stack location according
**                to application requirements.
**
**                Set memory bank area and size if external memory is used.
**
**  Target      : STMicroelectronics STM32
**
**  Environment : GCC Eclipse
**
*****************************************************************************
*/

/* Entry Point */
ENTRY(Reset_Handler)

/* Highest address of the user mode stack */
_estack = 0x20020000;    /* end of 128K SRAM */

/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200;  /* required amount of heap  */
_Min_Stack_Size = 0x200; /* required amount of stack */

/* Specify the memory areas */
/* STM32F2 has upto 12 sectors: 0=16k, 1=16k, 2=16k, 3=16k, 4=64k, 5=128k, 6=128k, 7=128k, 8=128k, 9=128k, 10=128k, 11=128k */
/* STM32F205VET has 512k, Boot code is in first 16k, Main code is in remainder */      
MEMORY
{
  FLASH (rx)      : ORIGIN = 0x08004000, LENGTH = (512K - 16k)
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}

_vRamTop = ORIGIN( RAM ) + LENGTH( RAM );

/* Variables used by FreeRTOS-MPU. */
/*_Privileged_Functions_Region_Size = 16K;*/
/*_Privileged_Data_Region_Size = 512;*/
_MPU_Function_Region_Size = 4k;
_MPU_Data_Region_Size = 256;

__FLASH_segment_start__ = ORIGIN( FLASH );
__FLASH_segment_end__ = __FLASH_segment_start__ + LENGTH( FLASH );

/*__privileged_functions_start__ = ORIGIN( FLASH );*/
/*__privileged_functions_end__ = __privileged_functions_start__ + _Privileged_Functions_Region_Size;*/

__SRAM_segment_start__ = ORIGIN( RAM );
__SRAM_segment_end__ = __SRAM_segment_start__ + LENGTH( RAM );

/*__privileged_data_start__ = ORIGIN( RAM );*/
/*__privileged_data_end__ = ORIGIN( RAM ) + _Privileged_Data_Region_Size;*/


/* Define output sections */
SECTIONS
{
	/* The vector table goes into FLASH at address 0 */
	.isr_vector :
	{
	    . = ALIGN(4);
    	KEEP(*(.isr_vector))
	} >FLASH

	/* Privileged section at the start of the flash - vectors must be first	whatever */
	privileged_functions ALIGN(256) :
	{
		__privileged_functions_start__ = .;
		. = ALIGN(4);
		*(privileged_functions)
		. = ALIGN(_MPU_Function_Region_Size);
		__privileged_functions_end__ = .;
	} >FLASH
	_Privileged_Functions_Region_Size = __privileged_functions_end__ - __privileged_functions_start__;

	. = ALIGN(4);
	/* The program code and other data goes into FLASH */
	.text :
	{
		. = ALIGN(4);
		*(.text)           /* .text sections (code) */
		*(.text*)          /* .text* sections (code) */
		*(.rodata)         /* .rodata sections (constants, strings, etc.) */
		*(.rodata*)        /* .rodata* sections (constants, strings, etc.) */
		*(.glue_7)         /* glue arm to thumb code */
		*(.glue_7t)        /* glue thumb to arm code */

		KEEP (*(.init))
		KEEP (*(.fini))

		. = ALIGN(4);
		_etext = .;        /* define a global symbols at end of code */
	} >FLASH

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} >FLASH
	
	.ARM :
	{
		__exidx_start = .;
		*(.ARM.exidx*)
		__exidx_end = .;
	} >FLASH

	.ARM.attributes :
	{
		*(.ARM.attributes)
	} >FLASH

	.preinit_array     :
	{
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP (*(.preinit_array*))
		PROVIDE_HIDDEN (__preinit_array_end = .);
	} >FLASH
	
	.init_array :
	{
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP (*(SORT(.init_array.*)))
		KEEP (*(.init_array*))
		PROVIDE_HIDDEN (__init_array_end = .);
	} >FLASH
	
	.fini_array :
	{
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP (*(.fini_array*))
		KEEP (*(SORT(.fini_array.*)))
		PROVIDE_HIDDEN (__fini_array_end = .);
  	} >FLASH

	/* used by the startup to initialize data */
	_sidata = ALIGN(4);
  
	. = ALIGN(256);
	privileged_data : AT ( _sidata )
	{
		. = ALIGN(256);
		_sdata = .;        /* create a global symbol at data start */
		__privileged_data_start__ = .;
		. = ALIGN(4);
		*(privileged_data)
		. = ALIGN(_MPU_Data_Region_Size);
		__privileged_data_end__ = .;
	} >RAM
	_Privileged_Data_Region_Size = __privileged_data_end__ - __privileged_data_start__;
	
	. = ALIGN(4);
  	/* Initialized data sections goes into RAM, load LMA copy after code */
	.data : AT ( _sidata + _Privileged_Data_Region_Size )
	{
		. = ALIGN(4);
		/*_sdata = .; */       /* create a global symbol at data start */
		*(.data)           /* .data sections */
		. = ALIGN(4);
		*(.data*)          /* .data* sections */
		*(.ramsection) /* location of ramsection */
	. = ALIGN(4);
		_edata = .;        /* define a global symbol at data end */
	} >RAM

	/* Uninitialized data section */
	. = ALIGN(4);
	.bss :
	{
		/* This is used by the startup in order to initialize the .bss secion */
		_sbss = .;         /* define a global symbol at bss start */
		__bss_start__ = _sbss;
		*(.bss)
		*(.bss*)
		*(COMMON)

		. = ALIGN(4);
		_ebss = .;         /* define a global symbol at bss end */
		__bss_end__ = _ebss;
	} >RAM

	PROVIDE ( end = _ebss );
	PROVIDE ( _end = _ebss );

	/* User_heap_stack section, used to check that there is enough RAM left */
	._user_heap_stack :
	{
		. = ALIGN(4);
		. = . + _Min_Heap_Size;
		. = . + _Min_Stack_Size;
		. = ALIGN(4);
	} >RAM

	/* Remove information from the standard libraries */
	/DISCARD/ :
	{
		libc.a ( * )
		libm.a ( * )
		libgcc.a ( * )
	}
}
