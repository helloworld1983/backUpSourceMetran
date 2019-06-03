#line 2 "btps.cpp"           // force __FILE__ to drop path name
/******************************************************************************/
//$COMMON.CPP$
//   File Name:  Btps.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the constants, static attributes and operations
//      used to compute BTPS compensation factors for delivered and exhaled
//      flows.
//
//   Interfaces:
//      Operations in this class retrieve values from NVRAM.  They also
//      invoke the math utility interpolate().
//
//   Restrictions:
//      Btps is a static class; there are no instantiations.
/******************************************************************************/
#include "Btps.h"
//#include "Nvram.h"
//#include "GuiInterface.h"
//#include "GuiTask.h"
//#include "mathutil.h"

/******************************************************************************/
//      CONSTANT DEFINITIONS
/******************************************************************************/

// Intercept and slope constants used to compute the BTPS correction factor
// for delivered air.  The correction factor is computed as:
//      y = mx + b   where y is the correction factor and x is the altitude.
// The slope defined here is actually 1/m.
// Note: if SCALE ever changes, BTPS_AIR_INTERCEPT and BTPS_AIR_INV_SLOPE
// must be recalculated.
static const LONG BTPS_AIR_INTERCEPT = 111;     // 1.109 scaled by 100
static const LONG BTPS_AIR_INV_SLOPE = 184;     // 1 / ((5.43 * 10**-5) * 100)

// Intercept and slope constants used to compute the BTPS correction factor
// for delivered oxygen.  The correction factor is computed as:
//      y = mx + b   where y is the correction factor and x is the altitude.
// The slope defined here is actually 1/m.
// Note: if SCALE ever changes, BTPS_O2_INTERCEPT and BTPS_O2_INV_SLOPE
// must be recalculated.
static const LONG BTPS_O2_INTERCEPT = 113;      // 1.128 scaled by 100
static const LONG BTPS_O2_INV_SLOPE = 167;      // 1 / ((5.98 * 10**-5) * 100)

// Exhaled flow correction depends upon both altitude and the delivered oxygen
// mix.   Altitude is fixed once ventilator operation begins (i.e. operator
// cannot change this value on the fly).  But, oxygen can be changed at any
// time.   At initialization, a value for S_BtpsExhAirFactor and 
// S_BtpsExhO2Factor is calculated based upon altitude.  When the operator
// changes %O2, these factors are used to linearly interpolate between the
// the 2 points.
static const LONG MIN_O2_SETTING = 21 * SCALE;
static const LONG MAX_O2_SETTING = 100 * SCALE;

// Intercept and slope constants used to compute the BTPS correction factor
// for exhaled air.  The correction factor is computed as:
//      y = mx + b   where y is the correction factor and x is the altitude.
// The slope defined here is actually 1/m.
// Note: if SCALE ever changes, BTPS_EXH_AIR_INTERCEPT and 
// BTPS_EXH_AIR_INV_SLOPE must be recalculated.
static const LONG BTPS_EXH_AIR_INTERCEPT = 109;     // 1.09 scaled by 100
static const LONG BTPS_EXH_AIR_INV_SLOPE = 192;     // 1 / ((5.22 * 10**-5)*100)

// Intercept and slope constants used to compute the BTPS correction factor
// for exhaled oxygen.  The correction factor is computed as:
//      y = mx + b   where y is the correction factor and x is the altitude.
// The slope defined here is actually 1/m.
// Note: if SCALE ever changes, BTPS_EXH_O2_INTERCEPT and
// BTPS_EXH_O2_INV_SLOPE must be recalculated.
static const LONG BTPS_EXH_O2_INTERCEPT = 105;    // 1.05 scaled by 100
static const LONG BTPS_EXH_O2_INV_SLOPE = 173;    // 1 / ((5.78 * 10**-5)*100)

