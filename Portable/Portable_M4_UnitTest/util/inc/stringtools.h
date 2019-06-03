#ifndef STRINGTOOLS_H
#define STRINGTOOLS_H

/********************************************************************/
/*                                                                  */
/* File Name    : stringtools.h                          			*/
/*                                                                  */
/* General      : stringtools provides all string tool 				*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/14       Viet Le		  	    new file        */
/* #0001       														*/
/*                                                                  */
/********************************************************************/

#include <string.h>
#include "stdint.h"

/****************************************************************************************/
/*                                                                                      */
/* General  -Convert int to Ascii                                                       */
/*                                                                                      */
/* Details  -This operation convert an integer number to ASCII string character         */
/*      then return number of character of the output string. A desired number of       */
/*      character in string output is supported as an option. if number of character    */
/*      is bigger than string size, the '0' character is filled in front of the string. */
/*      Otherwise, keep string as it is                                                 */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) long int a : the number need to be converted                         */
/*                 char* buff : pointer to save string after conversion.                */
/*                 int numOfDigit : desired length of string.                           */
/*                                                                                      */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : int - size of string                                                   */
/****************************************************************************************/
int stringtools_IntToAscii(long int a, char* buff, int numOfDigit);

/****************************************************************************************/
/*                                                                                      */
/* General  -Converts a floating point number to string                                 */
/*                                                                                      */
/* Details  -Converts a given floating x to string str[].  d is the number              */
/*           of digits required in output. If d is more than the number                 */
/*           of digits in x, then 0s are added at the beginning.                        */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) char str[] : pointer string                                          */
/*                 char *res : buffer res                                               */
/*                 afterpoint : point                                                   */
/*                                                                                      */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : NONE                                                                   */
/****************************************************************************************/
void ftoa(float n, char *res, int afterpoint);

/****************************************************************************************/
/*                                                                                      */
/* General  - Convert flow output to string                                             */
/*                                                                                      */
/* Details  - Convert flow output to string                                             */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) char* buff - pointer to save string after conversion                 */
/*                                                                                      */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : char - string after convert                                            */
/****************************************************************************************/
char* ConvertFCFlowToString(unsigned char value);

/****************************************************************************************/
/*                                                                                      */
/* General  - Get current date to string                                                */
/*                                                                                      */
/* Details  - Get current date to string                                                */
/*                                                                                      */
/*                                                                                      */
/* Arguments : (I) NONE                                                                 */
/*                                                                                      */
/*             (O) NONE                                                                 */
/*                                                                                      */
/* ReturnValue : char - string after convert                                            */
/****************************************************************************************/
char* GetDateString();

#endif /* STRINGTOOLS_H */
