/******************************************************************************/
//$COMMON.H$
//    File Name: AlarmTask.h
//    Class: AlarmTask - Alarms Task
/******************************************************************************/
#ifndef ALARMTASK_H
#define ALARMTASK_H

#include "CommonTypes.h"

class AlarmTask
{
    public:
        // static instance of Alarm task
        static AlarmTask* S_GetInstance (void);

        // initialize all public access to domain objects
        void Init (void);

        //create alarm task
        void AlarmCreateTask (void);

        // Set alarm to be active or not active
        E_ReturnStatus SetIsActive (int alarmID, E_AlarmStatus stat);

        //get alarm status
        E_AlarmStatus GetIsActive (int alarmID);

        //Get level of alarm
        E_AlarmLevel GetLevel(int alarmID);

        //Set reset flag
        void SetResetFlag(E_Bool val);

        //clear all alarm
        void ClearAllAlarm(void);

        //clean alarm of HFO
        void ClearHFOAlarm(void);

    protected:

    private:

        //$COMMON.ATTRIBUTE$
        //    Name: S_Instance
        //
        //    Description: Pointer to instance of Alarm task
        //
        //    Units: None
        //
        //    Range: N/A
        //
        static AlarmTask* S_Instance;

        // Entry Point into Alarms Task
        static void Entry (void* pvParameters);

        // Alarms Task constructor
        AlarmTask (void);

};
// class AlarmsTask

#endif //ALARMTASK_H  

