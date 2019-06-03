/******************************************************************************/
//$COMMON.H$
//   File Name:  Standard.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains no class declaration.  It is a repository for 
//      standard basic types and constants used throughout the system. 
//
//   Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      03    02/03/98  K. A. McGuigan      Morph
//      Description: SCR 198 - Added comment that eSuccess in the E_ReturnStatus
//      enumeration must remain at 0.  Otherwise, the UpdateAlarmStatus() 
//      operations in the BD Mode classes must be revisited.
//
//      Rev:  Date:     Engineer:           Project:
//      02    07/31/97  R. L. Stephenson    Morph
//      Description: Changed so that FMACRO.h is not included if we are
//      doing an OTP build.
//
//      Rev:  Date:     Engineer:           Project:
//      01    01/31/97  K.A. McGuigan       Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef STANDARD_H
#define STANDARD_H


// Basic types
#ifndef CHAR                    // prevent conflict with Nucleus use of CHAR
typedef char CHAR; 
#endif
typedef long LONG;
typedef unsigned long ULONG;
typedef short SHORT;
typedef unsigned short USHORT;
typedef ULONG BIT32;
typedef USHORT BIT16;
typedef unsigned char UCHAR;

// Boolean
enum E_Bool 
{ 
	eFalse, 
	eTrue 
};

// Return status is used as an operation return value to indicate if the 
// operation was successful or not.
enum E_ReturnStatus
{
    eSuccess = 0,   // eSuccess must remain at 0 
    eError,
    eCompleted,
    eEmpty          // indicates if a the event log is empty (Devices)
};


// NULL is defined using a #define instead of a constant definition to avoid
// type checking conflicts.
#ifndef NULL
#define NULL 0L
#endif

#ifndef OTP_BUILD
#include "FMacro.h"
#endif

#endif // STANDARD_H
