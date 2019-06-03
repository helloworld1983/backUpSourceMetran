//******************************************************************************
//$COMMON.H$
//    Filename: virtualpressure.h
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Class:  VirtualPressureSensor
//      This class is a derived class of the Sensor base class.
//      This class provides an interface to the currently active Pressure Sensor
//    This lets the breath delivery subsystem and the exhalation controller
//    sample pressure readings from a consistent source.
//******************************************************************************
#ifndef    VIRTUAL_PRESSURE_H
#define    VIRTUAL_PRESSURE_H

#include "sensor.h"

class VirtualPressureSensor
{
public:
    //constructor
    VirtualPressureSensor(Sensor* initialSensor)
{
        Primary = initialSensor;
};

    void SetPrimary(Sensor* sensorPtr);

    //initialize for class
    void Init(void);

    // returns most recent obtained value
    SHORT GetLastReading(void);

protected:

    //$COMMON.ATTRIBUTE$
    //    Name:        Primary
    //    Description: Pointer to the currently active pressure sensor.
    //    Units: N/A
    //    Range: N/A
    Sensor* Primary;
};

#endif    // VIRTUAL_PRESSURE_H
