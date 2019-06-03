/******************************************************************************/
//$COMMON.H$
//   File Name:  NonVentPhase.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The non-ventilation Phase class declared in this file is derived from
//      the Phase class.  The non-ventilation phase is the breath phase entered
//      when the ventilator is in a non-ventilation mode.
/******************************************************************************/
#ifndef NONVENTPHASE_H
#define NONVENTPHASE_H

#include <Phase.h>
#include <Util.h>

// DEFAULT_MIN_NON_VENT_TIME defines the default minimum time that the 
// ventilator must remain in the phase.  
const UNSIGNED DEFAULT_MIN_NON_VENT_TIME = 200 / MS_PER_TICK;  // 200ms in ticks

class NonVentPhase : public Phase
{
public:
    //pointer to the object instantiated from this class
    static NonVentPhase* S_GetInstance (void);

    //Start phase
    virtual void Start (void);

    //End phase
    virtual void End (void);

    //Process Bd cycle
    virtual void ProcessBdCycle (void);

    //calculate the time elapsed since the start of the phase
    static inline UNSIGNED S_CalcTimeSincePhaseStart (void);

    //indicates if the ventilator is at least minExhTime into exhalation phase
    static E_Bool S_IsInUnrestrictedPhase (UNSIGNED minExhTime =
            DEFAULT_MIN_NON_VENT_TIME);

protected:

private:
    NonVentPhase (void);
    NonVentPhase (const NonVentPhase&);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static NonVentPhase* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: S_StartTime
    //  Description:  System time when the phase was entered
    //  Units: TICKS
    //  Range: 0 - maximum UNSIGNED value
    static UNSIGNED S_StartTime;

};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_CalcTimeSincePhaseStart()
//
//    Processing: 
//      This operation invokes the CalcElapsedTime() utility to calculate
//      the time elapsed since the start of the phase.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      UNSIGNED - time in system ticks elapsed since the start of the 
//          phase
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
inline UNSIGNED NonVentPhase::S_CalcTimeSincePhaseStart (void)
{

    // Return the time elapsed since the start of the exhalation phase.
    return (CalcElapsedTime (S_StartTime));

}   // end S_CalcTimeSincePhaseStart()

#endif // NONVENTPHASE_H
