/********************************************************************/
/*                                                                  */
/* File Name    : updateFlash.c                                		*/
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
/*             2017/08/24      Linh Nguyen		  	modify		    */
/*  	       add the LCD print to handle update bootloader failure*/
/*             2017/08/30      Linh Nguyen		  	modify		    */
/*  	       modify code to pass coding rule						*/
/* #0004  	   2017/09/01		Quyen Ngo(MV)		modify			*/
/*		-add {} to if() condition									*/
/* #0005	   2017/09/11		Quyen Ngo(MV)		modify			*/
/*		-remove pointless "if" branch in updateFlash.c(M4) Line 424,*/
/*		587 (bug 1827)												*/
/* #0006	   2017/09/11	   Quyen Ngo(MV)			modify		*/
/*	   -change arument of f_mount function for adapt with new update*/
/*		of fatfs R0.11a												*/
/* #0007       2017/09/19		Viet Le				modify			*/
/*		- Remove: s_m4_filename,s_m0_filename					    */
/*		  Edit g_boot_filename->s_boot_filename						*/
/*		  Edit updateFlash_CheckFirwareFiles() function 		    */
/*		  Edit update_checkFirmwareELFFiles() function 	(bug 1847)  */
/* #0008       2017/09/27	   Quyen Ngo				moddify		*/
/*	    - Add wdt feed when upgrdade boot LD						*/
/* #0009       2017/09/28		Viet Le				modify			*/
/*		- Move #define from updateFlash.c to updateFlash.h			*/
/*		- Move getBootloaderInfo() to updateFlash and changed name  */
/*			to updateFlash_GetBootloaderInfo()						*/
/********************************************************************/
#include "updateFlash.h"
#include "iap_18xx_43xx.h"
#include "stdlib.h"
#include "GUI.h"
#include "board.h"
#include "GuiInterface.h"
#include "lpc_swim_font.h"

#define ELF_STRING_SIZE 				3
#define CRC_SIZE 						4
#define FIRMWARE_NAME_SIZE 				32
#define FIRMWARE_NAME_ARRAY_SIZE 		40
#define PERCENT_STRING_SIZE 			3
#define ELF_STRING_BYTE 				1
#define ELF_STRING_SIZE 				3
#define CHIP_IAP_COMMAND_SIZE 			5
#define CHIP_IAP_RESULT_SIZE 			4
#define CHIP_IAP_COMMAND_INIT 			49
#define FLASH_BANK_MASK 				0xFF000000
#define BOOTLOADER_NAME_SIZE 			34
#define START_BOOTLOADER_SECTOR_BANK_A 	0
#define END_BOOTLOADER_SECTOR_BANK_A 	6
#define START_BOOTLOADER_SECTOR_BANK_B 	12
#define END_BOOTLOADER_SECTOR_BANK_B 	13
#define MAX_TEXT 						100

#define CHECKSUM_STRING_SIZE 			9

enum
{
	eFlashBankA,
	eFlashBankB,
	eFlashSPIF,
	eNumberOfFlashBank
};

FATFS fatFS;
static uint32_t g_Size[eNumberOfFlashBank];
static int8_t s_boot_filename[FIRMWARE_NAME_ARRAY_SIZE];
uint8_t Buff[BYTE_WRITE];

extern SWIM_WINDOW_T upgradeScreen;

WCHAR ff_convert (WCHAR wch, UINT dir)
{
	if (wch < 0x80) {
		/* ASCII Char */
		return wch;
	}

	/* I don't support unicode it is too big! */
	return 0;
}

