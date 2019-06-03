/********************************************************************/
/*                                                                  */
/* File Name    : updateFlash.c                                		*/
/*                                                                  */
/* General      : Provide function to update firmware files         */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/25       Thao Ha		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/
#include "updateFlash.h"
#include "ff.h"
#include "string.h"
#include "board.h"
#include "readElf.h"
#include "checksum.h"
#include "SPIFMgr.h"
#include "common.h"
#include  "Led.h"
#include "IRQHandle.h"
#include "common.h"
#include "boot.h"
#include "LogMgr.h"
#include "AudioMgr.h"
#include "UsbMgr.h"
#define USER_START_SECTOR_M4 	7
#define MAX_USER_SECTOR_M4 		14
#define FIRST_SPI_BLOCK			0
#define NUMBER_SPI_BLOCK_ERASE	200
#define FLASH_BANK_A 0x1A000000
#define BYTE_WRITE				4096
#define SIZE_OF_BANKA 0
#define SIZE_OF_SPIF 1
#define INVALID_FILE_ERROR 90
#define COPY_SPIF_ERROR 91
#define COPY_ROM_ERROR 92
extern FATFS fatFS;
extern SPIFI_HANDLE_T *pSpifi;
static DIR gs_dir={'\0'};
static FILINFO gs_fno;
static char gs_m4_filename[40];
static uint32_t gs_Size[2]={0};
static bool update_checkFirmwareFile();
static void update_checkNumberOfFirmwareFileExist(uint32_t* numberOfFirmwareFileExist,uint32_t* numberOfFirmwareFileValid);
static bool update_checkFirmwareFileIsELF();
static bool update_updateFlash(char* filename);
static uint8_t Chip_IAP_init(void);
static bool update_bankA(char *filename,Elf32_Phdr* Phdr);
static bool update_SPIF(char *filename,Elf32_Phdr* Phdr);
static E_Update_State gs_updateState = eNoneUpdate;
static uint8_t gs_versionH=0;
static uint8_t gs_versionL=0;
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_getUpdatedVersionH				              				*/
/*                                                                                      */
/* Details:    get Version(High)														*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
uint8_t update_getUpdatedVersionH(void)
{
	return gs_versionH;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_getUpdatedVersionL				              				*/
/*                                                                                      */
/* Details:    get Version(Low)															*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : uint8_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
uint8_t update_getUpdatedVersionL(void)
{
	return gs_versionL;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_getCurrentState					              				*/
/*                                                                                      */
/* Details:    get current update state													*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : E_Update_State	                                                        */
/*                                                                                      */
/****************************************************************************************/
E_Update_State update_getCurrentState()
{
	return gs_updateState;
}
WCHAR ff_convert (WCHAR wch, UINT dir)
{
	if (wch < 0x80) {
		/* ASCII Char */
		return wch;
	}
	return 0;
}

