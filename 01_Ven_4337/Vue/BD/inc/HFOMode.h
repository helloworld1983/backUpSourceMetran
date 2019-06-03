/******************************************************************************/
//$COMMON.H$
//   File Name:  HFOMode.h
//
//   Class: 
//      This file contains the class declaration for the HFO Mode

/******************************************************************************/
#ifdef HFO_SYSTEM
#ifndef HFOMODE_H
#define HFOMODE_H

#include "NormBreathingMode.h"

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/

class HFOMode : public NormBreathingMode
{
public:
    static inline HFOMode* S_GetInstance(void);
    virtual void Enter(E_VentilationMode fromModeId);
    virtual void Exit(void);

    //Process action for each ID trigger
    virtual void ProcessTriggerID(E_TriggerId id);  //implement later when macro is active


    virtual void ProcessHighMAP_2AlarmTrigger(void);
    virtual void ProcessExtremeLowMapAlarmTrigger(void);
    virtual void ProcessSI10sTimerTrigger();
    virtual void ProcessResetSITimeTrigger();
    virtual void ProcessAutoSIOnTimeTrigger();
    virtual void ProcessAutoSIOffTimeTrigger();

protected:

private:
    HFOMode(void);
    HFOMode(const HFOMode&);
    virtual void UpdateAlarmStatus(void);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HFOMode* S_Instance;
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
//      access to the SpontMode object, it calls HFOMode::S_GetInstance(),
//      which returns a pointer to the HFOMode object.
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
//      HFOMode* - pointer to the object instantiated from this class
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
inline HFOMode* HFOMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)    
    {
        S_Instance = new HFOMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

#endif // HFOMODE_H
#endif // HFO_SYSTEM
