/******************************************************************************/
//$COMMON.H$
//   File Name:  HFOExtremeLowMapMode.h
//
//   Class: 
//      This file contains the class declaration for the HFOExtremeLowMapMode Mode
//      which is derived from the NonVentMode base class.
//
//   Revision History:
//
/******************************************************************************/
#ifdef HFO_SYSTEM
#ifndef HFOEXTREMELOWMAPMODE_H
#define HFOEXTREMELOWMAPMODE_H

#include "AlarmBreathingMode.h"

class HFOExtremeLowMapMode : public AlarmBreathingMode
{
public:
    static inline HFOExtremeLowMapMode* S_GetInstance(void);
    virtual void Enter(E_VentilationMode fromModeId);
    virtual void Exit(void);

protected:

private:
    HFOExtremeLowMapMode(void);
    HFOExtremeLowMapMode(const HFOExtremeLowMapMode&);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id); //implement when HFO_SYSTEM is active

    virtual void ProcessExtremeLowMapAutoResetTrigger(void);
    virtual void UpdateAlarmStatus(void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HFOExtremeLowMapMode* S_Instance;
};


/******************************************************************************/
//      INLINE OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the HFOExtremeLowMapMode object, it invokes HFOExtremeLowMapMode::S_GetInstance(),
//      which returns a pointer to the HFOExtremeLowMapMode object.
//
//      If the object has not already been created, this method instantiates 
//      it and returns a pointer to the object.  If the object has already been 
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      HFOExtremeLowMapMode* - pointer to the object instantiated from this class
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
inline HFOExtremeLowMapMode* HFOExtremeLowMapMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)    
    {
        S_Instance = new HFOExtremeLowMapMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

#endif // HFOEXTREMELOWMAPMODE_H
#endif // HFO_SYSTEM
