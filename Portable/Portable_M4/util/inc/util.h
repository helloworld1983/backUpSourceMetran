/*
 * util.h
 *
 *  Created on: Dec 26, 2016
 *      Author: ThaoHa
 */

#ifndef UTIL_INC_UTIL_H_
#define UTIL_INC_UTIL_H_

/****************************************************************************************/
/* General： find_index                                                                  */
/*                                                                                      */
/* Details：  find index                                                                  */
/*                                                                                      */
/* Arguments : (I) int a[]                                                              */
/*                 size_t size,                                                         */
/*                 int value                                                            */
/*                                                                                      */
/* ReturnValue : size_t                                                                */
//***************************************************************************************/
size_t find_index(const uint16_t a[], size_t size, int value );

/****************************************************************************************/
/* General： HexCharToInt                                                                 */
/*                                                                                      */
/* Details：  Convert hex to char                                                         */
/*                                                                                      */
/* Arguments : (I) char a                                                               */
/*                                                                                      */
/* ReturnValue : int                                                                    */
//***************************************************************************************/
int HexCharToInt(char a);

/****************************************************************************************/
/* General： CharToAscci                                                                 */
/*                                                                                      */
/* Details：  Convert char to Ascii                                                       */
/*                                                                                      */
/* Arguments : (I) char a                                                               */
/*                                                                                      */
/* ReturnValue : int                                                                    */
//***************************************************************************************/
int CharToAscci(char ch);

/****************************************************************************************/
/* General： util_CompCrcCcitt                                                            */
/*                                                                                      */
/* Details：  Computes a 16 bit CRC-CCITT value on a given data using the                 */
//        16 bit polynominal function. In order to make the CRC computing               */
//        fast, the method utilizes the S_CRC16Tab pre-computed lookup table.           */
//      The table was computed based on the 16 bit polynominal, X^16+X^12+X^5+X^0       */
//        using the reverse Poly mask to  suit LSB data CRC computation.                */
//        This method can be called accumulatively as long as the initCRC value         */
//        contains a CRC value on the previous data.  A calling routine should          */
//      provide a zero as the initCRC when a new CRC value is computed on a new         */
//      segment of data.                                                                */
/*                                                                                      */
/* Arguments : (I) uint16_t initCRC                                                     */
/*              unsigned ch                                                             */
/*              long nBytes                                                             */
/*                                                                                      */
/* ReturnValue : uint16_t                                                               */
//***************************************************************************************/
uint16_t util_CompCrcCcitt(uint16_t initCRC, unsigned char *pData, long nBytes);



#endif /* UTIL_INC_UTIL_H_ */
