/******************************************************************************/
//$COMMON.H$
//   File Name:  Util.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      This file contains no class declaration.  It is a repository for
//      standard basic function used throughout the system.
/******************************************************************************/
#ifndef UTIL_H
#define UTIL_H

#include "CommonTypes.h"

//Return the time elapsed since the start of start time
unsigned long CalcElapsedTime (unsigned long startTime);

//returns whether or not the ventilator is operating in "Diagnostic Mode"
E_Bool InDiagnosticMode (void);

#endif  //end UTIL_H
