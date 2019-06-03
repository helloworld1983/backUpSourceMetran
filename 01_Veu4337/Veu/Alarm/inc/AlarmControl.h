/******************************************************************************/
//
//$COMMON.H$
//    File Name: AlarmControl.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: AlarmControl
//
//  The AlarmControl class category interfaces with  the DataIO class to perform
//  low level operations for silencing alarm horn and resetting alarms in the
//	auto-reset priority back to normal.  
//
/******************************************************************************/
#ifndef ALARMCONTROL_H
#define ALARMCONTROL_H

//Utils includes
#include "AlarmInterface.h"
#include "DebugTrace.h"
#include "CommonTypes.h"

//alarm includes
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include "BdInterface.h"

#include "Timer.h"

class AlarmControl: public Timer
{

    public:
        // static instance of AlarmControl
        static inline AlarmControl* S_GetInstance (void);

        // Key send with key id
        virtual E_ReturnStatus Send (E_DeviceId id);

        // gets reset flag using a semaphore
        inline E_Bool GetResetFlag (void);

        // gets silence flag using a semaphore
        inline E_Bool GetSilenceFlag (void);

        // sets reset flag using a semaphore
        inline void SetResetFlag (E_Bool val);

        // sets silence flag using a semaphore
        inline void SetSilenceFlag (E_Bool val);

        // turnoff silence duration
        void CancelSilence (void);

        //Return status of silence button
        E_Bool GetSilenceButtonStatus(void);

        //Set silence button status
        void SetSilenceButtonStatus(E_Bool val);

    protected:

    private:
        // static instance of alarm controls
        static AlarmControl* S_Instance;

        // constructor
        AlarmControl (void);

        // Virtual routine to call upon expiration.

        void ExpirationRoutine (void);

        //$COMMON.ATTRIBUTE$
        //    Name: ResetFlag
        //
        //    Description: flag used to indicate a reset event occurred
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool ResetFlag;

        //$COMMON.ATTRIBUTE$
        //    Name: SilenceFlag
        //
        //    Description: flag used to indicate a silence event occurred
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool SilenceFlag;

        //$COMMON.ATTRIBUTE$
        //    Name: SilenceOn
        //
        //    Description: Flag used for silence button activation
        //
        //    Units: None
        //
        //    Range: N/A
        //
        E_Bool SilenceOn;

};
// AlarmControl

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
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
//      * - pointer to the object instantiated from this class
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
AlarmControl* AlarmControl::S_GetInstance (void)
{
    //Create object if it is null
    if(S_Instance == NULL)
    {
        S_Instance = new AlarmControl ();
    }

    //Check opinter is null or not
    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetResetFlag   
//
//    Processing: returns the value of the reset flag 
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: E_Bool ResetFlag
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements:    
//
/******************************************************************************/
inline E_Bool AlarmControl::GetResetFlag (void)
{
    return (ResetFlag);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetResetFlag   
//
//    Processing: sets the value of the reset flag
//
//    Input Parameters: val value to set reset flag
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements:    
//
/******************************************************************************/
inline void AlarmControl::SetResetFlag (E_Bool val)
{
    //Suspend all system
    vTaskSuspendAll ();
    //Set Flag
    ResetFlag = val;
    //Resume all system
    xTaskResumeAll ();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GetSilenceFlag   
//
//    Processing: Gets the silence flag
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: E_Bool silence flag
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements:    
//
/******************************************************************************/
inline E_Bool AlarmControl::GetSilenceFlag (void)
{
    return (SilenceFlag);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetSilenceFlag   
//
//    Processing: sets the silence flag 
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: E_Bool val
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements:    
//
/******************************************************************************/
inline void AlarmControl::SetSilenceFlag (E_Bool val)
{
    //Suspend all system
    vTaskSuspendAll ();
    //Set flag
    SilenceFlag = val;
    //Resume all system
    xTaskResumeAll ();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: IsSilenceButtonActivate
//
//    Processing: status of silence button
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: E_Bool SilenceOn
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements:
//
/******************************************************************************/
inline E_Bool AlarmControl::GetSilenceButtonStatus (void)
{
    return SilenceOn;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SetSilenceButtonStatus
//
//    Processing: set silence button
//
//    Input Parameters: E_Bool val
//
//    Output Parameters: None
//
//    Return Values: E_Bool val
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements:
//
/******************************************************************************/
inline void AlarmControl::SetSilenceButtonStatus (E_Bool val)
{
    SilenceOn = val;
}

#endif // ALARMCONTROL_H
