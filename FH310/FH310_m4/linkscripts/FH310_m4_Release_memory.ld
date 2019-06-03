/*
 * GENERATED FILE - DO NOT EDIT
 * (c) Code Red Technologies Ltd, 2008-2013
 * (c) NXP Semiconductors 2013-2017
 * Generated linker script file for LPC4357
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.23
 * LPCXpresso v8.2.2 [Build 650] [2016-09-09]  on Oct 29, 2017 4:47:54 PM
 */

MEMORY
{
  /* Define each memory region */
  MFlashA512 (rx) : ORIGIN = 0x1a00e000, LENGTH = 0x72000 /* 456K bytes (alias Flash) */  
  MFlashB512 (rx) : ORIGIN = 0x1b000000, LENGTH = 0x80000 /* 512K bytes (alias Flash2) */  
  SPIFlash (rx) : ORIGIN = 0x14000000, LENGTH = 0x2000000 /* 32M bytes (alias Flash3) */  
  RamLoc32 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000 /* 32K bytes (alias RAM) */  
  RamLoc40 (rwx) : ORIGIN = 0x10080000, LENGTH = 0xa000 /* 40K bytes (alias RAM2) */  
  RamAHB32 (rwx) : ORIGIN = 0x20000000, LENGTH = 0x8000 /* 32K bytes (alias RAM3) */  
  RamAHB16 (rwx) : ORIGIN = 0x20008000, LENGTH = 0x4000 /* 16K bytes (alias RAM4) */  
  RamAHB_ETB16 (rwx) : ORIGIN = 0x2000c000, LENGTH = 0x4000 /* 16K bytes (alias RAM5) */  
  RAM_EXT_16_6144_6144 (rwx) : ORIGIN = 0x28000000, LENGTH = 0x600000 /* 6M bytes (alias RAM6) */  
  RAM_EXT_16_4096_2048 (rwx) : ORIGIN = 0x28600000, LENGTH = 0x200000 /* 2M bytes (alias RAM7) */  
  RAM_EXT_16_8192 (rwx) : ORIGIN = 0x28800000, LENGTH = 0x200000 /* 2M bytes (alias RAM8) */  
  RAM_EXT_16_8192_2048 (rwx) : ORIGIN = 0x28a00000, LENGTH = 0x200000 /* 2M bytes (alias RAM9) */  
  RAM_EXT_16_8192_4096 (rwx) : ORIGIN = 0x28c00000, LENGTH = 0x200000 /* 2M bytes (alias RAM10) */  
  RAM_EXT_16_8192_4096_2048 (rwx) : ORIGIN = 0x28e00000, LENGTH = 0x200000 /* 2M bytes (alias RAM11) */  
}

  /* Define a symbol for the top of each memory region */
  __base_MFlashA512 = 0x1a00e000  ; /* MFlashA512 */  
  __base_Flash = 0x1a00e000 ; /* Flash */  
  __top_MFlashA512 = 0x1a00e000 + 0x72000 ; /* 456K bytes */  
  __top_Flash = 0x1a00e000 + 0x72000 ; /* 456K bytes */  
  __base_MFlashB512 = 0x1b000000  ; /* MFlashB512 */  
  __base_Flash2 = 0x1b000000 ; /* Flash2 */  
  __top_MFlashB512 = 0x1b000000 + 0x80000 ; /* 512K bytes */  
  __top_Flash2 = 0x1b000000 + 0x80000 ; /* 512K bytes */  
  __base_SPIFlash = 0x14000000  ; /* SPIFlash */  
  __base_Flash3 = 0x14000000 ; /* Flash3 */  
  __top_SPIFlash = 0x14000000 + 0x2000000 ; /* 32M bytes */  
  __top_Flash3 = 0x14000000 + 0x2000000 ; /* 32M bytes */  
  __base_RamLoc32 = 0x10000000  ; /* RamLoc32 */  
  __base_RAM = 0x10000000 ; /* RAM */  
  __top_RamLoc32 = 0x10000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM = 0x10000000 + 0x8000 ; /* 32K bytes */  
  __base_RamLoc40 = 0x10080000  ; /* RamLoc40 */  
  __base_RAM2 = 0x10080000 ; /* RAM2 */  
  __top_RamLoc40 = 0x10080000 + 0xa000 ; /* 40K bytes */  
  __top_RAM2 = 0x10080000 + 0xa000 ; /* 40K bytes */  
  __base_RamAHB32 = 0x20000000  ; /* RamAHB32 */  
  __base_RAM3 = 0x20000000 ; /* RAM3 */  
  __top_RamAHB32 = 0x20000000 + 0x8000 ; /* 32K bytes */  
  __top_RAM3 = 0x20000000 + 0x8000 ; /* 32K bytes */  
  __base_RamAHB16 = 0x20008000  ; /* RamAHB16 */  
  __base_RAM4 = 0x20008000 ; /* RAM4 */  
  __top_RamAHB16 = 0x20008000 + 0x4000 ; /* 16K bytes */  
  __top_RAM4 = 0x20008000 + 0x4000 ; /* 16K bytes */  
  __base_RamAHB_ETB16 = 0x2000c000  ; /* RamAHB_ETB16 */  
  __base_RAM5 = 0x2000c000 ; /* RAM5 */  
  __top_RamAHB_ETB16 = 0x2000c000 + 0x4000 ; /* 16K bytes */  
  __top_RAM5 = 0x2000c000 + 0x4000 ; /* 16K bytes */  
  __base_RAM_EXT_16_6144_6144 = 0x28000000  ; /* RAM_EXT_16_6144_6144 */  
  __base_RAM6 = 0x28000000 ; /* RAM6 */  
  __top_RAM_EXT_16_6144_6144 = 0x28000000 + 0x600000 ; /* 6M bytes */  
  __top_RAM6 = 0x28000000 + 0x600000 ; /* 6M bytes */  
  __base_RAM_EXT_16_4096_2048 = 0x28600000  ; /* RAM_EXT_16_4096_2048 */  
  __base_RAM7 = 0x28600000 ; /* RAM7 */  
  __top_RAM_EXT_16_4096_2048 = 0x28600000 + 0x200000 ; /* 2M bytes */  
  __top_RAM7 = 0x28600000 + 0x200000 ; /* 2M bytes */  
  __base_RAM_EXT_16_8192 = 0x28800000  ; /* RAM_EXT_16_8192 */  
  __base_RAM8 = 0x28800000 ; /* RAM8 */  
  __top_RAM_EXT_16_8192 = 0x28800000 + 0x200000 ; /* 2M bytes */  
  __top_RAM8 = 0x28800000 + 0x200000 ; /* 2M bytes */  
  __base_RAM_EXT_16_8192_2048 = 0x28a00000  ; /* RAM_EXT_16_8192_2048 */  
  __base_RAM9 = 0x28a00000 ; /* RAM9 */  
  __top_RAM_EXT_16_8192_2048 = 0x28a00000 + 0x200000 ; /* 2M bytes */  
  __top_RAM9 = 0x28a00000 + 0x200000 ; /* 2M bytes */  
  __base_RAM_EXT_16_8192_4096 = 0x28c00000  ; /* RAM_EXT_16_8192_4096 */  
  __base_RAM10 = 0x28c00000 ; /* RAM10 */  
  __top_RAM_EXT_16_8192_4096 = 0x28c00000 + 0x200000 ; /* 2M bytes */  
  __top_RAM10 = 0x28c00000 + 0x200000 ; /* 2M bytes */  
  __base_RAM_EXT_16_8192_4096_2048 = 0x28e00000  ; /* RAM_EXT_16_8192_4096_2048 */  
  __base_RAM11 = 0x28e00000 ; /* RAM11 */  
  __top_RAM_EXT_16_8192_4096_2048 = 0x28e00000 + 0x200000 ; /* 2M bytes */  
  __top_RAM11 = 0x28e00000 + 0x200000 ; /* 2M bytes */  
