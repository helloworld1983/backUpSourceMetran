/********************************************************************/
/*                                                                  */
/* File Name    : readELF.c                                			*/
/*                                                                  */
/* General      : Provide function to read Elf files                */
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
/* #0001       2017/09/28       Viet Le             modify 			*/
/*			Add #include "updateFlash.h"							*/
/*                                                                  */
/********************************************************************/
#include "readElf.h"
#include "updateFlash.h"

/****************************************************************************************/
/*                                                                                      */
/* General 	-read ELF header of a AXF file				   								*/
/*                                                                                      */
/* Details  -read ELF header of a AXF file												*/
/* Arguments : (I) int8_t *filename														*/
/*             (O) Elf32_Ehdr* Ehdr														*/
/* ReturnValue : bool		                                                        	*/
/* Using Global Data : (I) g_win							                           	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
bool readElfhdr(int8_t *filename,Elf32_Ehdr* Ehdr)
{
	static FIL file;
	f_mount(&fatFS,"",0);
	bool error=false;
	if((f_open(&file, (char*)filename, FA_READ)) == FR_OK)
	{
		FRESULT errf;
		UINT r = sizeof(Elf32_Ehdr) ;
		errf = f_read(&file,Ehdr,r,&r);
		if (errf != FR_OK)
		{
			//DEBUGOUT("Read Elf header failed %d \n",errf);
			error = true;
		}
		else
		{
			/*Print Elf Header file*/
			DEBUGOUT("************************************************************************\n");
			DEBUGOUT("Entry address: 					0x%08x\n",Ehdr->e_entry);
			DEBUGOUT("Program header offset: 				0x%08x\n",Ehdr->e_phoff);
			DEBUGOUT("Size of Program Header entry: 			%d\n",Ehdr->e_phentsize);
			DEBUGOUT("Number of Program Header entries:		%d\n",Ehdr->e_phnum);
			DEBUGOUT("Section header offset: 				0x%08x\n",Ehdr->e_shoff);
			DEBUGOUT("Size of Section Header entry: 			%d\n",Ehdr->e_shentsize);
			DEBUGOUT("Number of Section Header entries:		%d\n",Ehdr->e_shnum);
			DEBUGOUT("************************************************************************\n");
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
/* Using Global Data : (I) NONE								                           	*/
/*                     (O) NONE                                           			   	*/
/* local variables   : NONE			                                                  	*/
/*【Note】                                                                              													*/
/*                                                                                      */
/****************************************************************************************/
bool readElfPhdr(int8_t *filename,Elf32_Phdr* Phdr,uint32_t offset,uint8_t size)
{
	DEBUGOUT("Start reading ELF program header at 0x%08x\n",offset);
	static FIL file;
	bool error=false;
	if((f_open(&file, (char*)filename, FA_READ)) == FR_OK)
	{
		DEBUGOUT("Open file to read ELF program header OK\n");
		FRESULT errf;
		UINT r = size ;
		errf = f_lseek(&file,offset);
		if (errf != FR_OK)
		{
			DEBUGOUT("Seek Elf program header failed %d \n",errf);
			error = true;
		}
		else
		{
			errf = f_read(&file,Phdr,size,&r);
			if (errf != FR_OK)
			{
				DEBUGOUT("Read Elf program header failed %d \n",errf);
				error = true;
			}
			else
			{
				/*Print Elf program Header file*/
				DEBUGOUT("************************************************************************\n");
				DEBUGOUT("Offset: 						0x%08x\n",Phdr->p_offset);
				DEBUGOUT("Virtual address: 				0x%08x\n",Phdr->p_vaddr);
				DEBUGOUT("Physical address: 				0x%08x\n",Phdr->p_paddr);
				DEBUGOUT("File size:					%d\n",Phdr->p_filesz);
				DEBUGOUT("Memory size: 					%d\n",Phdr->p_memsz);
				DEBUGOUT("************************************************************************\n");

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

/* end of files */
