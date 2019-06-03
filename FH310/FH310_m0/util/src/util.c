/********************************************************************/
/*                                                                  */
/* File Name    : util.c                             			   	*/
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
/* #0002	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add bracket {} to if else code to follow coding rule		*/
/********************************************************************/

#include "util.h"
/****************************************************************************************/
/*                                                                                      */
/* General： xxxxx				                                                        */
/*                                                                                      */
/* Details：  find_index                													*/
/*                                                                                      */
/* Arguments : (I) int a[]
 * 				   size_t size,
 * 				   int value
 * 				   							  											*/
/*																						*/
/*                                                                                      */
/*             (O) NONE                                                                 */
/* ReturnValue : uint16_t : index                                                       */
/*        		                                                      					*/
/*                                                                                      */
/* Using Global Data : (I) NONE                             							*/
/*                     (O) NONE                                              			*/
/*                     (O) NONE                                  						*/
/* local valiables   : (I) NONE                                                   		*/
/*【Note】                                                                             												*/
/*                                                                                      */
/****************************************************************************************/
size_t find_index(const uint16_t a[], size_t size, int value )
{
    size_t index = 0;

    while ( index < size && a[index] != value ) ++index;

    return ( index == size ? -1 : index );
}

//******************************************************************************/
//    Operation Name: HexCharToInt(int8_t a)
//
//    Processing:  Convert hex char to int
//
//    Input Parameters: None.
//
//    Output Parameters:
//
//    Return Values: int
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements: None.
//
/******************************************************************************/
int HexCharToInt(int8_t a)
{
    if (a>='0' && a<='9')
    {
        return (a-48);
    }
    else if (a>='A' && a<='Z')
    {
        return (a-55);
    }
    else
    {
        return (a - 87);
    }
}

//******************************************************************************/
//    Operation Name: CharToAscci(int8_t ch)
//
//    Processing:  Convert hex char to int
//
//    Input Parameters: None.
//
//    Output Parameters:
//
//    Return Values: int
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements: None.
//
/******************************************************************************/
int CharToAscci(int8_t ch)
{
    if (ch>=0 && ch<=9)
	{
        return (ch + 48);
	}
    else if (ch>=10 && ch<=15)
	{
        return (ch + 55);
	}
    else
	{
        return -1;
	}
}
