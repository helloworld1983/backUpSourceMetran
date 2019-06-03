/********************************************************************/
/*                                                                  */
/* File Name    : checksum.h                                        */
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
/*         define IMAGE_CHECK_RESULT_t struct and update 			*/
/*                checksum_File function	                        */
/*             2017/09/28      Viet Le		  	    modify		    */
/*         Remove unused code                                       */
/********************************************************************/
#ifndef INC_CHECKSUM_H_
#define INC_CHECKSUM_H_
/* include system headers */
#include <stdio.h>
#include "stdint.h"
#include "stdbool.h"

/* constants, definitions */
#define USER_PRO_ADDR		0x1A020000
#define CS_ADDR				0x1B07fffc
#define FL_WR_ADDR			0x1B07fe00
#define BYTE_CS_WRITE 		512
#define CS_START_SECTOR	14
#define CS_END_SECTOR	14//fatFS

typedef struct
{
	bool error; //checking result 1: fail   0: pass
	uint32_t checksumValue; //checksum value which is record in file
	uint32_t checksumCalcultation; //checksum value which is calculated
}IMAGE_CHECK_RESULT_t;

//Check if the file is valid
IMAGE_CHECK_RESULT_t checksum_File(int8_t* filename,uint32_t size);

#endif /* INC_CHECKSUM_H_ */
