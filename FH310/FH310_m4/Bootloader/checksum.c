/********************************************************************/
/*                                                                  */
/* File Name    : checksum.c                                        */
/*                                                                  */
/* General      : Provides fast and efficient way to compute a 		*/
/* 				  32 bit checksum values on a given sum of data 	*/
/*                bytes using pre-built checksum look-up tables. 	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH-310                                           */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/09/23      Thao Ha			  	new file        */
/*             2017/08/24      Linh Nguyen		  	modify		    */
/*           add the return value for checksum function             */
/*  #0002	   2017/09/01	   Quyen Ngo			modify			*/
/*			-add {} to if condition, while loop						*/
/*  #0003	   2017/09/28	   Viet Le			modify				*/
/*          -Remove unused code                                     */
/********************************************************************/
#include "checksum.h"
#include "GuiInterface.h"
#include "ff.h"

/****************************************************************************************/
/*                                                                                      */
/* General 	-Check if the file is valid													*/
/*                                                                                      */
/* Details  -Check if the file is valid by calculating the checksum value of the file	*/
/* 			 and compare to the one that included in the last 4 bytes of the file		*/
/* Arguments : (I) int8_t* filename														*/
/* 			   (I) uint32_t size														*/
/*             (O) NONE																	*/
/* ReturnValue : bool		                                                        	*/
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
IMAGE_CHECK_RESULT_t checksum_File(int8_t* filename,uint32_t size)
{
	IMAGE_CHECK_RESULT_t result;
	FIL file;
	bool error = false;
	FRESULT errf;
	uint32_t CheckSum = 0;
	uint32_t CheckSum_read=0;
	if((f_open(&file, (char*)filename, FA_READ)) == FR_OK)
	{

		uint32_t loopBytes=BYTE_CS_WRITE;
		uint8_t Buff[loopBytes];
		uint32_t readBytes;
		int32_t bytesRemaining = size;
		while(bytesRemaining > 0)
		{
			guiInterface_WWDTFeed(LPC_WWDT);
			if(bytesRemaining >= loopBytes)
			{
				readBytes = loopBytes;
			}
			else
			{
				readBytes = bytesRemaining;
			}
			errf=f_read(&file, &Buff[0], (UINT)readBytes, (UINT*)&readBytes);
			if (errf != FR_OK)
			{
				DEBUGOUT("Read failed %d \n",errf);
				error = true;
				break;
			}
			for(int i=0 ; i<readBytes;i++)
			{
				CheckSum += Buff[i];
			}
			bytesRemaining -=readBytes;
		}

	}
	if(error == false)
	{
		uint8_t size_t = 4;
		errf = f_read(&file, &CheckSum_read, (UINT)size_t, (UINT*)&size_t);
		if (errf != FR_OK)
		{
			DEBUGOUT("Read failed %d \n",errf);
			error = true;
		}
		else
		{
			uint32_t temp = CheckSum_read;
			CheckSum_read=0;
			for(int i=3;i>=0;i--)
			{
				CheckSum_read = ((temp>>i*8)&(0xff))<<((3-i)*8) | CheckSum_read;
			}

			if (CheckSum != CheckSum_read)
				error = true;
		}
	}
	f_close(&file);
	result.error = error;
	result.checksumValue = CheckSum_read;
	result.checksumCalcultation = CheckSum;
	return result;
}

/* end of files */
