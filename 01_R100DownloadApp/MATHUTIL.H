//*****************************************************************************/
//$COMMON.H$
//   File Name:  MATHUTIL.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: MATHUTIL
//
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      01    08/25/97  Lafreniere          Morph
//      Description: Initial Revision
//
//*****************************************************************************/

#ifndef MATHUTIL_H
#define MATHUTIL_H

#include "CommonTypes.h"

/* Function Prototypes */
long avg(long average, long value, unsigned short numberOfValues);
long avg(long average1, long average2);
long isqrt(long);
long ipow(long, long);

long interpolate(long x, long x1, long y1, long x2, long y2);

void tofrac(ULONG, LONG, CHAR*);
void tofrac(LONG, LONG, CHAR*);
inline void tofrac(USHORT value, LONG scale, CHAR* str)
{
    tofrac((ULONG)value, scale, str);
}

inline void tofrac(SHORT value, LONG scale, CHAR* str)
{
    tofrac((LONG)value, scale, str);
}

#endif	
