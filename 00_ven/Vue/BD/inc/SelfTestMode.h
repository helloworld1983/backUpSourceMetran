/******************************************************************************/
//$COMMON.H$
//   File Name:  SelfTestMode.h
//
//   Class: 
//      This file contains the class declaration for the Self Test Mode
//      which is derived from the AlarmBreathingMode base class.
//
//   Revision History:
//
/******************************************************************************/
#ifndef SELFTESTMODE_H
#define SELFTESTMODE_H

#include "Mode.h"
#include "OpRequest.h"

class SelfTestMode : public Mode
{
    public:
        //Pointer to the object instantiated from this class
        static SelfTestMode* S_GetInstance(void);

        //Process doing something when another mode enter and exit to SelfTestMode
        virtual void Enter(E_VentilationMode fromModeId);
        virtual void Exit(void);

        //Set BdEvent for current test ID
        inline void SetCurrentTest(E_BdEventId currentTest){CurrentTestId = currentTest;}

        //Get BdEvent id of current test ID
        inline E_BdEventId GetCurrentTest(void){return CurrentTestId;}

        //Process action for id trigger
        void ProcessTriggerID (E_TriggerId id);

    protected:

    private:
        //constructor for class
        SelfTestMode(void);

        //Copy constructor for class
        SelfTestMode(const SelfTestMode&);

        ////This operation simply performs error handling and should never be called
        virtual void UpdateAlarmStatus(void);

        //This operation simply performs error handling and should never be called
        void ProcessModeChangeTrigger(void);

        //$COMMON.ATTRIBUTE$
        //  Name: CurrentTestId
        //  Description:  Current test id
        //  Units: None
        //  Range: n/a
        E_BdEventId CurrentTestId;

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static SelfTestMode* S_Instance;
};

#endif // SELFTESTMODE_H
