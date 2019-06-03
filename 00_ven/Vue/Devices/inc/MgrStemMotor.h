//******************************************************************************//
//$COMMON.H$
//    Filename: MgrStemMotor.h
//    Copyright 1996 InVentived Technologies, Inc. All Rights Reserved.
//
//    Class:  StepMotor
//    This class is the class for the Step Motor Controller Devices
//******************************************************************************//
#ifndef INC_MGRSTEMMOTOR_H_
#define INC_MGRSTEMMOTOR_H_

#include "Stepmoto.h"

class MgrStemMotor
{
public:
    // static instance of MgrStemMotor class
    static MgrStemMotor *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //$COMMON.ATTRIBUTE$
    //    Name: AirValve
    //
    //    Description: Pointer to instance of air valve
    //    Units: None
    //
    //    Range: N/A
    //
    static StepMotor*  AirValve;

    //$COMMON.ATTRIBUTE$
    //    Name: O2Valve
    //
    //    Description: Pointer to instance of 02 valve
    //    Units: None
    //
    //    Range: N/A
    //
    static StepMotor*  O2Valve;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhValve
    //
    //    Description: Pointer to instance of exhalation valve
    //    Units: None
    //
    //    Range: N/A
    //
    static StepMotor*  ExhValve;

    //$COMMON.ATTRIBUTE$
    //    Name: HfoValve
    //
    //    Description: Pointer to instance of inhalation valve
    //    Units: None
    //
    //    Range: N/A
    //
    static StepMotor*  HfoValve;

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of MgrStemMotor class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static MgrStemMotor *S_Instance;

    //MgrStemMotor constructor
    MgrStemMotor(void);
};

#endif /* INC_MGRSTEMMOTOR_H_ */
