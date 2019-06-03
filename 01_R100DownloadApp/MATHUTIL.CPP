//$COMMON.CPP$
//   File Name:  MATHUTIL.CPP
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: A repository of math utility functions.
//
//   Interfaces: None
//
//   Restrictions: None
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      03    04/10/98  Lafreniere          Morph
//      Description: SCR-288. Fixed bug the avg routine.
//
//      02    02/23/98  R.P. Rush           Morph
//      Description: SCR-149. Added Code review comments
//
//      Rev:  Date:     Engineer:           Project:
//      01    08/22/97  Lafreniere          Morph
//      Description: Initial Revision
//
/******************************************************************************/

#include "mathutil.h"		
#include "CoreStream.h"
#include "string.h"
#include "stdlib.h"

//*****************************************************************************/
//    Operation Name: avg()
//
//    Processing: This function provides a means to calculate an average for a 
//      set of values when called in succession. The average calculated 
//      by avg() is the arithmetic mean (the sum of the values divided by 
//      the number of values). The maximum supported values in a set is 65535.
//
//      This formula is unique in that an average can be calculated for any 
//      sequence of numbers without using floating point, and never exceeding 
//      the maximum size for the integral data type. 
//
//      For instance, suppose we wanted to average the numbers -2100000004, 
//      2100000000, 2100000004, and 2100000008 without floating point. You
//      cannot just add them all together and divide by 4 since the max size
//      for a LONG is 2147483642 and it will overflow. 
//
//      Example:
//
//      static const MAX_NUM = 3;
//      long numbers[MAX_NUM] = { 200, 100, 900 };
//
//      long average = 0;
//      for (USHORT seq = 1; seq<=MAX_NUM; seq++) {
//          average = avg(average, numbers[seq-1], seq);
//      }
//      // average will equal 400 when loop is done.
//
//    Input Parameters:
//      average - the current average of the sequence.
//      value - a new number to average into the set.
//      numberOfValues - the set number of value we are averaging into the set. 
//          A numberOfValues always starts at 1 and counts upward. Never pass
//          in a 0.
//
//    Output Parameters: None
//
//    Return Values:
//      long - the new calculated average.
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
long avg(long average, long value, unsigned short numberOfValues)
{
    return average - ((average / numberOfValues) - (value / numberOfValues));
}

//*****************************************************************************/
//    Operation Name: avg()
//
//    Processing: Averages two numbers.
//
//    Input Parameters:
//      average1 - first number to average.
//      average2 - second number to average.
//
//    Output Parameters: None
//
//    Return Values:
//      long - the average of the two numbers.
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
long avg(long average1, long average2)
{
    long calcAvg = 0;
    
    calcAvg = avg(calcAvg, average1, 1);
    return avg(calcAvg, average2, 2);
}           

//*****************************************************************************/
//    Operation Name: isqrt()
//
//    Processing: Computes the integer square root 
//
//    Input Parameters: x 
//
//    Output Parameters: None 
//
//    Return Values: isqrt - the integer square root
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
long isqrt(long x)
{
    long root = 0;
    long remainder = x;
    long squaredbit;

    if (x > 0)
    {
	squaredbit = (long) ((((unsigned long) ~0L) >> 1) &
			    ~(((unsigned long) ~0L) >> 2));

	// Form bits of the answer
	while (squaredbit > 0)
	{
		if (remainder >= (squaredbit | root))
		{
			remainder -= (squaredbit | root);
			root >>= 1;
			root |= squaredbit;
		}
		else
		{
			root >>= 1;
		}

		squaredbit >>= 2;
	}
    }

    return(root);
}

//*****************************************************************************/
//    Operation Name: ipow()
//
//    Processing: Computes the integer power 
//
//    Input Parameters: long base, long exp
//
//    Output Parameters: None
//
//    Return Values: long ipow 
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
long ipow(long base, long exp)
{
    long result = 1;

    for (; exp > 0; exp--)
    {
	result = result * base;
    }

    return(result);
}

//*****************************************************************************/
//    Operation Name: interpolate()
//
//    Processing: This interpolates between two values. Used by the flow rate 
//      calculation.
//
//    Input Parameters: long x, long x1, long y1, long x2, long y2
//
//    Output Parameters: None
//
//    Return Values: long interpolate
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
long interpolate(long x, long x1, long y1, long x2, long y2)
{
    long result;

    result = (y2 - y1) * (((x - x1) << 10) / (x2 - x1));
    result = result + 0x00000200;   // Round the result...
    result = result >> 10;
    result = y1 + result;

    return(result);
}

//*****************************************************************************/
//    Operation Name: tofrac()
//
//    Processing: Convert an ULONG value with its scale factor into a string. 
//      The scale factor is where the implied decimal point lies.
//
//      e.g. a value of 123 with a scale of 100 would return a string of "1.23".
//
//    Input Parameters:
//      value - the value to convert.
//      scale - the scale factor applied to the value.
//
//    Output Parameters:
//      str - the string buffer to place the result. The caller is responsible
//          for allocating enough space for the result. No error checking is
//          done to ensure the answer will fit.
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
void tofrac(ULONG value, LONG scale, CHAR* str)
{
    static const UCHAR MAX_LENGTH = 24;
    USHORT precision = 0;
    ULONG tempScale = scale;
    CHAR fracStr[MAX_LENGTH];

    CoreStream os(fracStr, MAX_LENGTH);

    // calc the non-factional half of the number
    os << value / scale << ".";

    // calculate the width precision based upon the scale factor
    while (tempScale / 10)
    {
        tempScale /= 10;
        precision++;
    }
    os.width(precision);

    // calc the fractional half of the number
    os << value % scale << ends; 

    // copy the result to the callers buffer
    strcpy(str, fracStr);
}

//*****************************************************************************/
//    Operation Name: tofrac()
//
//    Processing: Convert a LONG value with its scale factor into a string. The
//      scale factor is where the implied decimal point lies.
//
//      e.g. a value of -123 with a scale of 100 would return a string of 
//      "-1.23".
//
//    Input Parameters:
//      value - the value to convert.
//      scale - the scale factor applied to the value.
//
//    Output Parameters:
//      str - the string buffer to place the result. The caller is responsible
//          for allocating enough space for the result. No error checking is
//          done to ensure the answer will fit.
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements:  
//
//*****************************************************************************/
void tofrac(LONG value, LONG scale, CHAR* str)
{
    static const UCHAR MAX_LENGTH = 24;
    USHORT precision = 0;
    ULONG tempScale = scale;
    CHAR fracStr[MAX_LENGTH];

    CoreStream os(fracStr, MAX_LENGTH);

    // calc the non-factional half of the number
    if (value < 0)
        os << "-";
    os << labs(value / scale) << ".";

    // calculate the width precision based upon the scale factor
    while (tempScale / 10)
    {
        tempScale /= 10;
        precision++;
    }
    os.width(precision);

    // calc the fractional half of the number
    os << labs(value % scale) << ends; 

    // copy the result to the callers buffer
    strcpy(str, fracStr);
}