WCHAR ff_wtoupper (WCHAR wch)
{
	if (wch < 0x80) {
		/* ASCII Char */
		if (wch >= 'a' && wch <= 'z') {
			wch &= ~0x20;
		}
		return wch;
	}
	/* I don't support unicode it is too big! */
	return 0;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Check if this file is an ELF file 				                		    */
/*                                                                                      */
/* Details  -Check if this file is an ELF file 											*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *filenameM0 : M0 file name									*/
/*                 int8_t *filenameM4 : M4 file name                                    */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
static bool update_checkFirmwareELFFiles(int8_t *filenameM0,int8_t *filenameM4)//(void)
{
	Elf32_Ehdr Ehdr={'\0'};
	bool error = readElfhdr(filenameM4,&Ehdr);
	if(error == false)
	{
		int8_t str[4];
		memcpy (str,&Ehdr.e_ident[ELF_STRING_BYTE],ELF_STRING_SIZE);
		str[ELF_STRING_SIZE]='\0';
		if(strcmp((char*)str,"ELF") == 0)
		{
			error = readElfhdr(filenameM0,&Ehdr);
			if(error == false)
			{
				int8_t str[ELF_STRING_SIZE + 1]={'\0'};
				memcpy (str,&Ehdr.e_ident[ELF_STRING_BYTE],ELF_STRING_SIZE);
				if(strcmp((char*)str,"ELF") == 0)
				{
					return error;
				}
				else
				{
					error = true;
				}
			}
			else
			{
				error= true;
			}
		}
		else
		{
			error = true;
		}
	}
	else
	{
		error = true;
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Check if the usb contains valid firmware files                    		    */
/*                                                                                      */
/* Details  -Check the existence of firmware files, checksum of files and 				*/
/* 			 whether they are ELF files  												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t *filenameM0 : M0 file name									*/
/*                 int8_t *filenameM4 : M4 file name                                    */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool updateFlash_CheckFirwareFiles(int8_t *filenameM0,int8_t *filenameM4)
{
	bool error = false;
	DIR dir={'\0'};
	FILINFO fno;
	static TCHAR lfname[_MAX_LFN];
	f_mount(&fatFS,"",0);
	FRESULT rc = f_opendir(&dir, "");
	uint32_t m4Size =0;
	uint32_t m0Size =0;
	if (rc) {
		error = true;
	}
	else
	{
		int m4_exist=0;
		int m0_exist=0;
		for (;; )
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			fno.lfname = lfname;
			fno.lfsize = _MAX_LFN - 1;
			rc = f_readdir(&dir, &fno);

			if (rc || !fno.fname[0])
			{
				break;
			}
			if (fno.fattrib & AM_DIR)
			{
				DEBUGOUT("   <dir>  %s\r\n", fno.fname);
			}
			else
			{
				if(strlen(fno.lfname)==FIRMWARE_NAME_SIZE)
				{
					int8_t temp[FIRMWARE_NAME_ARRAY_SIZE]={'\0'};
					strncpy((char*)temp,fno.lfname,FIRMWARE_PREFIX_NAME_SIZE);
					if(strcmp((char*)temp,FIRMWARE_NAME_M0_STRING)==0)
					{
						m0_exist++;
						if(m0_exist > 1)
						{
							return true;
						}
						strncpy((char*)filenameM0,fno.lfname,FIRMWARE_NAME_SIZE);
						m0Size = fno.fsize;
					}
					if(strcmp((char*)temp,FIRMWARE_NAME_M4_STRING)==0)
					{
						m4_exist++;
						if(m4_exist > 1)
						{
							return true;
						}
						strncpy((char*)filenameM4,fno.lfname,FIRMWARE_NAME_SIZE);
						m4Size = fno.fsize;
					}
				}
			}
		}

		int8_t m0FirstVersion[7] = {'\0'};
		int8_t m0LastVersion[4] = {'\0'};

		int8_t m4FirstVersion[7] = {'\0'};
		int8_t m4LastVersion[4] = {'\0'};

		for(int i = 18; i < 24; i++)
		{
			m4FirstVersion[i - 18] = filenameM4[i];
			m0FirstVersion[i - 18] = filenameM0[i];
		}

		for(int i = 25; i < 28; i++)
		{
			m4LastVersion[i - 25] = filenameM4[i];
			m0LastVersion[i - 25] = filenameM0[i];
		}

		int32_t firstError = strcmp((char*)m4FirstVersion,(char*)m0FirstVersion);
		int32_t lastError = strcmp((char*)m4LastVersion,(char*)m0LastVersion);

		if(firstError != 0 || lastError != 0) //check version of m0 &m4 are same
		{
			error = true;
		}
		else if(checksum_File(filenameM4,m4Size-CRC_SIZE).error)
		{
			error =true;
		}
		else if (checksum_File(filenameM0,m0Size-CRC_SIZE).error)
		{
			error = true;
		}
		else
		{
			error = update_checkFirmwareELFFiles(filenameM0,filenameM4);
		}
	}

	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Chip_IAP_init									                   		    */
/*                                                                                      */
/* Details  -Chip IAP init																*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
uint8_t Chip_IAP_init(void)
{
	uint32_t command[CHIP_IAP_COMMAND_SIZE]={'\0'};
	uint32_t result[CHIP_IAP_RESULT_SIZE]={'\0'};
	command[0] = CHIP_IAP_COMMAND_INIT;
	iap_entry(command, result);
	return result[0];
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Update Firmware													 	    */
/*                                                                                      */
/* Details  -Copy data from AXF file to Flash									 		*/
/* Arguments : (I) int8_t* filename														*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool updateFlash_Update(int8_t* filename)
{
	f_mount(&fatFS,"",0);
	Elf32_Ehdr Ehdr={'\0'};
	Elf32_Phdr Phdr;
	bool error = false;
	error = readElfhdr(filename,&Ehdr);
	if(error == true)
	{
		error = true;
	}
	else
	{
		if(strcmp((char*)filename,(char*)s_boot_filename)==0)
		{
			Chip_IAP_init();
			Chip_IAP_PreSectorForReadWrite(START_BOOTLOADER_SECTOR_BANK_A, END_BOOTLOADER_SECTOR_BANK_A,  IAP_FLASH_BANK_A);
			Chip_IAP_EraseSector(START_BOOTLOADER_SECTOR_BANK_A, END_BOOTLOADER_SECTOR_BANK_A, IAP_FLASH_BANK_A);
		}

		for(int i=0;i < Ehdr.e_phnum;i++)
		{
			error = readElfPhdr(filename,&Phdr,Ehdr.e_phoff+i*Ehdr.e_phentsize,Ehdr.e_phentsize);
			if(error == true)
			{
				break;
			}
			else
			{
				bool errorFl = false;
				uint32_t Fl = Phdr.p_paddr & FLASH_BANK_MASK;
				switch(Fl)
				{
				case FLASH_BANK_A:
					errorFl = updateFlash_UpdateBankA(filename,&Phdr);
					g_Size[0]+=Phdr.p_memsz;
					DEBUGOUT("Size of Bank A %d \n",g_Size[eFlashBankA]);
					break;
				case FLASH_BANK_B:
					errorFl = updateFlash_UpdateBankB(filename,&Phdr);
					g_Size[1]+=Phdr.p_memsz;
					DEBUGOUT("Size of Bank B %d \n",g_Size[eFlashBankA]);
					break;
				default:
					DEBUGOUT("Belong to RAM\n");
					break;
				}
				if(errorFl == true)
				{
					error= true;
					break;
				}
			}
		}
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Copy data from AXF file to Bank A									 	    */
/*                                                                                      */
/* Details  -Copy data from AXF file to Bank A									 		*/
/* Arguments : (I) int8_t* filename														*/
/* 			   (I) Elf32_Phdr* Phdr            											*/
/* 			   (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool updateFlash_UpdateBankB(int8_t *filename,Elf32_Phdr* Phdr)
{
	static int count=0;
	if(count ==0)
	{
		__disable_irq();
		Chip_IAP_init();
		Chip_IAP_PreSectorForReadWrite(START_BOOTLOADER_SECTOR_BANK_B, END_BOOTLOADER_SECTOR_BANK_B,  IAP_FLASH_BANK_B);
		Chip_IAP_EraseSector(START_BOOTLOADER_SECTOR_BANK_B, END_BOOTLOADER_SECTOR_BANK_B, IAP_FLASH_BANK_B);
		__enable_irq();

	}
	count++;
	DEBUGOUT("Update Flash Bank A\n");
	bool error = false;
	FIL file;
	if (f_open(&file, (char*)filename, FA_READ) == FR_OK)
	{
		DEBUGOUT("Open file OK\n");
		DEBUGOUT("Start writing \n");
		FRESULT errf;
		int32_t remainByte;
		remainByte = Phdr->p_filesz;
		errf = f_lseek(&file,Phdr->p_offset);

		if (errf != FR_OK)
		{
			DEBUGOUT("Seek Elf program header failed %d \n",errf);
			error = true;
		}
		else
		{
			uint32_t address = Phdr->p_paddr;
			uint32_t diff=0;
			if(address%BYTE_WRITE != 0)
			{
				diff = address%BYTE_WRITE;
				address -= address%BYTE_WRITE;
			}
			uint32_t DownloadBytes=0;
			uint8_t Per=0;
			while (remainByte>0)
			{
				Chip_WWDT_Feed(LPC_WWDT);
				int32_t r = BYTE_WRITE;

				if(remainByte <= r)
				{
					r=remainByte;
				}
				for(int i = 0; i < BYTE_WRITE;i++)
				{
					Buff[i]=0;
				}
				if(diff!=0)
				{
					for(int i = 0; i<diff;i++)
					{
						Buff[i]=*(uint8_t *)address;
						address++;
					}
					address -= diff;
					__disable_irq();
					Chip_IAP_PreSectorForReadWrite(START_BOOTLOADER_SECTOR_BANK_B, END_BOOTLOADER_SECTOR_BANK_B,  IAP_FLASH_BANK_B);
					uint8_t result=Chip_IAP_ErasePage(address,address+BYTE_WRITE);
					if(result != IAP_CMD_SUCCESS)
					{
						DEBUGOUT("Erase page failed %d\n",result);
						error = true;
						break;
					}
					__enable_irq();
					r = BYTE_WRITE-diff;
				}
				if (r >= remainByte)
					r = remainByte;
				errf=f_read(&file, &Buff[diff], (UINT)r, (UINT*)&r);
				if (errf != FR_OK)
				{
					DEBUGOUT("Read failed %d \n",errf);
					error = true;
					break;
				}
				__disable_irq();
				Chip_IAP_PreSectorForReadWrite(START_BOOTLOADER_SECTOR_BANK_B, END_BOOTLOADER_SECTOR_BANK_B,  IAP_FLASH_BANK_B);
				uint8_t result = Chip_IAP_CopyRamToFlash(address, (uint32_t*)Buff, BYTE_WRITE);
				__enable_irq();
				if(result != IAP_CMD_SUCCESS)
				{
					DEBUGOUT("Write failed %d\n",result);
					error = true;
					break;
				}
				DownloadBytes +=r;
				Per = ((double)DownloadBytes/(double)Phdr->p_filesz)*100.0;
				int8_t bufferInt[4] = {'\0'};
				int8_t str[100] = "Bank B: update processing ... ";
				int8_t str1[50] = "%\n";
				itoa(Per,(char*)&bufferInt[0],10);
				if(strlen((char*)bufferInt) + 1 >
							(sizeof(str) - strlen((char*)str)))
				{
					DEBUGOUT("error cat");
				}
				else{
					strncat((char*)str, (char*)bufferInt, 4);
				}

				if(strlen((char*)str1) + 1 >
						(sizeof(str) - strlen((char*)str)))
				{
					DEBUGOUT("error cat");
				}
				else
				{
					strncat((char*)str,(char*)str1,50);
				}

				if(diff != 0)
				{
					address += BYTE_WRITE;
					diff=0;
				}
				else
				{
					address += r;
				}
				remainByte -= r;
			}
		}
		f_close(&file);
	}

	if(error == false)
	{
		DEBUGOUT("Write to Bank B success \n");
	}
	else
	{
		DEBUGOUT("Write to Bank B Failed \n");
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Copy data from AXF file to Bank A									 	    */
/*                                                                                      */
/* Details  -Copy data from AXF file to Bank A									 		*/
/* Arguments : (I) int8_t* filename														*/
/* 			   (I) Elf32_Phdr* Phdr            											*/
/* 			   (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool updateFlash_UpdateBankA(int8_t *filename,Elf32_Phdr* Phdr)
{
	static int count=0;
	if(count ==0)
	{
		__disable_irq();
		Chip_IAP_init();
		Chip_IAP_PreSectorForReadWrite(START_BOOTLOADER_SECTOR_BANK_A, END_BOOTLOADER_SECTOR_BANK_A,  IAP_FLASH_BANK_A);
		Chip_IAP_EraseSector(START_BOOTLOADER_SECTOR_BANK_A, END_BOOTLOADER_SECTOR_BANK_A, IAP_FLASH_BANK_A);
		__enable_irq();

	}
	count++;
	DEBUGOUT("Update Flash Bank A\n");
	bool error = false;
	FIL file;
	if (f_open(&file, (char*)filename, FA_READ) == FR_OK)
	{
		DEBUGOUT("Open file OK\n");
		DEBUGOUT("Start writing \n");
		FRESULT errf;
		int32_t remainByte;
		remainByte = Phdr->p_filesz;
		errf = f_lseek(&file,Phdr->p_offset);

		if (errf != FR_OK)
		{
			DEBUGOUT("Seek Elf program header failed %d \n",errf);
			error = true;
		}
		else
		{
			uint32_t address = Phdr->p_paddr;
			uint32_t diff=0;
			if(address%BYTE_WRITE != 0)
			{
				diff = address%BYTE_WRITE;
				address -= address%BYTE_WRITE;
			}
			uint32_t DownloadBytes=0;
			uint8_t Per=0;

			while (remainByte>0)
			{
				Chip_WWDT_Feed(LPC_WWDT);
				int32_t r = BYTE_WRITE;

				if(remainByte <= r)
				{
					r=remainByte;
				}
				for(int i = 0; i < BYTE_WRITE;i++)
				{
					Buff[i]=0;
				}
				if(diff!=0)
				{
					for(int i = 0; i<diff;i++)
					{
						Buff[i]=*(uint8_t *)address;
						address++;
					}
					address -= diff;
					__disable_irq();
					Chip_IAP_PreSectorForReadWrite(START_BOOTLOADER_SECTOR_BANK_A, END_BOOTLOADER_SECTOR_BANK_A,  IAP_FLASH_BANK_A);
					uint8_t result=Chip_IAP_ErasePage(address,address+BYTE_WRITE);
					if(result != IAP_CMD_SUCCESS)
					{
						DEBUGOUT("Erase page failed %d\n",result);
						error = true;
						break;
					}
					__enable_irq();
					r = BYTE_WRITE-diff;
				}
				if (r >= remainByte)
				{
					r = remainByte;
				}
				errf=f_read(&file, &Buff[diff], (UINT)r, (UINT*)&r);
				if (errf != FR_OK)
				{
					DEBUGOUT("Read failed %d \n",errf);
					error = true;
					break;
				}
				__disable_irq();
				Chip_IAP_PreSectorForReadWrite(START_BOOTLOADER_SECTOR_BANK_A, END_BOOTLOADER_SECTOR_BANK_A,  IAP_FLASH_BANK_A);
				uint8_t result = Chip_IAP_CopyRamToFlash(address, (uint32_t*)Buff, BYTE_WRITE);
				__enable_irq();
				if(result != IAP_CMD_SUCCESS)
				{
					DEBUGOUT("Write failed %d\n",result);
					error = true;
					break;
				}
				DownloadBytes +=r;
				Per = ((double)DownloadBytes/(double)Phdr->p_filesz)*100.0;
				int8_t bufferInt[PERCENT_STRING_SIZE+1] = {'\0'};
				int8_t str[100] = "Bank A: update processing ... ";
				int8_t str1[50] = "%\n";
				itoa(Per,(char*)&bufferInt[0],10);
				if(strlen((char*)bufferInt) + 1 >
							(sizeof(str) - strlen((char*)str)))
				{
					DEBUGOUT("error cat");
				}
				else{
					strncat((char*)str, (char*)bufferInt,4);
				}

				if(strlen((char*)str1) + 1 >
						(sizeof(str) - strlen((char*)str)))
				{
					DEBUGOUT("error cat");
				}
				else
				{
					strncat((char*)str,(char*)str1,50);
				}
				if(diff != 0)
				{
					address += BYTE_WRITE;
					diff=0;
				}
				else
				{
					address += r;
				}
				remainByte -= r;
			}
		}
		f_close(&file);
	}

	if(error == false)
	{
		DEBUGOUT("Write to Bank A success \n");
	}
	else
	{
		DEBUGOUT("Write to Bank A Failed \n");
	}
	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-Check if the usb contains Bootloader file	                    		    */
/*                                                                                      */
/* Details  -Check the existence of Bootloader file, checksum of file and 			    */
/* 			 whether it  is ELF file	  												*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool updateFlash_CheckBootloaderFile(int8_t* fileName)
{
	bool error = false;
	DIR dir={'\0'};
	FILINFO fno;
	int8_t BootLDName[BOOTLOADER_NAME_SIZE] = {0};
	uint32_t BootLDSize;
	static TCHAR lfname[_MAX_LFN];

	f_mount(&fatFS,"",0);

	FRESULT rc = f_opendir_nonRTOS(&dir, "");

	IMAGE_CHECK_RESULT_t result;

	if (rc)
	{
		error = true;
		swim_put_newline(&upgradeScreen);
		swim_put_text(&upgradeScreen,"Fail: [E2] USB memory access error");
		swim_put_newline(&upgradeScreen);
		swim_put_text(&upgradeScreen,"Please remove the USB and insert again with update files!");
	}
	else
	{
		int bootloader_exist=0;
		for (;; )
		{
			Chip_WWDT_Feed(LPC_WWDT);
			fno.lfname = lfname;
			fno.lfsize = _MAX_LFN - 1;
			rc = f_readdir(&dir, &fno);
			if (rc || !fno.fname[0])
			{
				break;
			}
			if (fno.fattrib & AM_DIR)
			{
				DEBUGOUT("   <dir>  %s\r\n", fno.fname);
			}
			else
			{
				if(strlen(fno.lfname)==BOOTLOADER_NAME_SIZE)
				{
					int8_t temp[BOOTLOADER_PREFIX_NAME_SIZE+1] = {'\0'};

					strncpy((char*)temp,(char*)fno.lfname,BOOTLOADER_PREFIX_NAME_SIZE);
					if(strcmp((char*)temp,FIRMWARE_NAME_BOOTLOADER_STRING)==0)
					{
						bootloader_exist++;
						strncat((char*)BootLDName,fno.lfname,BOOTLOADER_NAME_SIZE);
						BootLDSize = fno.fsize;
					}
				}
			}
		}
		if(!(bootloader_exist==1))
		{
			error = true;
			swim_put_newline(&upgradeScreen);
			swim_put_text(&upgradeScreen,"Fail: [E5] Illegal file format");
			swim_put_newline(&upgradeScreen);
			swim_put_text(&upgradeScreen,"Error Information: Can't find firmware files");
			swim_put_newline(&upgradeScreen);
			swim_put_text(&upgradeScreen,"Please remove the USB and insert again with update files!");
		}
		else
		{
			result =checksum_File(BootLDName,BootLDSize-CRC_SIZE);
			if(!result.error)
			{
				strncpy((char*)s_boot_filename,(char*)BootLDName,BOOTLOADER_NAME_SIZE);

				Elf32_Ehdr Ehdr={'\0'};
				error = readElfhdr(s_boot_filename,&Ehdr);
				if(error == false)
				{
					int8_t str[ELF_STRING_SIZE+1]={'\0'};
					memcpy (str,&Ehdr.e_ident[1],ELF_STRING_SIZE);
					if(strcmp((char*)str,"ELF") == 0)
					{
						strncpy((char*)fileName,(char*)s_boot_filename,BOOTLOADER_NAME_SIZE);
					}
					else
					{
						error = true;
						swim_put_newline(&upgradeScreen);
						swim_put_text(&upgradeScreen,"Fail: [E5] Illegal file format");
						swim_put_newline(&upgradeScreen);
						swim_put_text(&upgradeScreen,"Error Information: Can't find firmware files");
						swim_put_newline(&upgradeScreen);
						swim_put_text(&upgradeScreen,"Please remove the USB and insert again with update files!");
					}

				}
			}
			else
			{
				error = true;
				swim_put_newline(&upgradeScreen);
				swim_put_text(&upgradeScreen,"Fail: [E1] Checksum Error");
				swim_put_newline(&upgradeScreen);
				char str[MAX_TEXT]="Error Information: [0x";

				char calChecksum_char[CHECKSUM_STRING_SIZE]={'\0'};
				itoa(result.checksumCalcultation,calChecksum_char,16);

				char readCheckSum_char[CHECKSUM_STRING_SIZE]={'\0'};
				itoa(result.checksumValue,readCheckSum_char,16);

				strncat(str, readCheckSum_char,CHECKSUM_STRING_SIZE);
				strcat(str, "] [0x");

				strncat(str,readCheckSum_char,CHECKSUM_STRING_SIZE);
				strcat(str,"]");
				swim_put_text(&upgradeScreen,str);
				swim_put_newline(&upgradeScreen);
				swim_put_text(&upgradeScreen,"Please remove the USB and insert again with update files!");
			}
		}
	}
	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-updateFlash_CheckBootloaderFileRTOS	                    		    	*/
/*                                                                                      */
/* Details  -Check bootloader file when RTOS running						 			*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) int8_t* fileName														*/
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : Bool		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool updateFlash_CheckBootloaderFileRTOS(int8_t* fileName)//result
{
	bool error = false;
	DIR dir={'\0'};
	FILINFO fno;
	static TCHAR lfname[_MAX_LFN];

	f_mount(&fatFS,"",0);

	FRESULT rc = f_opendir(&dir, "");
	if (rc) {
		error = true;
	}
	else
	{
		int bootloader_exist=0;
		for (;; )
		{
			fno.lfname = lfname;
			fno.lfsize = _MAX_LFN - 1;
			rc = f_readdir(&dir, &fno);
			if (rc || !fno.fname[0])
			{
				break;
			}
			if (fno.fattrib & AM_DIR)
			{
				DEBUGOUT("   <dir>  %s\r\n", fno.fname);
			}
			else
			{
				if(strlen(fno.lfname)==BOOTLOADER_NAME_SIZE)
				{
					int8_t temp[BOOTLOADER_PREFIX_NAME_SIZE+1] = {'\0'};

					strncpy((char*)temp,fno.lfname,BOOTLOADER_PREFIX_NAME_SIZE);
					if(strcmp((char*)temp,FIRMWARE_NAME_BOOTLOADER_STRING)==0)
					{
						if(!checksum_File((int8_t*)fno.lfname,fno.fsize-CRC_SIZE).error)
						{
							bootloader_exist++;
							if(bootloader_exist >1)
							{
								return true;
							}
							strncpy((char*)fileName,fno.lfname,BOOTLOADER_NAME_SIZE);
						}
					}
				}
			}
		}

		Elf32_Ehdr Ehdr={'\0'};
		error = readElfhdr(fileName,&Ehdr);
		if(error == false)
		{
			int8_t str[ELF_STRING_SIZE+1];
			memcpy (str,&Ehdr.e_ident[1],ELF_STRING_SIZE);
			str[ELF_STRING_SIZE]='\0';
			if(strcmp((char*)str,"ELF") == 0)
			{
				return error;
			}
			else
			{
				error=true;
			}
		}
	}

	return error;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-updateFlash_GetBootloaderInfo											 	*/
/*                                                                                      */
/* Details  -get Bootloader Information												 	*/
/* Arguments : (I) uint32_t image_addr													*/
/* 			   (O) char* version														*/
/*			   (O) char* buildDate	                                                    */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
void updateFlash_GetBootloaderInfo(uint32_t image_addr,int8_t* version,int8_t* buildDate)
{
	const uint32_t *addr = (uint32_t *) image_addr +8;
	const struct image_sig *img;
	addr++;
	img = (const struct image_sig *) *addr;
	strncpy((char*)version,(char*)img->version,sizeof(img->version));
	strncpy((char*)buildDate,(char*)img->build_date,sizeof(img->build_date));
}

/* end of files */
