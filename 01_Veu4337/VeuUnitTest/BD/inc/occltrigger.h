/******************************************************************************/
//$COMMON.H$
//   File Name:  OcclTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      This file contains the declaration for the Occlusion Trigger class.
//      This class is derived from the Trigger class.
/******************************************************************************/
#ifndef OCCLTRIGGER_H
#define OCCLTRIGGER_H

#include "Trigger.h"

// Types of occlusion detection
enum E_OcclId
{
    ePvFlowRes,     // Pressure versus flow resistance
    ePexhPdiff,     // Exhalation Pressure change, Pinh/Pexh difference
    eNumOcclIds
};

// The TimeCondition structure defines the structure used to keep track of
// conditions that must be present for a specified amount of time.
struct TimedCondition
{
    UNSIGNED TimeFirstMet;
    UNSIGNED TimeLimit;
    E_Bool IsCondMet;
};

class OcclTrigger : public Trigger
{
public:
    //pointer to the instance of this class
    static OcclTrigger* S_GetInstance(void);

    //Constructor
    OcclTrigger (E_TriggerId triggerId, LONG resistance, LONG offset);

    //retrieves the current pressure measured in the exhalation, inhalation
    virtual E_Bool CheckCondition (SHORT xexhPress, SHORT yinhPress);

    //Enable trigger
    virtual void EnableTrigger (void);

    //Process action base on trigger id
    virtual inline void ProcessTrigger (Mode* mode);

    //$COMMON.ATTRIBUTE$
    //  Name: OcclusionCondition
    //  Description:  Array of TimedCondition structures.  There are
    //      multiple checks for determining if the circuit is occluded.
    //      This array contains the time status of each of thos checks.
    //  Units: None
    //  Range: n/a
    TimedCondition OcclCondition[eNumOcclIds];

    //Constructor
    OcclTrigger (void);
    //Copy constructor
    OcclTrigger (const OcclTrigger&);

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of OcclTrigger
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static OcclTrigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: LastExhPress
    //  Description:  Exhalation pressure measured the last time the
    //      trigger condition was checked.  One of the occlusion checks
    //      determines compares the current exhalation pressure with the
    //      one measured during the previous cycle.
    //  Units: cmH2O * SCALE
    //  Range: -100 to 200 cmH2O, scaled
    SHORT LastExhPress;

    //$COMMON.ATTRIBUTE$
    //  Name: Resistance
    //  Description:  Resistance in the tubing downstream of the pressure
    //      transducer.
    //  Units: (cmH2O / ml per sec) * SCALE
    //  Range: n/a
    LONG Resistance;

    //$COMMON.ATTRIBUTE$
    //  Name: Offset
    //  Description:  Pressure offset margin allowed in the comparison
    //      of the difference between the inhalation and exhalation pressure
    //      transducers and the flow resistance downstream of the pressure
    //      transducer.
    //  Units: cmH2O * SCALE
    //  Range: n/a
    LONG Offset;
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessOcclusionAlarmTrigger() operation of 
//      the active mode.
//
//    Input Parameters:
//      mode - pointer to the currently active Mode object
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
inline void OcclTrigger::ProcessTrigger (Mode* mode)
{
    // Invoke the operation to handle the Occlusion trigger in the current mode.
    mode->ProcessTriggerID(eOcclusionTrigger);

}   // end ProcessTrigger()

#endif // OcclTrigger_H
