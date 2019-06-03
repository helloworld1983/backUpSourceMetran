/******************************************************************************/
//$COMMON.H$
//   File Name:  ExtremeLowMapAlarmTrigger.h
//
//   Class: 
//      This file contains the declaration for the HighMAP_2 Alarm Trigger
//      class.The HighMAP_2 Alarm Trigger is derived from Trigger class 
//
//   Revision History:
/******************************************************************************/
#ifdef HFO_SYSTEM
#ifndef EXTREMELOWMAPALARMTRIGGER_H
#define EXTREMELOWMAPALARMTRIGGER_H

#include "Trigger.h"
/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class ExtremeLowMapAlarmTrigger : public Trigger
{
    public:
        static inline ExtremeLowMapAlarmTrigger* S_GetInstance(void);
        virtual E_Bool CheckCondition(void);
        virtual inline void ProcessTrigger(Mode* mode);
        inline void SetInactiveTimer(SHORT value){ InactiveTimer = value;}
        virtual void PhaseInSettingChange(LONG map);
        virtual void EnableTrigger(void);

    protected:

    private:
        ExtremeLowMapAlarmTrigger(void);
        ExtremeLowMapAlarmTrigger(const ExtremeLowMapAlarmTrigger&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.
        //  Units: None
        //  Range: n/a
        static ExtremeLowMapAlarmTrigger* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: InactiveTimer
        //  Description:  Keep track the time of being inactive.
        //  Units: None
        //  Range: n/a
        SHORT InactiveTimer;

        //$COMMON.ATTRIBUTE$
        //  Name: MapLimit
        //  Description:  Map limit
        //  Units: None
        //  Range: n/a
        LONG MapLimit;

        //$COMMON.ATTRIBUTE$
        //  Name: MapSetting
        //  Description:  Map setting.
        //  Units: None
        //  Range: n/a
        LONG MapSetting;

        static USHORT activeCounter;
        static USHORT notActiveCounter;
};


/******************************************************************************/
//      INLINE OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ExtremeLowMapAlarmTrigger() operation of 
//      the HFO mode.
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
inline void ExtremeLowMapAlarmTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the High MAP_2 trigger in the current mode.
    mode->ProcessExtremeLowMapAlarmTrigger();

}   // end ProcessTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the ExtremeLowMapAlarmTrigger object, it invokes 
//      ExtremeLowMapAlarmTrigger::S_GetInstance(), which returns a pointer 
//      to the ExtremeLowMapAlarmTrigger object.
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
//      ExtremeLowMapAlarmTrigger* - pointer to the instance of this class
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
inline ExtremeLowMapAlarmTrigger* ExtremeLowMapAlarmTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ExtremeLowMapAlarmTrigger(); 
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end ExtremeLowMapAlarmTrigger()

#endif // EXTREMELOWMAPALARMTRIGGER_H
#endif // HFO_SYSTEM
