//******************************************************************************/
//$COMMON.H$
//   File Name:  CRC.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: CRC - Provides CRC checksum computation capabilities.
//
//
//   Revision History:
//      Rev:  Date:   	Engineer:     		Project:
//      01    03/05/97    Jung Verheiden   			  Morph
//      Description: Initial Revision
//
//******************************************************************************/

#ifndef	CRC_H
#define CRC_H
#include "standard.h"

class CRC
{
	public:
		static USHORT S_CompCRC16(USHORT initCRC, UCHAR *pData, ULONG nBytes);
		static ULONG S_CompCRC32(ULONG initCRC, UCHAR *pData, ULONG nBytes);
		static UCHAR S_CompCheckSum(UCHAR *pData, SHORT size);

	private:
		static  USHORT S_CRC16Tab[256];
		static  ULONG S_CRC32Tab[256];
};

extern "C"  ULONG CompCRC32(ULONG initCRC, UCHAR *pData, ULONG nBytes);
extern "C"  USHORT CompCRC16(USHORT initCRC, UCHAR *pData, ULONG nBytes);

#endif //CRC_H
