//******************************************************************************/
//$COMMON.CPP$
//   File Name:  CRC.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Provides fast and efficient way to compute a 16 bit CRC and a 32 bit
//      checksum values on a given sum of data bytes using pre-built
//		checksum look-up tables. The 16bit CRC is computed using the  polynominal
//      X^16+X^15+X^2+X^0 and its computer internal representation is 0xA001.
//		The 32 bit CRC is computed using the polynominal; 
//      X^32 +X^26 +X^23 +X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0,
//		and internal representation 0xEDB88320.  
//      
//
//   Interfaces: This class doesn't get instantiated, two static methods
//		S_CompCRC32 and S_CompCRC16 are provided.
//	    Two C Wrap-around methods are provided for a convenience sake.
//
//   Restrictions:
//
//   Revision History:
//      Rev:  Date:   	Engineer:     		Project:
//      01    03/05/97   Jung Verheiden    			  Morph
//      Description: Initial Revision
//
/******************************************************************************/
#include "CRC.h"

//define static class attributes.

//****************************************************************************
//
// The following CRC lookup table was generated by the CRC32Tab MS Windows
// program. The look up table is only valid for the 32 bit CRC using the 
// polynominal: 
// X^32 +X^26 +X^23 +X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0
// and its reverse polynominal mask(LSB representation ) 0xEDB88320.
// This same polynominal is used for the Ethernet communication protocol.
//***************************************************************************
//


ULONG
CRC::S_CRC32Tab[256] =
{	
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 
		0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 
		0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 
		0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 
		0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 
		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 
		0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 
		0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 
		0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 
		0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 
		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 
		0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 
		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 
		0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 
		0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 
		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 
		0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 
		0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 
		0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 
		0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 
		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 
		0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 
		0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 
		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 
		0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 
		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 
		0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 
		0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 
		0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 
		0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 
		0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 
		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 
		0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 
		0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 
		0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 
		0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 
		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 
		0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 
		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 
		0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 
		0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 
		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 
		0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 
		0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 
		0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 
		0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 
		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 
		0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 
		0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 
		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 
		0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 
		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 
		0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 
		
};

//****************************************************************************
//
// The following CRC lookup table was generated by the CRC16Tab MS Windows
// program. The look up table is only valid for the 16 bit CRC using the 
// polynominal X^16+X^15+X^2+X^0 and its reverse bit order (LSB representation) 
// mask 0xA001.
//***************************************************************************
//

USHORT CRC::S_CRC16Tab[256] =
{	
		0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241, 
		0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440, 
		0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40, 
		0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841, 
		0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40, 
		0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41, 
		0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641, 
		0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040, 
		0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240, 
		0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441, 
		0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41, 
		0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840, 
		0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41, 
		0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40, 
		0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640, 
		0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041, 
		0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240, 
		0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441, 
		0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41, 
		0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840, 
		0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41, 
		0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40, 
		0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640, 
		0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041, 
		0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241, 
		0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440, 
		0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40, 
		0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841, 
		0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40, 
		0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41, 
		0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641, 
		0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040, 
		
};

//******************************************************************************/
//    Operation Name: CompCRC16( USHORT initCRC, UCHAR *pData, ULONG nBytes)
//
//    Processing:  Computes a 16 bit CRC value on a given data using the
//		16 bit polynominal function. It is a C wrap around function of
//      the CRC::S_CompCRC16 method for a C or an Assembly code to call it
//      without having to deal the C++ function name mangling.
//
//    Input Parameters: initCRC -- the initial CRC value 
//
//						pData  -- a pointer to the data area that needs to be
//								  checksumed. The bit and byte ording is assumed
//								  to be LSB.
//						nBytes - a total number of data bytes needs to be CRC
//								 checksumed.
//
//    Output Parameters:
//
//    Return Values: a 16 bit unsigned CRC checksum value on the provided data.  
//					 
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

extern "C" 
USHORT CompCRC16(USHORT initCRC, UCHAR *pData, ULONG nBytes)
{
	return( CRC::S_CompCRC16(initCRC, pData, nBytes));
}

