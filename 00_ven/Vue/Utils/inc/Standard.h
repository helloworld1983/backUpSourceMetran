/******************************************************************************/
//$COMMON.H$
//   File Name:  Standard.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains no class declaration.  It is a repository for 
//      standard basic types and constants used throughout the system. 
//
/******************************************************************************/
#ifndef STANDARD_H
#define STANDARD_H

// NULL is defined using a #define instead of a constant definition to avoid
#ifndef NULL
#define NULL                    (0L)
#endif //end define NULL

// Basic types
#ifndef CHAR
typedef char CHAR;
#endif	//end define CHAR

typedef long LONG;

typedef unsigned long ULONG;

typedef short SHORT;

typedef unsigned short USHORT;

typedef ULONG BIT32;
typedef USHORT BIT16;

typedef unsigned char UCHAR;
typedef unsigned long UNSIGNED;

#ifndef STATUS
typedef int STATUS;
#endif //end define STATUS

// Boolean
enum E_Bool
{
    eFalse, eTrue
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

#define Q_SUCCESS       0
#define QUEUE_EMPTY        -45
#define QUEUE_FULL         -46
#define QUEUE_ERROR        -47

#endif // STANDARD_H
