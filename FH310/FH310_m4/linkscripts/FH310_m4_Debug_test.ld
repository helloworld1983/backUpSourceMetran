/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2017
 * Generated linker script file for LPC4357
 * Created from linkscript.ldt by FMCreateLinkLibraries
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on Aug 14, 2017 4:36:46 PM
 */

INCLUDE "FH310_m4_Debug_library.ld"
INCLUDE "FH310_m4_Debug_memory.ld"

ENTRY(ResetISR)

SECTIONS
{
  
    
    /* _core_m0app text section */
    .text.core_m0app_text : SUBALIGN(4)
    {
       FILL(0xff)
       __core_m0app_START__ = .; /* start of slave image */
       KEEP(*(.core_m0app))
    }> MFlashB512

    /* _core_m0app extab and exidx sections */
    .text.core_m0app_ARM_extab . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.ARM.extab))
    } 

    .text.core_m0app_ARM_exidx . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.ARM.exidx))
    } 

    /* _core_m0app data section */
    .text.core_m0app_data . : SUBALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.core_m0app.data_*)) KEEP(*(.core_m0app.data))
        __core_m0app_END__ = .; /* end of slave image */

        /* perform some simple sanity checks */
        ASSERT(!(__core_m0app_START__ == __core_m0app_END__), "No slave code for _core_m0app");
        ASSERT( (ABSOLUTE(__core_m0app_START__) == __vectors_start___core_m0app), "M0APP execute address differs from address provided in source image");
    } 
    .text_Flash2 : ALIGN(4)
    {
       FILL(0xff)

    	*(.text_Flash2*) /* for compatibility with previous releases */
    	*(.text_MFlashB512*) /* for compatibility with previous releases */
    	*(.text.$Flash2*)
    	*(.text.$MFlashB512*)

    	*(.rodata.$Flash2*)
    	*(.rodata.$MFlashB512*)
    } > MFlashB512

  
    .text_Flash3 : ALIGN(4)
    {
       FILL(0xff)

        *spifidata/*(.text*)
    	*(.text_Flash3*) /* for compatibility with previous releases */
    	*(.text_SPIFlash*) /* for compatibility with previous releases */
    	*(.text.$Flash3*)
    	*(.text.$SPIFlash*)

        *spifidata/*(.rodata*)
    	*(.rodata.$Flash3*)
    	*(.rodata.$SPIFlash*)
    } > SPIFlash

    /* MAIN TEXT SECTION */
    .text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        /* Global Section Table */
        . = ALIGN(4) ; 
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));
        LONG(LOADADDR(.data_RAM2));
        LONG(    ADDR(.data_RAM2));
        LONG(  SIZEOF(.data_RAM2));
        LONG(LOADADDR(.data_RAM3));
        LONG(    ADDR(.data_RAM3));
        LONG(  SIZEOF(.data_RAM3));
        LONG(LOADADDR(.data_RAM4));
        LONG(    ADDR(.data_RAM4));
        LONG(  SIZEOF(.data_RAM4));
        LONG(LOADADDR(.data_RAM5));
        LONG(    ADDR(.data_RAM5));
        LONG(  SIZEOF(.data_RAM5));
        LONG(LOADADDR(.data_RAM6));
        LONG(    ADDR(.data_RAM6));
        LONG(  SIZEOF(.data_RAM6));
        LONG(LOADADDR(.data_RAM7));
        LONG(    ADDR(.data_RAM7));
        LONG(  SIZEOF(.data_RAM7));
        LONG(LOADADDR(.data_RAM8));
        LONG(    ADDR(.data_RAM8));
        LONG(  SIZEOF(.data_RAM8));
        LONG(LOADADDR(.data_RAM9));
        LONG(    ADDR(.data_RAM9));
        LONG(  SIZEOF(.data_RAM9));
        LONG(LOADADDR(.data_RAM10));
        LONG(    ADDR(.data_RAM10));
        LONG(  SIZEOF(.data_RAM10));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        LONG(    ADDR(.bss_RAM2));
        LONG(  SIZEOF(.bss_RAM2));
        LONG(    ADDR(.bss_RAM3));
        LONG(  SIZEOF(.bss_RAM3));
        LONG(    ADDR(.bss_RAM4));
        LONG(  SIZEOF(.bss_RAM4));
        LONG(    ADDR(.bss_RAM5));
        LONG(  SIZEOF(.bss_RAM5));
        LONG(    ADDR(.bss_RAM6));
        LONG(  SIZEOF(.bss_RAM6));
        LONG(    ADDR(.bss_RAM7));
        LONG(  SIZEOF(.bss_RAM7));
        LONG(    ADDR(.bss_RAM8));
        LONG(  SIZEOF(.bss_RAM8));
        LONG(    ADDR(.bss_RAM9));
        LONG(  SIZEOF(.bss_RAM9));
        LONG(    ADDR(.bss_RAM10));
        LONG(  SIZEOF(.bss_RAM10));
        __bss_section_table_end = .;
        __section_table_end = . ;
	    /* End of Global Section Table */

        *(.after_vectors*)

        /* Code Read Protection data */
        . = 0x000002FC ;
        PROVIDE(__CRP_WORD_START__ = .) ;
        KEEP(*(.crp))
        PROVIDE(__CRP_WORD_END__ = .) ;
        ASSERT(!(__CRP_WORD_START__ == __CRP_WORD_END__), "Linker CRP Enabled, but no CRP_WORD provided within application");
        /* End of Code Read Protection */
    } >MFlashA512

    .text : ALIGN(4)    
    {
        *(.text*)
        *(.rodata .rodata.* .constdata .constdata.*)
        . = ALIGN(4);
    } > MFlashA512
    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this. 
     */
    .ARM.extab : ALIGN(4) 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MFlashA512
    __exidx_start = .;

    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MFlashA512
    __exidx_end = .;

    _etext = .;
        
    /* DATA section for RamLoc40 */
  
    .data_RAM2 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM2 = .) ;
        *(.ramfunc.$RAM2)
        *(.ramfunc.$RamLoc40)
        *(.data.$RAM2*)
        *(.data.$RamLoc40*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM2 = .) ;
     } > RamLoc40 AT>MFlashA512

    /* DATA section for RamAHB32 */
  
    .data_RAM3 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM3 = .) ;
        *(.ramfunc.$RAM3)
        *(.ramfunc.$RamAHB32)
        *(.data.$RAM3*)
        *(.data.$RamAHB32*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM3 = .) ;
     } > RamAHB32 AT>MFlashA512

    /* DATA section for RamAHB16 */
  
    .data_RAM4 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM4 = .) ;
        *(.ramfunc.$RAM4)
        *(.ramfunc.$RamAHB16)
        *(.data.$RAM4*)
        *(.data.$RamAHB16*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM4 = .) ;
     } > RamAHB16 AT>MFlashA512

    /* DATA section for RamAHB_ETB16 */
  
    .data_RAM5 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM5 = .) ;
        *(.ramfunc.$RAM5)
        *(.ramfunc.$RamAHB_ETB16)
        *(.data.$RAM5*)
        *(.data.$RamAHB_ETB16*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM5 = .) ;
     } > RamAHB_ETB16 AT>MFlashA512

    /* DATA section for RAM_EXT_16_6144 */
  
    .data_RAM6 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM6 = .) ;
        *(.ramfunc.$RAM6)
        *(.ramfunc.$RAM_EXT_16_6144)
        *(.data.$RAM6*)
        *(.data.$RAM_EXT_16_6144*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM6 = .) ;
     } > RAM_EXT_16_6144 AT>MFlashA512

    /* DATA section for RAM_EXT_16_4096_2048 */
  
    .data_RAM7 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM7 = .) ;
        *(.ramfunc.$RAM7)
        *(.ramfunc.$RAM_EXT_16_4096_2048)
        *(.data.$RAM7*)
        *(.data.$RAM_EXT_16_4096_2048*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM7 = .) ;
     } > RAM_EXT_16_4096_2048 AT>MFlashA512

    /* DATA section for RAM_EXT_16_8192 */
  
    .data_RAM8 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM8 = .) ;
        *(.ramfunc.$RAM8)
        *(.ramfunc.$RAM_EXT_16_8192)
        *(.data.$RAM8*)
        *(.data.$RAM_EXT_16_8192*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM8 = .) ;
     } > RAM_EXT_16_8192 AT>MFlashA512

    /* DATA section for RAM_EXT_16_8192_4096 */
  
    .data_RAM9 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM9 = .) ;
        *(.ramfunc.$RAM9)
        *(.ramfunc.$RAM_EXT_16_8192_4096)
        *(.data.$RAM9*)
        *(.data.$RAM_EXT_16_8192_4096*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM9 = .) ;
     } > RAM_EXT_16_8192_4096 AT>MFlashA512

    /* DATA section for RAM_EXT_16_8192_4096_2048 */
  
    .data_RAM10 : ALIGN(4)
    {
        FILL(0xff)
        PROVIDE(__start_data_RAM10 = .) ;
        *(.ramfunc.$RAM10)
        *(.ramfunc.$RAM_EXT_16_8192_4096_2048)
        *(.data.$RAM10*)
        *(.data.$RAM_EXT_16_8192_4096_2048*)
        . = ALIGN(4) ;
        PROVIDE(__end_data_RAM10 = .) ;
     } > RAM_EXT_16_8192_4096_2048 AT>MFlashA512

    /* MAIN DATA SECTION */
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
        . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > RamLoc32
    /* Main DATA section (RamLoc32) */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       *(vtable)
       *(.ramfunc*)
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
    } > RamLoc32 AT>MFlashA512
    /* BSS section for RamLoc40 */
    .bss_RAM2 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM2 = .) ;
       *(.bss.$RAM2*)
       *(.bss.$RamLoc40*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM2 = .) ;
    } > RamLoc40 
    /* BSS section for RamAHB32 */
    .bss_RAM3 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM3 = .) ;
       *(.bss.$RAM3*)
       *(.bss.$RamAHB32*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM3 = .) ;
    } > RamAHB32 
    /* BSS section for RamAHB16 */
    .bss_RAM4 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM4 = .) ;
       *(.bss.$RAM4*)
       *(.bss.$RamAHB16*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM4 = .) ;
    } > RamAHB16 
    /* BSS section for RamAHB_ETB16 */
    .bss_RAM5 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM5 = .) ;
       *(.bss.$RAM5*)
       *(.bss.$RamAHB_ETB16*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM5 = .) ;
    } > RamAHB_ETB16 
    /* BSS section for RAM_EXT_16_6144 */
    .bss_RAM6 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM6 = .) ;
       *(.bss.$RAM6*)
       *(.bss.$RAM_EXT_16_6144*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM6 = .) ;
    } > RAM_EXT_16_6144 
    /* BSS section for RAM_EXT_16_4096_2048 */
    .bss_RAM7 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM7 = .) ;
       *(.bss.$RAM7*)
       *(.bss.$RAM_EXT_16_4096_2048*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM7 = .) ;
    } > RAM_EXT_16_4096_2048 
    /* BSS section for RAM_EXT_16_8192 */
    .bss_RAM8 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM8 = .) ;
       *(.bss.$RAM8*)
       *(.bss.$RAM_EXT_16_8192*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM8 = .) ;
    } > RAM_EXT_16_8192 
    /* BSS section for RAM_EXT_16_8192_4096 */
    .bss_RAM9 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM9 = .) ;
       *(.bss.$RAM9*)
       *(.bss.$RAM_EXT_16_8192_4096*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM9 = .) ;
    } > RAM_EXT_16_8192_4096 
    /* BSS section for RAM_EXT_16_8192_4096_2048 */
    .bss_RAM10 : ALIGN(4)
    {
       PROVIDE(__start_bss_RAM10 = .) ;
       *(.bss.$RAM10*)
       *(.bss.$RAM_EXT_16_8192_4096_2048*)
       . = ALIGN (. != 0 ? 4 : 1) ; /* avoid empty segment */
       PROVIDE(__end_bss_RAM10 = .) ;
    } > RAM_EXT_16_8192_4096_2048 
    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > RamLoc32
    /* NOINIT section for RamLoc40 */
    .noinit_RAM2 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM2*)
       *(.noinit.$RamLoc40*)
       . = ALIGN(4) ;
    } > RamLoc40 
    /* NOINIT section for RamAHB32 */
    .noinit_RAM3 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM3*)
       *(.noinit.$RamAHB32*)
       . = ALIGN(4) ;
    } > RamAHB32 
    /* NOINIT section for RamAHB16 */
    .noinit_RAM4 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM4*)
       *(.noinit.$RamAHB16*)
       . = ALIGN(4) ;
    } > RamAHB16 
    /* NOINIT section for RamAHB_ETB16 */
    .noinit_RAM5 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM5*)
       *(.noinit.$RamAHB_ETB16*)
       . = ALIGN(4) ;
    } > RamAHB_ETB16 
    /* NOINIT section for RAM_EXT_16_6144 */
    .noinit_RAM6 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM6*)
       *(.noinit.$RAM_EXT_16_6144*)
       . = ALIGN(4) ;
    } > RAM_EXT_16_6144 
    /* NOINIT section for RAM_EXT_16_4096_2048 */
    .noinit_RAM7 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM7*)
       *(.noinit.$RAM_EXT_16_4096_2048*)
       . = ALIGN(4) ;
    } > RAM_EXT_16_4096_2048 
    /* NOINIT section for RAM_EXT_16_8192 */
    .noinit_RAM8 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM8*)
       *(.noinit.$RAM_EXT_16_8192*)
       . = ALIGN(4) ;
    } > RAM_EXT_16_8192 
    /* NOINIT section for RAM_EXT_16_8192_4096 */
    .noinit_RAM9 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM9*)
       *(.noinit.$RAM_EXT_16_8192_4096*)
       . = ALIGN(4) ;
    } > RAM_EXT_16_8192_4096 
    /* NOINIT section for RAM_EXT_16_8192_4096_2048 */
    .noinit_RAM10 (NOLOAD) : ALIGN(4)
    {
       *(.noinit.$RAM10*)
       *(.noinit.$RAM_EXT_16_8192_4096_2048*)
       . = ALIGN(4) ;
    } > RAM_EXT_16_8192_4096_2048 
    /* DEFAULT NOINIT SECTION */
    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        *(.noinit*) 
         . = ALIGN(4) ;
        _end_noinit = .;
    } > RamLoc32

    PROVIDE(_pvHeapStart = ADDR(.data_RAM6));
	PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RamAHB16 - 0);		
    /*PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RAM_EXT_16_8192 - 0);*/
    /*PROVIDE(_pvHeapStart = DEFINED(__user_heap_base) ? __user_heap_base : .);	
    PROVIDE(_vStackTop = DEFINED(__user_stack_top) ? __user_stack_top : __top_RamLoc32 - 0);*/

    /* ## Create checksum value (used in startup) ## */
    PROVIDE(__valid_user_code_checksum = 0 - 
                                         (_vStackTop 
                                         + (ResetISR + 1) 
                                         + (NMI_Handler + 1) 
                                         + (HardFault_Handler + 1) 
                                         + (( DEFINED(MemManage_Handler) ? MemManage_Handler : 0 ) + 1)   /* MemManage_Handler may not be defined */
                                         + (( DEFINED(BusFault_Handler) ? BusFault_Handler : 0 ) + 1)     /* BusFault_Handler may not be defined */
                                         + (( DEFINED(UsageFault_Handler) ? UsageFault_Handler : 0 ) + 1) /* UsageFault_Handler may not be defined */
                                         ) );
}