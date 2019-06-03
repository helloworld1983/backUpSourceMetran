/******************************************************************************/
//$COMMON.H$
//   File Name:  CommonConstants.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains no class declaration.  It is a repository for 
//      common constants used throughout the system.
//
/******************************************************************************/
#ifndef COMMONCONSTANTS_H
#define COMMONCONSTANTS_H

#include "Standard.h"

#define NU_NO_SUSPEND           (1)
#define NU_SUCCESS              (0)
#define NU_SUSPEND              (1)

typedef unsigned long UNSIGNED;

// The scale factor is used to scale all settings and internal representations
// of Pressure, Volume and Flow.
static const LONG SCALE = 100;

static const UNSIGNED MS_PER_TICK = 1;

static const UNSIGNED TICKS_PER_SEC = 1000 / MS_PER_TICK;

static const UNSIGNED TICKS_PER_HALF_SEC = TICKS_PER_SEC / 2;

// number of milliseconds in one second.
static const UNSIGNED MS_PER_SEC = 1000;

// number of seconds in one minute
static const UNSIGNED SEC_PER_MIN = 60;

// number of TICKS in 1 min.
static const UNSIGNED TICKS_PER_MIN = 60 * TICKS_PER_SEC;

// Minimum and Maximum flow rates in ml/sec, scaled
static const LONG MIN_FLOW_RATE = 0;
static const LONG MAX_FLOW_RATE = (200 * SCALE * 1000) / SEC_PER_MIN; // 200 Lpm

// Minimum and Maximum Pressure Readings in cmH2O, scaled
static const SHORT MIN_PRESSURE_READING = (-100) * SCALE;
static const SHORT MAX_PRESSURE_READING = (200) * SCALE;

static const USHORT MAX_X_COORDINATE = 639;
static const USHORT MAX_Y_COORDINATE = 479;

#endif  // COMMONCONSTANTS_H

