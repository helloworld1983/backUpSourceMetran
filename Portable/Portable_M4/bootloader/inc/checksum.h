#ifndef INC_CHECKSUM_H_
#define INC_CHECKSUM_H_

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

#include "board.h"

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
bool checksum_checkFile(char* filename,uint32_t size,uint32_t* readValue,uint32_t* calValue);


#endif /* INC_CHECKSUM_H_ */
