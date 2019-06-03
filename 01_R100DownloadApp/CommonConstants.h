/******************************************************************************/
//$COMMON.H$
//   File Name:  CommonConstants.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains no class declaration.  It is a repository for 
//      common constants used throughout the system.
// 
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      05    11/19/97  R.P. Rush       	Morph
//      Description: Made all const vars static const vars.  This keeps multiple
//					 copies of the variable from being generated everytime it is
//					 included.
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      05    06/02/98  Dan Seibert         Morph
//      Description: Moved BIT_MASK data into fixeddata.h
//
//      Rev:  Date:     Engineer:           Project:
//      04    10/20/97  K.A. McGuigan       Morph
//      Description: Added min and max flow and pressure definitions.
//
//      Rev:  Date:     Engineer:           Project:
//      03    06/11/97  K.A. McGuigan       Morph
//      Description: Added definition of SEC_PER_MIN.
//
//      Rev:  Date:     Engineer:           Project:
//      02    05/21/97  K.A. McGuigan       Morph
//      Description: Added definition of TICKS_PER_MIN.
//
//      Rev:  Date:     Engineer:           Project:
//      01    01/28/97  K.A. McGuigan       Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef COMMONCONSTANTS_H
#define COMMONCONSTANTS_H
#include "Standard.h"

#define NU_DEBUG 1

#if !defined(_WINDOWS) && !defined(GUI_STANDALONE)
extern "C" {
#include "Nucleus.h"
}
#else
typedef unsigned long   UNSIGNED;
#endif
       
// The scale factor is used to scale all settings and internal representations
// of Pressure, Volume and Flow.
static const LONG SCALE = 100;

#ifdef _MSC_VER
static const UNSIGNED TICKS_PER_SEC = 18;   // approximately 18 ticks per sec in dos
static const UNSIGNED MS_PER_TICK = 56;     // so, about 56 ms per tick in DOS
#else
#ifdef V851_SIM
static const UNSIGNED MS_PER_TICK = 100; 
#else
static const UNSIGNED MS_PER_TICK = 1;
#endif
static const UNSIGNED TICKS_PER_SEC = 1000/MS_PER_TICK;
static const UNSIGNED TICKS_PER_HALF_SEC = TICKS_PER_SEC / 2;
#endif

static const UNSIGNED MS_PER_SEC = 1000;   // number of milliseconds in one second.
static const UNSIGNED SEC_PER_MIN = 60;    // number of seconds in one minute

static const UNSIGNED TICKS_PER_MIN = 60 * TICKS_PER_SEC; // number of TICKS in 1 min.

// Minimum and Maximum flow rates in ml/sec, scaled
static const LONG MIN_FLOW_RATE = 0;
static const LONG MAX_FLOW_RATE = (200 * SCALE * 1000) / SEC_PER_MIN;   // 200 Lpm

// Minimum and Maximum Pressure Readings in cmH2O, scaled
static const SHORT MIN_PRESSURE_READING = (-100) * SCALE;
static const SHORT MAX_PRESSURE_READING = (200) * SCALE;

static const USHORT MAX_X_COORDINATE = 639;
static const USHORT MAX_Y_COORDINATE = 479;

#endif  // COMMONCONSTANTS_H

