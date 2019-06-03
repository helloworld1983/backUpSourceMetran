#ifndef HPHIGHALARMTRIGGER_H
#define HPHIGHALARMTRIGGER_H
#include "Trigger.h"
#include "DebugTrace.h"

class HPHighAlarmTrigger : public Trigger
{
public:
    static inline HPHighAlarmTrigger* S_GetInstance(void);
    virtual E_Bool CheckCondition(void);
    virtual void DisableTrigger(void);
    virtual inline void ProcessTrigger(Mode* mode);
    inline void SetInactiveTimer(USHORT value){ InactiveTimer = value;}

    inline bool IsAlarmActive(){return alarmFlag;}

    inline void ClearAlarmFlag(){alarmFlag = false;}


    HPHighAlarmTrigger(void);
    HPHighAlarmTrigger(const HPHighAlarmTrigger&);

    LONG highAlarmDelay;
    LONG highAlarmClearDelay;

    LONG lowAlarmDelay;
    LONG lowAlarmClearDelay;

    //$COMMON.ATTRIBUTE$
    //  Name: InactiveTimer
    //  Description:  Keep track the time of being inactive.
    //  Units: None
    //  Range: n/a
    USHORT InactiveTimer;

    //$COMMON.ATTRIBUTE$
    //  Name: alarmFlag
    //  Description:  This flag is checked at the end of next
    //                P-High phase to clear High P-High Alarm
    //                if High P-High Alarm is active.
    //  Units: None
    //  Range: n/a
    bool alarmFlag;

    //$COMMON.ATTRIBUTE$
    //  Name: S_Instance
    //  Description:  Pointer to the single instance of this class.
    //  Units: None
    //  Range: n/a
    static HPHighAlarmTrigger* S_Instance;

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
//      access to the HPHighAlarmTrigger object, it invokes
//      HPHighAlarmTrigger::S_GetInstance(), which returns a pointer
//      to the HPHighAlarmTrigger object.
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
//      HPHighAlarmTrigger* - pointer to the instance of this class
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
inline HPHighAlarmTrigger* HPHighAlarmTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new HPHighAlarmTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end Hip2AlarmTrigger()
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger()
//
//    Processing:
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
inline void HPHighAlarmTrigger::ProcessTrigger(Mode* mode)
{
    // Invoke the operation to handle the HIP trigger in the current mode.
    mode->ProcessTriggerID(eHPHighAlarmTrigger);

}   // end ProcessTrigger()

#endif // HPHIGHALARMTRIGGER_H
