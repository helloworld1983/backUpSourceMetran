//$COMMON.H$
//    File Name:  MorphImageHeader.h
//    Copyright 1999  Respironics, Inc.  All Rights Reserved.
//
//    Class: MorphImageHeader
//
//
//    Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      01    04/07/99  eblume              Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  MORPHIMAGEHEADER_H
#define  MORPHIMAGEHEADER_H

#include "OTPPartNumber.h"

struct MorphImageHeader
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

#endif