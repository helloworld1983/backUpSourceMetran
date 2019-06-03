/********************************************************************/
/*                                                                  */
/* File Name    : util.c                       						*/
/*                                                                  */
/* General      : Contains function for calulation					*/
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* 		       2016/08/25      Quyen Ngo(MV)  	    new file        */
/*  #0001	   2017/10/09      Linh Nguyen		  	modify		    */
/*  Modify UpperCase function for long string					 	*/
/*  #0002	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add bracket {} to if else code to follow coding rule		*/
/********************************************************************/

#include "util.h"
#include "string.h"
#include "math.h"

#define VDDA 3.3

const float a1 = 0.004091;
const float a2 = 0.008182;
const float a3 = -0.008182;
const float b1 = 0.09091;
const float b2 = 0.09091;
const float b3 = 0.09091;

//****************************************************************************
//
// The following CRC lookup table was copied from linux/crc-ccitt.c
//***************************************************************************
//
static short S_CRC_CcittTab[256] =
{
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

//******************************************************************************/
//    Operation Name: S_CompCRC_Ccitt(uint16_t initCRC, uint8_t *pData, long nBytes)
//
//    Processing:  Computes a 16 bit CRC-CCITT value on a given data using the
//        16 bit polynominal function. In order to make the CRC computing
//        fast, the method utilizes the S_CRC16Tab pre-computed lookup table.
//      The table was computed based on the 16 bit polynominal, X^16+X^12+X^5+X^0
//        using the reverse Poly mask to  suit LSB data CRC computation.
//        This method can be called accumulatively as long as the initCRC value
//        contains a CRC value on the previous data.  A calling routine should
//      provide a zero as the initCRC when a new CRC value is computed on a new
//      segment of data.
//
//    Input Parameters: initCRC -- the initial CRC value
//
//                        pData  -- a pointer to the data area that needs to be
//                                  checksumed. The bit and byte ording is assumed
//                                  to be LSB.
//                        nBytes - a total number of data bytes needs to be CRC
//                                 checksumed.
//
//    Output Parameters:
//
//    Return Values: a 16 bit unsigned CRC checksum value on the provided data.
//
//
//    Pre-Conditions:
//
//    Miscellaneous: This algorithem is most efficient for a LSB bit first data feed.
//
//    Requirements:
//
//******************************************************************************/
uint16_t util_CompCrcCcitt(uint16_t initCRC, uint8_t *pData, uint32_t nBytes)
{
    while(nBytes--)
    {
        initCRC = S_CRC_CcittTab[ ( initCRC >> 8 ^ *pData++) & (0xFFL)]
                ^ ( initCRC << 8 );
    }
    return (initCRC);
}
/****************************************************************************************/
/*                                                                                      */
/* General： xxxxx				                                                        */
/*                                                                                      */
/* Details：  find_index                													*/
/*                                                                                      */
/* Arguments : (I) int a[]                                                  		    */
/* 				   size_t size,                                                       	*/
/* 				   int value                               		                        */
/* ReturnValue : uint16_t : index                                                       */
/****************************************************************************************/
size_t find_index(const uint16_t a[], size_t size, int value )
{
    size_t index = 0;

    while ( index < size && a[index] != value ) ++index;

    return ( index == size ? -1 : index );
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	HexCharToInt			                                        	*/
/*                                                                                      */
/* Details		: 	convert hex char to int												*/
/*                                                                                      */
/* Arguments 	: 	(I)  int8_t a				                                  		*/
/*                                                                                      */
/* ReturnValue : int                                                         			*/
/*                                                                                      */
/****************************************************************************************/
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

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	CharToAscci				                                        	*/
/*                                                                                      */
/* Details		: 	convert integer digit to ascci										*/
/*                                                                                      */
/* Arguments 	: 	(I)  int8_t ch				                                  		*/
/*                                                                                      */
/* ReturnValue : int	                                                         		*/
/*                                                                                      */
/****************************************************************************************/
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

/****************************************************************************************/
/*                                                                                      */
/* Function name: UpperCase		                                       					*/
/*                                                                                      */
/* Details:  UpperCase for a sting														*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) char* s				  												*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : NONE			                                                        */
/****************************************************************************************/
void UpperCase(char* s)
{
	int len = strlen(s);
	for (int i=0; i < len; i++)
	{
		if(s[i]>= 'a' && s[i]<='z')
		{
			s[i] -= ('a'-'A');
		}
	}
	return;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	util_DecCharToInt		                                        	*/
/*                                                                                      */
/* Details		: 	convert dec char to int												*/
/*                                                                                      */
/* Arguments 	: 	(I)  int8_t *s				                                  		*/
/*                  (I)  size_t size                                                    */
/* ReturnValue : int                                                         			*/
/*                                                                                      */
/****************************************************************************************/
uint32_t util_DecCharToInt(int8_t *s, size_t size)
{
	uint32_t result=0;
	for (int i=0; i<size;i++)
	{
		result += (HexCharToInt(*s)*pow(10,(size -i-1)));
		s++;
	}
	return result;
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	HexStrToInt		                                        			*/
/*                                                                                      */
/* Details		: 	convert hex string to int											*/
/*                                                                                      */
/* Arguments 	: 	(I)  int8_t *s				                                  		*/
/*                  (I)  uint8_t size                                                   */
/* ReturnValue : uint32_t                                                      			*/
/*                                                                                      */
/****************************************************************************************/
uint32_t HexStrToInt(int8_t *s, uint8_t size)
{
	uint32_t result =0;
	int8_t* c;
	c=s;
	for(int i= size -1; i>=0; i--)
	{
		result += HexCharToInt(*c)<<(i*4);
		c++;
	}
	return result;
}


/****************************************************************************************/
/*                                                                                      */
/* Function name: 	HexStrToInt		                                        			*/
/*                                                                                      */
/* Details		: 	convert hex string to int											*/
/*                                                                                      */
/* Arguments 	: 	(I)  FIL* file				                                  		*/
/*                  (I)  int8_t headedCh                                                */
/*                  (I)  uint32_t* byteCount                                            */
/*                  (I)  uint8_t len	                                                */
/*                  (I)  bool isHex		                                                */
/* ReturnValue : uint32_t                                                      			*/
/*                                                                                      */
/****************************************************************************************/
uint32_t GetValue(FIL* file, int8_t headedCh, uint32_t* byteCount, uint8_t len, bool isHex)
{
	UINT r=0;
	int8_t ch =0;
	int8_t buff[8]={0};
	while(ch!=headedCh)
	{
		(*byteCount)--;
		f_lseek(file,(DWORD)(*byteCount));
		f_read(file,&ch,1,&r);
	}
	f_read(file,buff,len,&r);

	//DEBUGOUT("Flow rate: %s\n",buff);
	if(isHex)
	{
		return HexStrToInt(buff,len);
	}
	else
	{
		return util_DecCharToInt(buff,len);
	}
}
