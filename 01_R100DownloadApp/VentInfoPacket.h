//******************************************************************************/
//$COMMON.H$
//   File Name:  VentInfo.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Structure: Ventilator Information packet 
//
//
//   Revision History:
//      Rev:  Date:       Engineer:             Project:
//      01    02/05/97    Jung Verheiden                 Morph
//      Description: Initial Revision
//
//******************************************************************************/

#ifndef VENTINFO_H
#define VENTINFO_H

//for VERSION_SIZE and PART_NUMBER_SIZE declarations
#include "OTPPartNumber.h"

struct VentInfo
{
    LONG MagicCookie;
    CHAR PartNumber[PART_NUMBER_SIZE];
    CHAR Version[VERSION_SIZE];
    LONG TotalBlocks;
    LONG TotalBytes;
    LONG CRCValue;
    CHAR OTPPartNumber[PART_NUMBER_SIZE];
    CHAR OTPVersion[VERSION_SIZE];
    LONG OTPCRCValue;
};
#endif //VENTINFO_H
