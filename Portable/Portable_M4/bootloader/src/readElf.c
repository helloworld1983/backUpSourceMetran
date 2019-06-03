/********************************************************************/
/*                                                                  */
/* File Name    : readE.h                        	        		*/
/*                                                                  */
/* General      : Provide function to read Elf files                */
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
#include "readElf.h"
#include "ff.h"
/****************************************************************************************/
/*                                                                                      */
/* General 	-read ELF header of a AXF file				   								*/
/*                                                                                      */
/* Details  -read ELF header of a AXF file												*/
/* Arguments : (I) char *filename														*/
/*             (O) Elf32_Ehdr* Ehdr														*/
/* ReturnValue : bool		                                                        	*/
/*                                                                                      */
/****************************************************************************************/
bool readElfhdr(char *filename,Elf32_Ehdr* Ehdr)
{
	bool error=false;
	static FIL file;
	FRESULT result = f_open(&file, filename, FA_READ);
	if(result  == FR_OK)
	{
		FRESULT result;
		UINT size = sizeof(Elf32_Ehdr) ;
		result = f_read(&file,Ehdr,size,&size);
		if ((result != FR_OK)||(size!=sizeof(Elf32_Ehdr)))
		{
			error = true;
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
/* General 	-read ELF program header of a AXF file		   								*/
/*                                                                                      */
/* Details  -read ELF program header of a AXF file										*/
/* Arguments : (I) char *filename														*/
/*             (O) Elf32_Ehdr* Ehdr														*/
/* ReturnValue : bool		                                                        	*/
/*                                                                                      */
/****************************************************************************************/
bool readElfPhdr(char *filename,Elf32_Phdr* Phdr,uint32_t offset,uint8_t size)
{
	static FIL file;
	bool error=false;
	if((f_open(&file, filename, FA_READ)) == FR_OK)
	{
		FRESULT result;
		UINT sizeTemp = size ;
		result = f_lseek(&file,offset);
		if (result != FR_OK)
		{
			error = true;
		}
		else
		{
			result = f_read(&file,Phdr,sizeTemp,&sizeTemp);
			if ((result != FR_OK)||(sizeTemp!=size))
			{
				error = true;
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


