/********************************************************************/
/*                                                                  */
/* File Name    : util.h                             			   	*/
/*                                                                  */
/* General       : This module contain general function which is    */
/*				   used by others module 							*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/06/02     Quyen Ngo(MV)  		new file        */
/*                                                                  */
/* #0001	   2017/11/17	  Quyen Ngo				modify	        */
/*		-remove function "reverseByte" not use 						*/
/********************************************************************/

#ifndef INC_UTIL_H_
	#define INC_UTIL_H_

#include <lpc_types.h>
#include <stddef.h>

int CharToAscci(int8_t ch);

int HexCharToInt(int8_t a);

size_t find_index(const uint16_t a[], size_t size, int value );

#endif /* INC_UTIL_H_ */
