/******************************************************************************/
//$COMMON.CPP$
//   File Name:  UTIL.CPP
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains utility routines
//
//          Binary_Search()
//          Wait() - a routine to waste some time
//          ltoa() - a routine to convert a long number into a string
//
//   Interfaces: None
//
//   Restrictions:
//      None
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      04    04/10/98  Dan Seibert         Morph
//      Description: Moved frequently executed routines into v851 on-chip ROM
//          as a performance enhancement for Morph.
//          (See UtilOTP.cpp)
//
//      Rev:  Date:     Engineer:           Project:
//      03    02/23/98  R.P. Rush           Morph
//      Description: SCR-149. Added Code review comments
//
//      Rev:  Date:     Engineer:           Project:
//      02    02/19/98  Dan Seibert         Morph
//      Description:  Added InDiagnosticMode() utility function.
//      (was previously in BootUtil.cpp)
// 
//      Rev:  Date:     Engineer:           Project:
//      01    10/27/97  K.A. McGuigan       Morph
//      Description: 
//          Updated BinarySearch to check Start <= End instead of < End.  Also,
//          Updated BinarySearch to check Start <= End instead of < End.  Also,
//          added comments to BinarySearch().
//
/******************************************************************************/
#define UTIL_CPP
#include "util.h"
#include "CommonTypes.h"
#include "poststatus.h"

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Wait()
//
//    Processing: Waits count * 1000 loops; used when an OS wait sleep is not
//				  desireable.
//
//    Input Parameters: count
//
//    Output Parameters: None
//    Return Values: None
//    Pre-Conditions: None
//    Miscellaneous: None
//    Requirements:  
//
//******************************************************************************
void Wait(ULONG count)
{
   volatile ULONG  i;
   volatile ULONG  j;

   for (i=0; i<count; i++)
       for (j=0; j<0x1000; j++)
           ;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CalcCheckSum()
//
//    Processing: Sums up bytes into a checksum value.
//
//    Input Parameters:
//          checksum - starting checksum value
//          addr     - pointer to first item
//          count    - number of items to sum
//
//    Output Parameters:
//          checksum - updated sum
//          addr     - now points one past last item.
//
//    Return Values: None
//    Pre-Conditions: None
//    Miscellaneous: None
//    Requirements:  
//
//******************************************************************************
void CalcChecksum( USHORT *checksum, UCHAR *addr, USHORT count )
{
       for (; count>0; count--)
           *checksum += *addr++;           
} 



//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ltoa()
//
//    Processing: converts a long number to an ascii representation
//
//    Input Parameters: long n - number to be converted
//						char s[] - pointer of where answer is
//						int radix - radix of number to be converted.
//
//    Output Parameters: None
//    Return Values: long to ascii pointer
//    Pre-Conditions: None
//    Miscellaneous: None
//    Requirements:  
//
//******************************************************************************

extern "C"
{
char * ltoa(long n, char s[],int radix)
{
  long i=0, j, sign;
  char tmp;

  if ((sign=n)<0 && radix==10)
    n=-n;
  do
    {
      tmp = n%radix+'0';
      if (tmp>'9' && radix==16)
      tmp += 'a' - 0x3a;
      s[i++] = tmp;
    }
  while ((n/=radix)>0);

  if (sign<0 && radix==10)
    s[i++]='-';
  s[i]='\0';

  j = i - 1;
  for(i=0; i<j; i++,j--)
    {
      tmp = s[i];
      s[i] = s[j];
      s[j] = tmp;
    }

  return(s);
}

}


#undef UTIL_CPP
