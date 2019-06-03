/******************************************************************************/
//$COMMON.H$
//   File Name:  BatteryTask.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The BatteryTask class defines the Battery Task performed 
//		and is derived from the Task base class.
/******************************************************************************/
#ifndef BATTERYTASK_H
#define BATTERYTASK_H

#include "CommonConstants.h"
#include "Test.h"

class BatteryTask
{
    public:
        static inline BatteryTask* S_GetInstance(void);


    protected:
        virtual void Entry(void);

    private:
        BatteryTask(void);
        BatteryTask(const BatteryTask&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static BatteryTask* S_Instance;

		void Init(void);

		void ConfigureIntBat(void);

		void ConfigRunExternalBattery(void);

		void MonitorBatteryStatus(void);

		// builds gui alarm event messages

		void BuildGUIMsg(E_GUIEventID id, E_AlarmLevel stat);

		// manages the High Priority Low Internal battery alarm and LED

		void CheckHiAlarm(SHORT current, SHORT voltage);

		// manages the Low Priority Low Internal battery alarm and LED

		void CheckLowAlarm(SHORT current, SHORT voltage);

		// manages the External battery alarm

		void CheckExtBat(SHORT current, SHORT voltage);

		// clears all internal battery LEDs and Alarms

		void ClearBatAlarms(void);

};

#endif // BATTERYTASK_H
