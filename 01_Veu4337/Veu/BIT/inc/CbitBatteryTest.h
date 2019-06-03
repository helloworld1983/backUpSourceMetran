/******************************************************************************/
//$COMMON.H$
//   File Name:  CbitBatteryTest.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The CbitBatteryTest class defines the Battery Test performed during CBIT 
//		and is derived from the Test base class.
/******************************************************************************/
#ifndef CBITBATTERYTEST_H
#define CBITBATTERYTEST_H

#include "CommonConstants.h"
#include "Test.h"

class CbitBatteryTest //: public Test, public Timer
{
    public:
        CbitBatteryTest(E_EventCode eventCode, HandlePtr handlePtr);
        
        virtual E_TestStatus Run(void);

    protected:

    private:
        CbitBatteryTest(void);
        CbitBatteryTest(const CbitBatteryTest&);

        //$COMMON.ATTRIBUTE$
        //  Name: RecheckCharging
        //  Description:  flag used to check manage check the charging LED
        //  Units: None
        //  Range: n/a
 		E_Bool RecheckCharging;

        // Virtual routine to call upon expiration.
        
        void ExpirationRoutine(void);

		// builds gui alarm event messages

		void BuildGUIMsg(E_GUIEventID id, E_AlarmLevel stat);

		// manages the charging LED 

		void CheckChargingThresholds(void);

		// manages the High Priority Low Internal battery alarm and LED

		void CheckHiAlarm(SHORT current, SHORT voltage);

		// manages the Low Priority Low Internal battery alarm and LED

		void CheckLowAlarm(SHORT current, SHORT voltage);

		// manages the External battery alarm

		void CheckExtBat(SHORT current, SHORT voltage);

		// clears all internal battery LEDs and Alarms

		void ClearBatAlarms(void);

};

#endif // CBITBATTERYTEST_H
