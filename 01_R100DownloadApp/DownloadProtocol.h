//$COMMON.H$
//    File Name:  DownloadProtocol.h
//    Copyright 1999  Respironics, Inc.  All Rights Reserved.
//
//    Class:  This file contains no class.  It provides all declarations and
//      definitions which allow the download server and the ventilator to
//      communicate.  This
//      includes structures used to communicate image size and
//      version information as well as constants used for acknowledging
//      receipt or transmission of data and error codes. 
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  04/09/99 	Erik Blume			Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  DOWNLOADPROTOCOL_H
#define	 DOWNLOADPROTOCOL_H

//$COMMON.ATTRIBUTE$
//  Name: DOWNLOAD_PACKET_SIZE
//  Description:  The number of data bytes contained in one packet received
//  from the download server.
//  Units: None
//  Range: n/a
const SHORT DOWNLOAD_PACKET_SIZE = 256;

//The following is defined to be constants instead of enum since
// Green Hill's compiler and MS C++ Compiler's enums are handled
// differently and they are compatible on both environments.
const USHORT eFlashMemWriteError = 0;
const USHORT eVentInfoReceived = 1;
const USHORT eVentInfoSending = 2;
const USHORT eCheckSumError = 3;
const USHORT eCRCError = 4;
const USHORT eFlashEraseError = 6;
const USHORT eDataRecvOkay = 8;
const USHORT eCommError = 9;
const USHORT eFlashBurning = 11;
const USHORT eDownloadSuccess = 13;
const USHORT eBlockNumOutOfSync = 16;
const USHORT eWaitSerialNoInput = 18;


//$COMMON.ATTRIBUTE$
//  Name: ASCII_SYNC
//  Description:  The ASCII character value used as a sync between the
//  download server and the ventilator.
//  Units: None
//  Range: n/a
const CHAR ASCII_SYNC =  'X';


//$COMMON.ATTRIBUTE$
//   Name: PART_NUMBER_SIZE
//   Description: Number of characters required to store a Part Number
//   including NULL termination.
//   Units: N/A
//   Range: N/A
static const LONG PART_NUMBER_SIZE = 12;

//$COMMON.ATTRIBUTE$
//   Name: SERIAL_NUMBER_SIZE
//   Description: Number of characters required to store a Serial Number
//   including NULL termination.
//   Units: N/A
//   Range: N/A
static const LONG SERIAL_NUMBER_SIZE = 9 ;

//$COMMON.ATTRIBUTE$
//   Name: VERSION_SIZE
//   Description: Number of characters required to store a Version Number
//   including NULL termination.
//   Units: N/A
//   Range: N/A
static const LONG VERSION_SIZE     = 10;

struct OTPVersionInfo
{
    //$COMMON.ATTRIBUTE$
    //   Name: OTPPartNumber
    //   Description: Character string containing the part number for the 
    //   OTP based software.  Format is: xxx-xxxx-xx with a NULL termination.
    //   Units: N/A
    //   Range: N/A
    CHAR  OTPPartNumber[PART_NUMBER_SIZE];  

    //$COMMON.ATTRIBUTE$
    //   Name: OTPVersion
    //   Description: Character string containing the revision of the 
    //   OTP based software.  Format is: r.rr with a NULL termination.
    //   In order to support Validation Builds the version number needs to
    //   be long enough to store Xr.rr.nnn
    //   Units: N/A
    //   Range: N/A
    CHAR  OTPVersion[VERSION_SIZE];    

    //$COMMON.ATTRIBUTE$
    //   Name: ImageCRCValue
    //   Description: The computed 32-bit CRC for the OTP image.
    //   Units: N/A
    //   Range: N/A
    ULONG ImageCRCValue;
};


struct VersionInfo
{
    //$COMMON.ATTRIBUTE$
    //   Name: MagicCookie
    //   Description: Contains a magic cookie value that is checked to
    //   see if the contents of this structure are valid.
    //   Units: N/A
    //   Range: N/A
    ULONG MagicCookie;

    //$COMMON.ATTRIBUTE$
    //   Name: PartNumber
    //   Description: Character string containing the part number for the 
    //   Flash based software.  Format is: xxx-xxxx-xx with a NULL termination.
    //   Units: N/A
    //   Range: N/A
    CHAR  PartNumber[PART_NUMBER_SIZE];

    //$COMMON.ATTRIBUTE$
    //   Name: Version
    //   Description: Character string containing the revision of the 
    //   Flash based software.  Format is: r.rr with a NULL termination.
    //   In order to support Validation Builds the version number needs to
    //   be long enough to store Xr.rr.nnn
    //   Units: N/A
    //   Range: N/A
    CHAR  Version[VERSION_SIZE];        

    //$COMMON.ATTRIBUTE$
    //   Name: TotalFileBlocks
    //   Description: Total number of data packets (file blocks) for this
    //   specific image.
    //   Units: N/A
    //   Range: N/A
    ULONG TotalFileBlocks;

    //$COMMON.ATTRIBUTE$
    //   Name: TotalBytes
    //   Description: Total number of bytes contained in this image.
    //   Units: N/A
    //   Range: N/A
    ULONG TotalBytes;

    //$COMMON.ATTRIBUTE$
    //   Name: ImageCRCValue
    //   Description: The computed 32-bit CRC for the flash image.
    //   Units: N/A
    //   Range: N/A
    ULONG ImageCRCValue;

    //$COMMON.ATTRIBUTE$
    //   Name: OTPVersionInfo
    //   Description: Structure containing the OTP Part Number/Version/CRC
    //   information.  Identical to the structure stored in OTP memory.
    //   Units: N/A
    //   Range: N/A
    struct OTPVersionInfo  OTPInfo;  

};

// This structure represents each data packet that is received from
// the download server.
struct DataPacket
{
    //$COMMON.ATTRIBUTE$
    //   Name: SyncChar
    //   Description: Contains the syncronization character.  Used to
    //   check packet validity.
    //   Units: N/A
    //   Range: N/A
    CHAR  SyncChar;

    //$COMMON.ATTRIBUTE$
    //   Name: BlockNum
    //   Description: The block number for this packet.  Packets should be 
    //   transmitted in block number order. 
    //   Units: N/A
    //   Range: N/A
    USHORT BlockNum;

    //$COMMON.ATTRIBUTE$
    //   Name: Data
    //   Description: This array is used to hold the actual data received 
    //   from the server.  The array is declared of type USHORT since this
    //   corresponds to the physical size of the flash memory device.  The
    //   number of elements in the array is therefore the number of data
    //   bytes in a packet divided by the size of a USHORT (in bytes).
    //   Units: N/A
    //   Range: N/A
    USHORT Data[DOWNLOAD_PACKET_SIZE/sizeof(USHORT)];
    USHORT CRC;
};

struct DataAckPacket
{
    //$COMMON.ATTRIBUTE$
    //   Name: eMsg
    //   Description: Message number used to communicate with the server.
    //   Units: N/A
    //   Range: N/A
    USHORT eMsg;
};


struct BlockAckPacket
{
    //$COMMON.ATTRIBUTE$
    //   Name: eMsg
    //   Description: Message number used to communicate with the server.
    //   Units: N/A
    //   Range: N/A
    USHORT eMsg;

    //$COMMON.ATTRIBUTE$
    //   Name: BlockNum
    //   Description: Block number of the packet being acknowledged.
    //   Units: N/A
    //   Range: N/A
    USHORT BlockNum;
};

#endif