WCHAR ff_wtoupper (WCHAR wch)
{
	if (wch < 0x80) {
		if (wch >= 'a' && wch <= 'z') {
			wch &= ~0x20;
		}
		return wch;
	}
	return 0;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_bankA							              				*/
/*                                                                                      */
/* Details:    Update Flash bank A														*/
/*                                                                                      */
/* Arguments:	(I) char *filename														*/
/*    			(I) Elf32_Phdr* Phdr                                                    */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
static bool update_bankA(char *filename,Elf32_Phdr* Phdr)
{
	bool error = false;
	static FIL file;
	static bool s_isFirstUpdate = true;
	if(s_isFirstUpdate == true)
	{
		DEBUGOUT("Delete Bank A...");
		Chip_IAP_init();
		__disable_irq();
		Chip_IAP_PreSectorForReadWrite(USER_START_SECTOR_M4, MAX_USER_SECTOR_M4,  IAP_FLASH_BANK_A);
		uint8_t errIAP = Chip_IAP_EraseSector(USER_START_SECTOR_M4, MAX_USER_SECTOR_M4, IAP_FLASH_BANK_A);
		__enable_irq();
		if(errIAP != IAP_CMD_SUCCESS)
		{
			error = true;
			DEBUGOUT("Fail %d\n",errIAP);
		}
		s_isFirstUpdate = false;
	}
	if(error == false)
	{
		DEBUGOUT("Done\n");
		if(f_open(&file,filename,FA_READ)==FR_OK)
		{
			FRESULT result;
			int32_t remainBytes = 0;
			remainBytes = Phdr->p_filesz;
			result = f_lseek(&file,Phdr->p_offset);
			if(result == FR_OK)
			{
				uint32_t address = Phdr->p_paddr;
				uint32_t diff = 0;
				if((address % BYTE_WRITE)!=0)
				{
					diff = address % BYTE_WRITE;
					address -= address % BYTE_WRITE;
				}
				uint32_t downloadBytes = 0;
				//uint8_t percentage = 0;
				while(remainBytes > 0)
				{
					uint32_t readBytes = BYTE_WRITE;
					uint8_t Buff[BYTE_WRITE];
					if(remainBytes <= readBytes)
					{
						readBytes = remainBytes;
					}
					memset(Buff, 0, BYTE_WRITE);
					if(diff != 0)
					{
						for(int i = 0; i<diff; i++)
						{
							Buff[i]=*(uint8_t *)address;
							address++;
						}
						address -= diff;
						__disable_irq();
						Chip_IAP_PreSectorForReadWrite(USER_START_SECTOR_M4, MAX_USER_SECTOR_M4,  IAP_FLASH_BANK_A);
						Chip_IAP_ErasePage(address,address+(7*512));
						__enable_irq();
						readBytes = BYTE_WRITE-diff;
					}
//					if(readBytes >= remainBytes)
//					{
//						readBytes = remainBytes;
//					}
					result = f_read(&file, &Buff[diff], (UINT)readBytes, (UINT*)&readBytes);
					if(result != IAP_CMD_SUCCESS)
					{
						error = true;
						break;
					}
					__disable_irq();
					Chip_IAP_PreSectorForReadWrite(USER_START_SECTOR_M4, MAX_USER_SECTOR_M4,  IAP_FLASH_BANK_A);
					uint8_t resultRam = Chip_IAP_CopyRamToFlash(address, (uint32_t*)Buff, BYTE_WRITE);
					__enable_irq();
					if(resultRam != IAP_CMD_SUCCESS)
					{
						error = true;
						break;
					}
					downloadBytes += readBytes;
					//percentage = ((double)downloadBytes/(double)Phdr->p_filesz)*100.0;
					//					led_DisplayPercentValue(percentage);
					//DEBUGOUT("Percent Update Bank A %d\n",percentage);
					if(diff != 0)
					{
						address += BYTE_WRITE;
						diff = 0;
					}
					else
					{
						address += readBytes;
					}
					remainBytes -= readBytes;
				}
			}
			else
			{
				error = true;
			}
			f_close(&file);
		}
		else
		{
			error = true;
		}
	}
	return error;

}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_SPIF								              				*/
/*                                                                                      */
/* Details:    Update SPI Flash															*/
/*                                                                                      */
/* Arguments:	(I) char *filename														*/
/*    			(I) Elf32_Phdr* Phdr                                                    */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
static bool update_SPIF(char *filename,Elf32_Phdr* Phdr)
{

	bool error = false;
	SPIFI_ERR_T errSPIF = SPIFI_ERR_NONE;
	static FIL file;
	static bool s_isFirstUpdate = true;
	if(s_isFirstUpdate == true)
	{
		DEBUGOUT("Delete SPIF...");
		errSPIF = spifMgr_spifiErase(pSpifi,FIRST_SPI_BLOCK, NUMBER_SPI_BLOCK_ERASE);
		s_isFirstUpdate = false;
	}
	if(errSPIF!=SPIFI_ERR_NONE)
	{
		error=true;
	}
	else
	{
		DEBUGOUT("Done\n");
		if(f_open(&file,filename,FA_READ)==FR_OK)
		{
			FRESULT result;
			uint32_t readBytes = BYTE_WRITE;
			uint32_t remainBytes = 0;
			if(Phdr->p_filesz < Phdr->p_memsz)
			{
				remainBytes = Phdr->p_memsz;
			}
			else
			{
				remainBytes = Phdr->p_filesz;
			}
			result = f_lseek(&file,Phdr->p_offset);
			if(result == FR_OK)
			{
				uint32_t address = Phdr->p_paddr;
				uint32_t diff = 0;
				if(address % BYTE_WRITE != 0)
				{
					diff = address % BYTE_WRITE;
					address -= address % BYTE_WRITE;
				}
				uint32_t downloadBytes=0;
				//uint8_t percentage = 0;
				while(remainBytes > 0)
				{
					uint8_t Buff[BYTE_WRITE];
					if(remainBytes <= readBytes)
						readBytes = remainBytes;
					if(diff != 0)
					{
						for(int i = 0; i<diff; i++)
						{
							Buff[i]=*(uint8_t *)address;
							address++;
						}
						address -= diff;
					}
					memset(Buff,0,BYTE_WRITE);
					result=f_read(&file, &Buff[diff], (UINT)readBytes, (UINT*)&readBytes);
					if(result!=FR_OK)
					{
						error = true;
						break;
					}
					errSPIF= spifMgr_spifiProgram(pSpifi,address,(uint32_t*)Buff, BYTE_WRITE);
					if(errSPIF!=SPIFI_ERR_NONE)
					{
						error = true;
						break;
					}
					downloadBytes +=readBytes;
					//percentage = ((double)downloadBytes/(double)Phdr->p_filesz)*100.0;
					//DEBUGOUT("Percent Update SPIF %d\n",percentage);

					remainBytes -= readBytes;
					address += readBytes;
				}
			}
			else
			{
				error = true;
			}
			f_close(&file);
		}
		else
		{
			error = true;
		}
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_checkFirmwareFileIsELF			              				*/
/*                                                                                      */
/* Details:    Check whether firmware file is ELF file									*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*    			(O) None			                                                    */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
static bool update_checkFirmwareFileIsELF()
{
	bool error = false;
	char str[4]={"\0"};
	Elf32_Ehdr Ehdr={'\0'};
	error = readElfhdr(gs_m4_filename,&Ehdr);
	if(error == false)
	{
		memcpy (str,&Ehdr.e_ident[1],3);
		str[3]='\0';
		if(strcmp(str,"ELF") != 0)
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
/* Function name: 	update_checkFirmwareFile				              				*/
/*                                                                                      */
/* Details:    Check whether there is valid firmware file is USB						*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/*    			(O) None			                                                    */
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
static bool update_checkFirmwareFile()
{
	bool error = false;
	FRESULT result = f_opendir(&gs_dir,"");
	gs_updateState = eChecking;
	NVIC_EnableIRQ(TIMER3_IRQn);
	if(result)
	{
		error = true;
	}
	else
	{
		DEBUGOUT("File is Checking ...\n");
		uint32_t numberOfFirmwareFileValid = 0;
		uint32_t numberOfFirmwareFileExist = 0;
		update_checkNumberOfFirmwareFileExist(&numberOfFirmwareFileExist,&numberOfFirmwareFileValid);
		DEBUGOUT("number file exist.....%d\n",numberOfFirmwareFileExist);
		DEBUGOUT("number file valid.....%d\n",numberOfFirmwareFileValid);
		if(numberOfFirmwareFileValid==1)
		{
			error = update_checkFirmwareFileIsELF();
		}
		else
		{
			error = true;
		}
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_checkNumberOfFirmwareFileExist	              				*/
/*                                                                                      */
/* Details:    Count number of exist firmware file and valid firmware file in USB		*/
/*                                                                                      */
/* Arguments:	(O) uint32_t* numberOfFirmwareFileExist									*/
/*    			(O) uint32_t* numberOfFirmwareFileValid                                 */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void update_checkNumberOfFirmwareFileExist(uint32_t* numberOfFirmwareFileExist,uint32_t* numberOfFirmwareFileValid)
{
	static TCHAR lfname[_MAX_LFN];
	*numberOfFirmwareFileValid=0;
	*numberOfFirmwareFileExist=0;
	uint32_t numberOfFirmwareFileValidTemp=0;
	uint32_t numberOfFirmwareFileExistTemp=0;
	FRESULT result = FR_OK;
	for(;;)
	{
		gs_fno.lfname = lfname;
		gs_fno.lfsize = _MAX_LFN - 1;
		result = f_readdir(&gs_dir,&gs_fno);
		if((result!=FR_OK) || (result || !gs_fno.fname[0]))
		{
			break;
		}
		else
		{
			if(gs_fno.fattrib & AM_DIR)
			{

			}
			else
			{
				if(strlen(gs_fno.lfname)==29)
				{
					char temp[19]={"\0"};
					strncpy(temp,gs_fno.lfname,16);
					temp[17]='\0';
					if(strcmp(temp,"m4_upgrade_POR_V")==0)
					{
						numberOfFirmwareFileExistTemp++;
						uint32_t readChecksum=0;
						uint32_t calChecksum=0;
						if(!checksum_checkFile(gs_fno.lfname,gs_fno.fsize-4,&readChecksum,&calChecksum))
						{
							if(readChecksum==calChecksum)
							{
								numberOfFirmwareFileValidTemp++;
								strncpy(gs_m4_filename,gs_fno.lfname,32);
							}
							else
							{
								DEBUGOUT("Wrong Checksum\n");
							}
						}
					}
				}
			}
		}

	}
	*numberOfFirmwareFileExist=numberOfFirmwareFileExistTemp;
	*numberOfFirmwareFileValid=numberOfFirmwareFileValidTemp;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_updateFlash						              				*/
/*                                                                                      */
/* Details:    Update flash by copy the appropriate data from USB to appropriate Flash	*/
/*                                                                                      */
/* Arguments:	(I) char* filename														*/
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
static bool update_updateFlash(char* filename)
{
	Elf32_Ehdr Ehdr={'\0'};
	Elf32_Phdr Phdr={'\0'};
	bool error = false;
	error = readElfhdr(filename,&Ehdr);

	for(int i=0;i<Ehdr.e_phnum;i++)
	{
		error = readElfPhdr(filename,&Phdr,Ehdr.e_phoff+i*Ehdr.e_phentsize,Ehdr.e_phentsize);
		if(error == false)
		{
			uint32_t bank = Phdr.p_paddr & 0xFF000000;
			switch(bank)
			{
			case FLASH_BANK_A:
				DEBUGOUT("Bank A is updating...\n");
				error = update_bankA(filename,&Phdr);
				if(error == true)
				{
					POR_LOG_T log ={'\0'};
					log.logCode=eUpdateFileCopyErrorROMLogId;
					log.EventOrAlarm = eAlarmInspect;
					logMgr_GetCurrentSetting(&log.oldValue);
					logMgr_GetCurrentSetting(&log.newValue);
					logMgr_RecordLog(log);
					IRQ_timer0Disable();
					led_DisplayErrorCode(COPY_ROM_ERROR);
					led_TurnOnInspectLed();
				}
				gs_Size[SIZE_OF_BANKA]+=Phdr.p_memsz;
				break;
			case SPIF_BASE_ADDRESS:
				DEBUGOUT("SPIF is updating...\n");
				error = update_SPIF(filename,&Phdr);
				if(error == true)
				{
					IRQ_timer0Disable();
					led_DisplayErrorCode(COPY_SPIF_ERROR);
					led_TurnOnInspectLed();
				}
				gs_Size[SIZE_OF_SPIF]+=Phdr.p_memsz;
				break;
			default:
				break;
			}
			if(error == true)
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	Chip_IAP_init							              				*/
/*                                                                                      */
/* Details:    Init IAP chip															*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/* ReturnValue : uint8_t			                                                        */
/*                                                                                      */
/****************************************************************************************/
static uint8_t Chip_IAP_init(void)
{
	uint32_t command[5]={'\0'};
	uint32_t result[4]={'\0'};
	command[0] = 49;
	iap_entry(command, result);
	return result[0];
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_startUpdate						              				*/
/*                                                                                      */
/* Details:    Start Update firmware process											*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/* ReturnValue : bool			                                                        */
/*                                                                                      */
/****************************************************************************************/
bool update_startUpdate()
{
	usb_hw_setup();
	gs_updateState = eStartUpdate;
	bool error = false;
	f_mount(0, &fatFS);
	DEBUGOUT("Insert USB\n");
	IRQ_timer0Init();
	error = update_checkFirmwareFile();
	DEBUGOUT("error static %d\n", error);
	if(error == false)
	{
		gs_updateState = eUpdating;
		error = update_updateFlash(gs_m4_filename);
		if(error == false)
		{
			uint32_t CheckSum_BankA=checksum_Compute((uint8_t *)USER_PRO_ADDR_M4,gs_Size[SIZE_OF_BANKA]);
			uint32_t CheckSum_SPIF=checksum_ComputeSPIF(SPIF_BASE_ADDRESS,gs_Size[SIZE_OF_SPIF]);
			uint32_t CheckSum[2];
			CheckSum[0]=CheckSum_BankA;
			CheckSum[1]=CheckSum_SPIF;
			uint8_t resultFlash=checksum_WritetoFlash(CheckSum,gs_Size);
			if(resultFlash != IAP_CMD_SUCCESS)
			{
				error = true;
			}
		}
		else
		{
			error = true;
		}
		gs_updateState = eFinishUpdate;
		IRQ_timer0Disable();
		if(error == false)
		{
			spifMgr_unlock();
			boot_getControlSWVersion(USER_PRO_ADDR_M4,&gs_versionH,&gs_versionL);
			IRQ_timer1Init();
		}
	}
	else
	{

		POR_LOG_T log ={'\0'};
		log.logCode=eUpdateFileErrorLogId;
		log.EventOrAlarm = eAlarmInspect;
		logMgr_GetCurrentSetting(&log.oldValue);
		logMgr_GetCurrentSetting(&log.newValue);
		logMgr_RecordLog(log);
		IRQ_timer0Disable();
		led_DisplayErrorCode(INVALID_FILE_ERROR);
		led_TurnOnInspectLed();
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	update_runCtrlSW						              				*/
/*                                                                                      */
/* Details:    Jump to main control software at USER_PRO_ADDR_M4						*/
/*                                                                                      */
/* Arguments:	(I) None																*/
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void update_runCtrlSW()
{
	DEBUGOUT("Run ctrl sw \n");
	__disable_irq();
	uint32_t addr= USER_PRO_ADDR_M4;
	//Set vector table offset
	SCB->VTOR = addr;

	// reset pipeline, sync bus and memory access
	__asm ("dmb\n" "dsb\n" "isb\n");

	//reboot system at user program address
	asm("LDR SP, [%0]" : : "r"(addr));
	asm("LDR PC, [%0, #4]" : : "r"(addr));

}
