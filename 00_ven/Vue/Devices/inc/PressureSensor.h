//******************************************************************************
//$COMMON.H$
//   File Name:  PressureSensor.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: PressureSensor
//    This class is the base class for the sensor devices.
//******************************************************************************
#ifndef INC_PRESSURESENSOR_H_
#define INC_PRESSURESENSOR_H_

#include "sensor.h"
#include "VirtualPressure.h"
#include "BdInterface.h"
#include "ExhPressureController.h"

class PressureSensor
{
public:
    // static instance of PressureSensor class
    static PressureSensor *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to exhalation sensor
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static Sensor * ExhPressSensor;

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of inhalation sensor
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static Sensor * InhPressSensor;

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of virtaul pressure sensor
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static VirtualPressureSensor * ActivePressureSensor;

    //$COMMON.ATTRIBUTE$
    //    Name: ExhController
    //
    //    Description: Pointer to instance of exhalation pressure sensor
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static ExhPressureController *ExhController;

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of PressureSensor class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static PressureSensor *S_Instance;

    //WatchDogTimer constructor
    PressureSensor(void);

};
#endif /* INC_PRESSURESENSOR_H_ */
