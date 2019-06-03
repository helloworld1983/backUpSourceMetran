//******************************************************************************
//$COMMON.H$
//   File Name:  sensor.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Sensor
//    This class is the base class for the sensor devices.
//    The derived classes are FlowSensor, PressureSensor, and FIO2Sensor
//******************************************************************************
#ifndef    SENSOR_H
#define    SENSOR_H

#include "CommonTypes.h"
#include "BdInterface.h"

#define VOLT_ERROR  255

class Sensor
{
public:
    //Constructor
    Sensor (E_DeviceId  id,     // Sensor Id #
            UCHAR   address,     // A2D Channel corresponding to sensor
            float   volts_lo,    // Lower voltage limit of sensor (Volts * 1000)
            float   volts_hi,    // Upper voltage limit of sensor (Volts * 1000)
            float   units_lo,    // Lower reading limit in Engineering Units
            float   units_hi);   // Upper reading limit in Engineering Units

    // Obtains current reading from sensor.
    SHORT GetCurrentReading(void);

    //Get last volt
    float GetLastVolt(void);

    //get current volt
    float GetCurrentVolt(void);

    // returns most recent obtained value
    SHORT  GetLastReading(void);

    // Instanciates and Initialized objects.
    virtual void S_Init(void);


protected:
    //Error Handling routine
    void HandleBadSensor(E_DeviceId  Deviceid);

    //Constructor
    Sensor(void);

    //$COMMON.ATTRIBUTE$
    //    Name:        Address
    //    Description: A2D Channel number of sensor input
    //    Units: None
    //    Range: 0 - 15
    UCHAR         Address;

    //$COMMON.ATTRIBUTE$
    //    Name:        Counts
    //    Description: Holds A2D Counts obtained from A2D Converter
    //    Units: None
    //    Range: 0 - 4095
    //USHORT        Counts;

    //$COMMON.ATTRIBUTE$
    //    Name:        CurrentReading
    //    Description: Contains current reading in engineering units
    //    Units: object dependent
    //    Range: object dependent
    SHORT         CurrentReading;

    //$COMMON.ATTRIBUTE$
    //    Name:       ConversionFactor
    //    Description:Multiplier to convert counts to engineering units
    //    Units: object dependent
    //    Range: object dependent
    float           ConversionFactor;

    //$COMMON.ATTRIBUTE$
    //    Name:       MinCounts
    //    Description:Minimum valid count reading expected from A2D
    //              converter
    //    Units: object dependent
    //    Range: object dependent
    float         MinVolt;

    //$COMMON.ATTRIBUTE$
    //    Name:       MaxCounts
    //    Description:Maximum valid count reading expected from A2D
    //              converter
    //    Units: object dependent
    //    Range: object dependent
    float         MaxVolt;

    //$COMMON.ATTRIBUTE$
    //    Name:        MinUnits
    //    Description: Minimum valid reading in engineering units.
    //    Units: object dependent
    //    Range: object dependent
    float         MinUnits;

    //$COMMON.ATTRIBUTE$
    //    Name:        MaxUnits
    //    Description: Maximum valid reading in engineering units.
    //    Units: object dependent
    //    Range: object dependent
    float         MaxUnits;

    //$COMMON.ATTRIBUTE$
    //    Name:       ConsecutiveFailures
    //    Description:counter used for determining sensor failure.
    //          If ConsecutiveFailures count exceeds threshold,
    //          then sensor is considered failed.
    //    Units: None
    //    Range: N/A
    SHORT         ConsecutiveFailures;

    //$COMMON.ATTRIBUTE$
    //    Name:        Overrange
    //    Description: Boolean used to flag value out of range (lo or hi)
    //    Units: None
    //    Range: N/A
    E_Bool        Overrange;

    //$COMMON.ATTRIBUTE$
    //    Name:        currentVolt
    //    Description: current voltage
    //    Units: None
    //    Range: N/A
    float currentVolt;

    //$COMMON.ATTRIBUTE$
    //    Name:        id
    //    Description: Device ID
    //    Units: None
    //    Range: N/A
    E_DeviceId  Deviceid;
};

#endif  //SENSOR_H

