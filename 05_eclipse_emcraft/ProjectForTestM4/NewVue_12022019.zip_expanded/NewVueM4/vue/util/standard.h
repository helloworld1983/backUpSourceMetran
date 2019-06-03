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
//      03    02/03/98  K. A. McGuigan      HUMX
//      Description: SCR 198 - Added comment that eSuccess in the E_ReturnStatus
//      enumeration must remain at 0.  Otherwise, the UpdateAlarmStatus()
//      operations in the BD Mode classes must be revisited.
//
//      Rev:  Date:     Engineer:           Project:
//      02    07/31/97  R. L. Stephenson    HUMX
//      Description: Changed so that FMACRO.h is not included if we are
//      doing an OTP build.
//
//      Rev:  Date:     Engineer:           Project:
//      01    01/31/97  K.A. McGuigan       HUMX
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef STANDARD_H
#define STANDARD_H

typedef char CHAR;
typedef long LONG;
typedef unsigned long ULONG;
typedef ULONG BIT32;
typedef ULONG UInt32;
typedef LONG  Int32;

typedef short SHORT;
typedef short Int16;
typedef unsigned short USHORT;
typedef USHORT BIT16;
typedef USHORT UInt16;

typedef unsigned char UCHAR;
typedef unsigned char UInt8;
typedef char Int8;

typedef unsigned long UNSIGNED;
typedef long SIGNED;
typedef unsigned char OPTION;
typedef int STATUS;

typedef  double DOUBLE;
typedef  char* STRING;
typedef  const char* C_STRING;
typedef  char STRINGELEM;
typedef  unsigned long ColorType;


enum BoolEnum{
    False = 0,
    True
};

// Return status is used as an operation return value to indicate if the
// operation was successful or not.
enum E_ReturnStatus{
    eSuccess = 0,   // eSuccess must remain at 0
    eError,
    eCompleted,
    eEmpty          // indicates if a the event log is empty (Devices)
};


#define Q_SUCCESS       0
#define QUEUE_EMPTY        -45
#define QUEUE_FULL         -46
#define QUEUE_ERROR        -47

#ifndef ULONG_MAX
#define ULONG_MAX          0xFFFFFFFFUL
#endif
// NULL is defined using a #define instead of a constant definition to avoid
// type checking conflicts.
#ifndef NULL
#define NULL 0L
#endif

#endif // STANDARD_H
