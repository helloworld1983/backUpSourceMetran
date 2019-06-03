/********************************************************************/
/*                                                                  */
/* File Name    : updateFlash.h                                		*/
/*                                                                  */
/* General      : Provide function to update firmware files         */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/25       Thao Ha		  	    new file        */
/* #0001       2017/09/21       Viet Le				modify			*/
/*		Edit updateFlash_CheckFirwareFiles() function ((bug 1847))	*/
/* #0002       2017/09/28		Viet Le				modify			*/
/*		- Move #define from updateFlash.c to updateFlash.h			*/
/*		- Move getBootloaderInfo() to updateFlash and changed name  */
/*			to updateFlash_GetBootloaderInfo()						*/
/********************************************************************/
#ifndef INC_UPDATEFLASH_H_
#define INC_UPDATEFLASH_H_

#include <string.h>
#include <stdio.h>
#include <board.h>
#include "ff.h"
#include "iap_18xx_43xx.h"
#include "UsbMgr.h"
#include "readElf.h"
#include "checksum.h"

#define USER_START_SECTOR_M4 	10	//sector 10 is start sector of user application (0x1A07 0000)
#define MAX_USER_SECTOR_M4 		14
#define USER_START_SECTOR_M0	0
#define MAX_USER_SECTOR_M0 		14
#define BYTE_WRITE			512
#define SPIFLASH_BASE_ADDRESS 0x14000000
#define FLASH_BANK_A	0x1A000000
#define FLASH_BANK_B	0x1B000000
#define FLASH_SPI		0x14000000
#define CTRL_SW_ADDR	0X1A030000
#define FIRMWARE_NAME_ARRAY_SIZE 		40
#define FIRMWARE_PREFIX_NAME_SIZE 		19
#define BOOTLOADER_PREFIX_NAME_SIZE 	21
#define FIRMWARE_NAME_M4_STRING			"m4_upgrade_FH-310_V"
#define FIRMWARE_NAME_M0_STRING			"m0_upgrade_FH-310_V"
#define FIRMWARE_NAME_BOOTLOADER_STRING	"boot_upgrade_FH-310_V"

struct image_sig {
	uint32_t signature;		/* Signature M0/M4 image */
	int8_t build_date[32];	/* Build Date string */
	int8_t build_time[32];	/* Build Time string */
	int8_t version[32];
};

extern FATFS fatFS;

bool updateFlash_CheckFirwareFiles(int8_t *filenameM0,int8_t *filenameM4);
bool updateFlash_Update(int8_t *filename);
bool updateFlash_UpdateBankA(int8_t *filename,Elf32_Phdr* Phdr);
bool updateFlash_UpdateBankB(int8_t *filename,Elf32_Phdr* Phdr);
bool updateFlash_CheckBootloaderFile(int8_t* fileName);
bool updateFlash_CheckBootloaderFileRTOS(int8_t* fileName);
uint8_t Chip_IAP_init(void);
//get Bootloader Information
void updateFlash_GetBootloaderInfo(uint32_t image_addr,int8_t* version,int8_t* buildDate);

#endif /* INC_UPDATEFLASH_H_ */
