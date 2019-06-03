/******************************************************************************/
//$COMMON.H$
//   File Name:  HighPressMode.h
//
//   Class: 
//      This file contains the class declaration for the HFOHighMAP_2 Mode
//      which is derived from the NonVentMode base class.
/******************************************************************************/
#ifndef HIGHPRESSMODE_H
#define HIGHPRESSMODE_H

#include "AlarmBreathingMode.h"
//#include "guiinterface.h"

class HighPressMode : public AlarmBreathingMode
{
    public:
        //Pointer to the object instantiated from this class
        static HighPressMode* S_GetInstance(void);

        //Process doing something when another mode enters this mode and exits
        virtual void Enter(E_VentilationMode fromModeId);
        virtual void Exit(void);

    protected:

    private:
        //Constructor
        HighPressMode(void);

        //Copy constructor
        HighPressMode(const HighPressMode&);

        //Update alarm status
        virtual void UpdateAlarmStatus(void);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.
        //  Units: None
        //  Range: n/a
        static HighPressMode* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: FromModeId
        //  Description:  Change mode from FromModeId
        //  Units: None
        //  Range: n/a
        E_VentilationMode FromModeId;
};

#endif // HIGHPRESSMODE_H
