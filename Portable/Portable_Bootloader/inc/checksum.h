/********************************************************************/
/*                                                                  */
/* File Name    : checksum.h                                        */
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
#ifndef INC_CHECKSUM_H_
#define INC_CHECKSUM_H_
#include "board.h"
bool checksum_checkFile(char* filename,uint32_t size,uint32_t* readValue,uint32_t* calValue);
uint32_t checksum_Compute(unsigned char *baseAddr, uint32_t nBytes);
uint32_t checksum_ComputeSPIF(uint32_t baseAddr, uint32_t nBytes);
uint8_t checksum_WritetoFlash(uint32_t val[2],uint32_t size[2]);
bool checksum_Check(void);
bool checksum_CheckBankA();
#endif /* INC_CHECKSUM_H_ */
