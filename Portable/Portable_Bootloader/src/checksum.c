/********************************************************************/
/*                                                                  */
/* File Name    : checksum.c                                        */
/*                                                                  */
/* General      : Provides fast and efficient way to compute a 		*/
/* 				  32 bit checksum values on a given sum of data 	*/
/*                bytes using pre-built checksum look-up tables. 	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/23      Thao Ha			  	new file        */
/*                                                                  */
/********************************************************************/
#include "checksum.h"
#include "ff.h"
#include "board.h"
#include "SPIFMgr.h"
#include "common.h"
#define BYTE_CHECKSUM_LOOP 	512
#define BYTE_CS_WRITE 		512
#define CS_ADDR				0x1B07fffc
#define FL_WR_ADDR			0x1B07fe00
#define CS_START_SECTOR	14
#define CS_END_SECTOR	14
#define NUMBER_OF_CHECKSUM_BYTE 4
#define NUMBER_OF_BYTE_READ_FROM_SPIF_PER_CYCLE 4096

extern SPIFI_HANDLE_T *pSpifi;
/****************************************************************************************/
/*                                                                                      */
/* General 	-checksum_checkFile								   							*/
/*                                                                                      */
/* Details  -Check whether this file is valid by calculate the check sum value and 		*/
/* 			 compare to the one in that file											*/
/* Arguments : (I) char* filename														*/
/* 			   (I) uint32_t size														*/
/*             (O) uint32_t* readValue													*/
/*			   (O) uint32_t* calValue													*/
/* ReturnValue : bool		                                                        	*/
/****************************************************************************************/
bool checksum_checkFile(char* filename,uint32_t size,uint32_t* readValue,uint32_t* calValue)
{
	static FIL file;
	bool error = false;
	FRESULT result;
	uint32_t checksum=0;
	if(f_open(&file,filename,FA_READ)==FR_OK)
	{
		uint32_t loopBytes = BYTE_CHECKSUM_LOOP;
		uint8_t Buff[BYTE_CHECKSUM_LOOP]={'\0'};
		uint32_t readBytes = 0;
		uint32_t remainBytes = size;
		uint32_t readBytesTemp = 0;
		while(remainBytes>0)
		{
			if(remainBytes >= loopBytes)
			{
				readBytes = loopBytes;
			}
			else
			{
				readBytes = remainBytes;
			}
			result = f_read(&file,Buff,(UINT)readBytes,(UINT*)&readBytesTemp);
			if(result != FR_OK)
			{
				error = true;
				break;
			}
			else
			{
				for(int i=0;i<readBytesTemp;i++)
				{
					checksum+=Buff[i];
				}
				remainBytes-=readBytesTemp;
			}
		}
		if(error == false)
		{
			uint32_t checksumReadFromFile=0;
			UINT size = NUMBER_OF_CHECKSUM_BYTE;
			result = f_read(&file,&checksumReadFromFile,size,&size);
			if((result != FR_OK)||(size!=NUMBER_OF_CHECKSUM_BYTE))
			{
				error = true;
			}
			else
			{
				uint32_t temp = checksumReadFromFile;
				checksumReadFromFile = 0;
				for(int i=3;i>=0;i--)
				{
					checksumReadFromFile = ((temp>>i*8)&(0xff))<<((3-i)*8) | checksumReadFromFile;
				}
				*readValue = checksumReadFromFile;
				*calValue = checksum;
			}
		}
		f_close(&file);
	}
	else
	{
		error = true;
	}
	return error;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Compute checksum value	for internal flash		   							*/
/*                                                                                      */
/* Details  -Compute checksum value based on the address and number of byte				*/
/* 			 in flash																	*/
/* Arguments : (I) unsigned char *baseAddr												*/
/* 			   (I) uint32_t nBytes														*/
/*             (O) NONE																	*/
/* ReturnValue : uint32_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
uint32_t checksum_Compute(unsigned char *baseAddr, uint32_t nBytes)
{
	uint32_t sum = 0;
	while(nBytes--)
	{
		sum ^= *baseAddr++;
	}
	return sum;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Compute checksum value	for SPI	flash		   								*/
/*                                                                                      */
/* Details  -Compute checksum value based on the address and number of byte				*/
/* 			 in flash																	*/
/* Arguments : (I) unsigned char *baseAddr												*/
/* 			   (I) uint32_t nBytes														*/
/*             (O) NONE																	*/
/* ReturnValue : uint32_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
uint32_t checksum_ComputeSPIF(uint32_t baseAddr, uint32_t nBytes)
{
	SPIFI_ERR_T errCode;
	uint8_t Buffer[NUMBER_OF_BYTE_READ_FROM_SPIF_PER_CYCLE]={'\0'};
	uint32_t readBytes=0;
	uint32_t loopBytes=NUMBER_OF_BYTE_READ_FROM_SPIF_PER_CYCLE;
	uint32_t remainBytes=nBytes;
	uint32_t CS_SPIF=0;
	while(readBytes<nBytes)
	{
		if(loopBytes>remainBytes)
		{
			loopBytes = remainBytes;
		}
		errCode = spifiDevRead(pSpifi, baseAddr, (uint32_t*)Buffer, loopBytes);
		if(errCode != SPIFI_ERR_NONE)
		{
			return 0;
		}
		for(int i=0;i<loopBytes;i++)
		{
			CS_SPIF+=Buffer[i];
		}
		readBytes+=loopBytes;
		remainBytes-=loopBytes;
	}
	return CS_SPIF;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-Write checksum values and sizes of firmware to Flash bank A				*/
/*                                                                                      */
/* Details  -Write checksum values and sizes of firmware to Flash bank A				*/
/* Arguments : (I) uint32_t val[3]														*/
/* 			   (I)	uint32_t size[3]													*/
/*             (O) NONE																	*/
/* ReturnValue : uint8_t		                                                        */
/*                                                                                      */
/****************************************************************************************/
uint8_t checksum_WritetoFlash(uint32_t val[2],uint32_t size[2])
{
	uint32_t data[128]={'\0'};
	data[127]=val[0];
	data[126]=val[1];
	data[125]=size[0];
	data[124]=size[1];
	DEBUGOUT("data write %d %d %d %d\n",data[127],data[126],data[125],data[124]);
	uint8_t result=0;
	__disable_irq();
	Chip_IAP_PreSectorForReadWrite(CS_START_SECTOR, CS_END_SECTOR,  IAP_FLASH_BANK_B);
	result = Chip_IAP_EraseSector(CS_START_SECTOR, CS_END_SECTOR, IAP_FLASH_BANK_B);
	if(result ==IAP_CMD_SUCCESS)
	{
		Chip_IAP_PreSectorForReadWrite(CS_START_SECTOR, CS_END_SECTOR,  IAP_FLASH_BANK_B);
		result = Chip_IAP_CopyRamToFlash(FL_WR_ADDR, &data[0], BYTE_CS_WRITE);
	}
	__enable_irq();
	return result;
}
/****************************************************************************************/
/*                                                                                      */
/* General 	-checksum_Check																*/
/*                                                                                      */
/* Details  -Checksum data in Flash Bank												*/
/* Arguments : (I) None																	*/
/*             (O) None																	*/
/* ReturnValue : bool		                                                        	*/
/*                                                                                      */
/****************************************************************************************/
bool checksum_Check(void)
{
	bool error = false;
	int32_t Size_BankA = (*(uint32_t*)(CS_ADDR-8));
	int32_t Size_SPIF = (*(uint32_t*)(CS_ADDR-12));
	DEBUGOUT("Size bank A %d\n",Size_BankA);
	DEBUGOUT("Size SPIF %d\n",Size_SPIF);
	if((Size_BankA<=0) || (Size_SPIF<=0))
	{
		error = true;
	}
	else
	{
		uint32_t CS_BankA_cmp=checksum_Compute((uint8_t *)USER_PRO_ADDR_M4,Size_BankA);
		uint32_t CS_SPIF_cmp=checksum_ComputeSPIF(SPIF_BASE_ADDRESS,Size_SPIF);
		uint32_t CS_BankA_r= (*(uint32_t*)CS_ADDR);
		uint32_t CS_SPIF_r = (*(uint32_t*)(CS_ADDR-4));
		DEBUGOUT("CS_BankA_cmp %d\n",CS_BankA_cmp);
		DEBUGOUT("CS_SPIF_cmp %d\n",CS_BankA_r);
		DEBUGOUT("CS_BankA_r %d\n",Size_BankA);
		DEBUGOUT("CS_SPIF_r %d\n",CS_SPIF_r);
		if((CS_BankA_cmp!=CS_BankA_r) || (CS_SPIF_cmp!=CS_SPIF_r))
		{
			error = true;
		}
	}
	return error;
}