//******************************************************************************/
//    Operation Name: CompCRC32()
//
//    Processing:  Computes a 32 bit CRC value on a given data using the
//		32 bit polynominal function. 
//
//    Input Parameters: initCRC -- the initial CRC value 
//
//						pData  -- a pointer to the data area that needs to be
//								  checksumed. The bit and byte ording is assumed
//								  to be LSB.
//						nBytes - a total number of data bytes needs to be CRC
//								 checksumed.
//
//    Output Parameters:
//
//    Return Values: a 32 bit unsigned CRC checksum value on the provided data.  
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/

extern "C" 
ULONG CompCRC32(ULONG initCRC, UCHAR *pData, ULONG nBytes)
{
	return( CRC::S_CompCRC32(initCRC, pData, nBytes));
}

//******************************************************************************/
//    Operation Name: S_CompCRC16( USHORT initCRC, UCHAR *pData, ULONG nBytes)
//
//    Processing:  Computes a 16 bit CRC value on a given data using the
//		16 bit polynominal function. In order to make the CRC computing
//		fast, the method utilizes the S_CRC16Tab pre-computed lookup table.
//      The table was computed based on the 16 bit polynominal, X^16+X^15+X^2+X^0
//	    using the reverse Poly mask to  suit LSB data CRC computation.
//		This method can be called accumulatively as long as the initCRC value 
//		contains a CRC value on the previous data.  A calling routine should 
//      provide a zero as the initCRC when a new CRC value is computed on a new
//      segment of data.
//
//    Input Parameters: initCRC -- the initial CRC value 
//
//						pData  -- a pointer to the data area that needs to be
//								  checksumed. The bit and byte ording is assumed
//								  to be LSB.
//						nBytes - a total number of data bytes needs to be CRC
//								 checksumed.
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
USHORT CRC::S_CompCRC16(USHORT initCRC, UCHAR *pData, ULONG nBytes)
{
		while(nBytes--)
		{
			initCRC = S_CRC16Tab[ ( initCRC ^ *pData++) & (0xFFL)]
						^ ( initCRC >> 8 );
	    }
		return(initCRC);
}

//******************************************************************************/
//    Operation Name: S_CompCRC32()
//
//    Processing:  Computes a 32 bit CRC value on a given data using the
//		32 bit polynominal function. In order to make the CRC computing
//		fast, the method utilizes the S_CRC32Tab pre-computed lookup table.
//		The table was computed based on the polynominal:
//      X^32 +X^26 +X^23 +X^22+X^16+X^12+X^11+X^10+X^8+X^7+X^5+X^4+X^2+X^1+X^0  
//	    using the reverse Poly mask to  suit LSB data CRC computation.
//		This method can be called accumulatively as long as the initCRC value 
//		contains a CRC value on the previous data.  A calling routine should 
//      provide a zero as the initCRC when a new CRC value is computed on a new
//      segment of data.
//
//    Input Parameters: initCRC -- the initial CRC value 
//
//						pData  -- a pointer to the data area that needs to be
//								  checksumed. The bit and byte ording is assumed
//								  to be LSB.
//						nBytes - a total number of data bytes needs to be CRC
//								 checksumed.
//
//    Output Parameters:
//
//    Return Values: a 32 bit unsigned CRC checksum value on the provided data.  
//
//    Pre-Conditions:
//
//    Miscellaneous: This algorithem is most efficient for a LSB bit first data feed.
//
//    Requirements:  
//
//******************************************************************************/
ULONG CRC::S_CompCRC32(ULONG initCRC, UCHAR *pData, ULONG nBytes)
{
		while(nBytes--)
        {
			initCRC = S_CRC32Tab[ ( initCRC ^ *pData++) & (0xFFL)]
						^ ( initCRC >> 8 );
	    }
		return(initCRC);
}


//******************************************************************************/
//    Operation Name: S_CompCheckSum(UCHAR *pData, SHORT size)
//
//    Processing:  Computes a character long checksum byte for a given
//				   data packet. Each Character is added and the sum's
//			       one's complement value is returned.
//				   
//    Input Parameters: None.
//
//    Output Parameters: None.
//
//    Return Values: one's complement of an additional result of all the data bytes.
//
//    Pre-Conditions: 
//
//    Miscellaneous: Performance of this routine is important.
//
//    Requirements: None.  
//
/******************************************************************************/
UCHAR
CRC::S_CompCheckSum(UCHAR *pData, SHORT size)
{
UCHAR  checkSum = 0;

	while(size-- > 0 ) 
	{ 
		checkSum += *pData++; 
	}

	return(checkSum);
}