// Define statics
LONG Btps::S_BtpsO2Factor = 0;
LONG Btps::S_BtpsAirFactor = 0;
LONG Btps::S_BtpsExhFactor = 0;
LONG Btps::S_BtpsExhAirFactor = 0;
LONG Btps::S_BtpsExhO2Factor = 0;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_ComputeBtpsFactors()
//
//    Processing: 
//      This operation is invoked when the BD Task starts to initialize the
//      BTPS compensation factors for delivered air and oxygen flows.  These
//      values are computed based upon altitude and do not change during normal
//      operation.  The exhaled compensation factor is also computed based
//      upon an oxygen setting of 60%.  This value is re-computed whenever
//      delivered oxygen mix changes.  
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      This operation is ONLY called at startup.  The handling of altitude
//      retrieval from NVRAM and the alarm handling of a corrupted altitude
//      value depend on this operation only being called at start-up.  If
//      it is ever desirable to call this during normal operation, altitude
//      should be stored as a BdSetting.
//
//    Requirements:  
//
/******************************************************************************/
void Btps::S_ComputeBtpsFactors (void)
{
//    LONG altitude;
//    BasicGUIEvent msg;
//    STATUS status;
//    static const LONG DEFAULT_ALTITUDE = 9999;   // 9999 feet, no scaling.

    // Retrieve altitude from NVRAM.    
//    altitude = systemConfigNvram->GetValue(eAltitudeCompensation);

    // If NVRAM is corrupted, use an altitude of 0
//    if (UNINITIALIZED_LONG_VALUE == altitude)
//    {
//        altitude = DEFAULT_ALTITUDE;

    // Build Alarm Msg
//        msg.ID = eAltitudeDefaultSetting;

    // Send message to GUI so that a dialog is displayed, indicating that a 
    // default value is in use.  Assert if not successful.
//        status = (GUITask::S_GetInstance())->Send(msg, NU_NO_SUSPEND);
//        ASSERTION(status == NU_SUCCESS);
//    }

    // Compute BTPS compensation factor for delivered oxygen based upon
    // altitude.  BTPS_O2_INTERCEPT is scaled by 100.
//    S_BtpsO2Factor = altitude / BTPS_O2_INV_SLOPE + BTPS_O2_INTERCEPT;

    // Compute BTPS compensation factor for delivered air based upon
    // altitude.  BTPS_AIR_INTERCEPT is scaled by 100.
//    S_BtpsAirFactor = altitude / BTPS_AIR_INV_SLOPE + BTPS_AIR_INTERCEPT;

    // Compute BTPS compensation factor for exhaled oxygen and air based upon
    // altitude.  Intercepts are scaled by 100.
//    S_BtpsExhO2Factor = altitude / BTPS_EXH_O2_INV_SLOPE + BTPS_EXH_O2_INTERCEPT;
//    S_BtpsExhAirFactor = altitude / BTPS_EXH_AIR_INV_SLOPE + BTPS_EXH_AIR_INTERCEPT;

    // Initialize this value based on 21% O2.
//    S_BtpsExhFactor = interpolate(MIN_O2_SETTING,
//                                  MIN_O2_SETTING,
//                                  S_BtpsExhAirFactor,
//                                  MAX_O2_SETTING,
//                                  S_BtpsExhO2Factor);

}   // end S_ComputeBtpsFactors()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_ComputeBtpsExhFactor()
//
//    Processing: 
//      This operation is invoked whenever delivered oxygen mix changes to
//      recompute the BTPS correction factor for exhaled flow based upon %O2.
//
//    Input Parameters:
//      pctO2 = %O2, scaled by SCALE
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void Btps::S_ComputeBtpsExhFactor (LONG pctO2)
{

//    S_BtpsExhFactor = interpolate(pctO2,
//                                  MIN_O2_SETTING,
//                                  S_BtpsExhAirFactor,
//                                  MAX_O2_SETTING,
//                                  S_BtpsExhO2Factor);

}   // end S_ComputeBtpsExhFactor()
