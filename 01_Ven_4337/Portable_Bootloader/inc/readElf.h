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
#ifndef INC_READELF_H_
#define INC_READELF_H_

#include <stdio.h>
#include "board.h"


typedef uint32_t Elf32_Addr;
typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Off;
typedef uint32_t Elf32_Sword;
typedef uint32_t Elf32_Word;

typedef struct {
	unsigned char e_ident[16];
	Elf32_Half 	e_type;
	Elf32_Half 	e_machine;
	Elf32_Word 	e_version;
	Elf32_Addr 	e_entry;
	Elf32_Off	e_phoff;
	Elf32_Off	e_shoff;
	Elf32_Word	e_flags;
	Elf32_Half	e_ehsize;
	Elf32_Half	e_phentsize;
	Elf32_Half	e_phnum;
	Elf32_Half	e_shentsize;
	Elf32_Half	e_shnum;
	Elf32_Half	e_shstrndx;
} Elf32_Ehdr;

typedef struct {
	Elf32_Word	p_type;
	Elf32_Off	p_offset;
	Elf32_Addr	p_vaddr;
	Elf32_Addr	p_paddr;
	Elf32_Word	p_filesz;
	Elf32_Word	p_memsz;
	Elf32_Word	p_flags;
	Elf32_Word	p_align;
} Elf32_Phdr;

bool readElfhdr(char *filename,Elf32_Ehdr* hdr);
bool readElfPhdr(char *filename,Elf32_Phdr* Phdr,uint32_t offset,uint8_t size);


#endif /* INC_READELF_H_ */
