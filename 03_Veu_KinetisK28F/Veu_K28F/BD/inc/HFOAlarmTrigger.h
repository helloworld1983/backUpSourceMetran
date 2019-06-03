/******************************************************************************/
//$COMMON.H$
//   File Name:  HFOAlarmTrigger.h
//
//   Class: 
//      This file contains the declaration for the HighMAP_2 Alarm Trigger
//      class.The HighMAP_2 Alarm Trigger is derived from Trigger class 
//
//   Revision History:
/******************************************************************************/
#ifdef HFO_SYSTEM
#ifndef HFOALARMTRIGGER_H
#define HFOALARMTRIGGER_H

#include "Trigger.h"
#include "guiinterface.h"
/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class HFOAlarmTrigger : public Trigger
{
public:
    static inline HFOAlarmTrigger* S_GetInstance(void);
    virtual E_Bool CheckCondition(void);
    virtual void DisableTrigger();
    virtual inline void ProcessTrigger(Mode* mode);
    inline void SetInactiveTimer(USHORT value){ InactiveTimer = value;}
    inline USHORT GetInactiveTimer(void){return InactiveTimer;}

    inline void DisableAmpChecking(){IsAmpChecking = false;}
    inline void EnableAmpChecking(){IsAmpChecking = true;}

    //When user changes Map setting, delay Map alarm checking
    inline void SetDelayMapAlarmCheck(){delayMapAlarmCheck = 500;}
    //    virtual EnableTrigger();
protected:

private:
    HFOAlarmTrigger(void);
    HFOAlarmTrigger(const HFOAlarmTrigger&);

    void CheckAmplitudeAlarm();
    void CheckMapAlarm();
    void CheckSIAlarm();
    void CheckProximalLeakage();

    E_Bool CheckExtremeHighMapAlarm();
    E_Bool CheckExtremeHighSIAlarm(E_GUIEventID alarmType = eExtremeHighSIPress);

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HFOAlarmTrigger* S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: InactiveTimer
    //  Description:  Keep track the time of being inactive.
    //  Units: None
    //  Range: n/a
    USHORT InactiveTimer;


    int Index;			//counter for sample buffer
    bool IsFlatPattern;			//flag for flat pattern
    bool IsFlowOverLimit;		//flag for flow over limit  if you have disconnect ET tube, flow patter have overshoot and flat, but overshoot is not stable.
    int Counter500msec;		//counter for 1sec
    int FlowOverLimitCnt;		//counter for flow over limit
    int CheckCnt;
    int AlarmActiveCnt;		//alarm active counter
    int AlarmResetCnt;		//alarm auto reset counter

    bool IsAmpChecking;

    short delayMapAlarmCheck;
};      


/******************************************************************************/
//      INLINE OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing: 
//      This operation invokes the ProcessHighMAP_2AlarmTrigger() operation of 
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
inline void HFOAlarmTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the High MAP_2 trigger in the current mode.
    mode->ProcessHighMAP_2AlarmTrigger();

}   // end ProcessTrigger()


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the HFOAlarmTrigger object, it invokes
//      HFOAlarmTrigger::S_GetInstance(), which returns a pointer
//      to the HFOAlarmTrigger object.
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
//      HFOAlarmTrigger* - pointer to the instance of this class
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
inline HFOAlarmTrigger* HFOAlarmTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HFOAlarmTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end HFOAlarmTriger()

#endif // HFOALARMTRIGGER_H
#endif // HFO_SYSTEM
