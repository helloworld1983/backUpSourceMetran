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
#define NUMBER_OF_CHECKSUM_BYTE 4



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
