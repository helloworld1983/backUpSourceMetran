#ifndef UTIL_H
#define UTIL_H

#ifdef UTIL_C
#define EXTERN
#else
#define EXTERN		extern
#endif

#include "nucpp.h"
#include "CommonTypes.h"

EXTERN long  BinarySearch(const long[], long, long, long);
EXTERN void Wait(ULONG count);
EXTERN void CalcChecksum(USHORT * checksum,  UCHAR * addr,  USHORT count);
EXTERN UNSIGNED CalcElapsedTime(UNSIGNED startTime);
EXTERN E_Bool InDiagnosticMode();

extern "C"
{
EXTERN char * ltoa(long n, char s[],int radix);
extern E_Bool  TestAndSet(UCHAR *addr);
extern void    EnableMaskableInt(UCHAR *addr);
extern void    DisableMaskableInt(UCHAR *addr);
extern LONG    AddSat(LONG, LONG);
}


#undef EXTERN
#endif		
