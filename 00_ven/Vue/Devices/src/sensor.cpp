//******************************************************************************//
//$COMMON.CPP$
//   File Name:  SENSOR.CPP
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:   This file contains the routines for the Sensor class.
//      The Sensor class represents generic analog inputs and also
//      serves as a base class for the FlowSensor and PressureSensor
//      derived classes.
//
//
//   Interfaces:
//      Interfaces with the A2DController.
//
//   Restrictions:
//      A Maximum of 16 analog output signals are supported by the hardware.
//
//******************************************************************************
#include "sensor.h"
#include "AnalogInput.h"
#include "Errorhan.h"
#include "logentry.h"
#include "AlarmTask.h"

#include "CommonConstants.h"
#include "DebugTrace.h"

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Sensor()
//
//    Processing: This is the default constructor.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    ptr to Sensor object created.
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
Sensor::Sensor(void)
{
    ConversionFactor = 0;

    ConsecutiveFailures = 0;
    Address   = 0;
    MinVolt = 0;
    MaxVolt = 0;
    MinUnits  = 0;
    MaxUnits  = 0;
    currentVolt = 0;

    // Initialize Attributes
    Overrange = eFalse;
    ConsecutiveFailures = 0;
    CurrentReading = 0;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Sensor()
//
//    Processing: This is the constructor for the Sensor class
//
//    Input Parameters:
//          address    - A2D channel connected to sensor input
//          counts_lo  - minimum valid A/D count
//          counts_hi  - maximum valid A/D count
//          units_lo   - minimum valid reading in engineering units
//          units_hi   - maximum valid reading in engineering units
//          conversion - conversion factor to use
//    Output Parameters: None
//    Return Values:     ptr to the created sensor object.
//    Pre-Conditions:    Channel number must be between 0 and 15; voltage must
//          be between 0 and 5.
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
Sensor::Sensor(E_DeviceId  id,      // Sensor Id #
               UCHAR  address,      // A2D Channel #
               float  counts_lo,    // Lower A/D Count Limit
               float  counts_hi,    // Upper A/D Count Limit
               float  units_lo,     // Lower Sensor Reading
               float  units_hi     // Upper Sensor Reading
)   // Conversion factor
{
    currentVolt = 0;

    // PRECONDITIONS
    ASSERTION( address <= 15 );
    //analog input range is from 0 to 5V
    ASSERTION((counts_hi <= (5)) && (counts_lo >= 0));

    // Initialize Attributes
    Address   = address;
    MinVolt = counts_lo;
    MaxVolt = counts_hi;
    MinUnits  = units_lo;
    MaxUnits  = units_hi;

    ConversionFactor = (MaxUnits - MinUnits)/(MaxVolt - MinVolt);

    // Initialize Attributes
    Overrange = eFalse;
    ConsecutiveFailures = 0;
    CurrentReading = 0;

    Deviceid = id;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetLastVolt()
//
//    Processing: get latest analog voltage.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    latest analog voltage
//    Pre-Conditions:   None
//    Miscellaneous:
//    Requirements:
//
//******************************************************************************
float Sensor::GetLastVolt(void)
{
    return currentVolt;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCurrentVolt()
//
//    Processing: update analog voltage from hardware.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    analog voltage has been read
//    Pre-Conditions:   None
//    Miscellaneous:
//    Requirements:
//
//******************************************************************************
float Sensor::GetCurrentVolt(void)
{
    currentVolt = AnalogInput::S_GetInstance()->getCurrentValue(Address);
    return currentVolt;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCurrentReading()
//
//    Processing: This routine obtains the count reading from the A2D converter.
//          The counts are then bounded by the acceptable limits and the count
//          value is converted into a reading in engineering units.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    Sensor reading in engineering units
//    Pre-Conditions:   None
//    Miscellaneous:
//          Needs more complete error handling.
//          Does not check A2DOutOfRange discrete input signal
//    Requirements:
//
//******************************************************************************
SHORT Sensor::GetCurrentReading(void)
{
    const USHORT MAX_A2D_FAILURES = 3;
    const float ZeroOffsetInVolt = 2.5;
    float currentUnit;
    //get analog voltage
    currentVolt = AnalogInput::S_GetInstance()->getCurrentValue(Address);
    currentUnit = ConversionFactor*(currentVolt - ZeroOffsetInVolt);

    if((currentUnit >= MinUnits)&&(currentUnit <= MaxUnits))
    {
        ConsecutiveFailures = 0;
        CurrentReading = (SHORT)(100*currentUnit);
    }
    else
    {
        ConsecutiveFailures++;
        if (ConsecutiveFailures > MAX_A2D_FAILURES)
        {

        }
    }
    //If error getting counts, use previous value and return status
    return(CurrentReading);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: S_Init()
//
//    Processing: Constructs global Sensor objects.
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void Sensor::S_Init(void)
{
//    InhPressSensor = new Sensor(eInhPressSensor,
//                                A2D_CHANNEL_00,    // A2D channel #
//                                0.5, // Low Voltage Limit * 1000
//                                4.5, // High Voltage Limit * 1000
//                                -200,    // Lower Pressure Reading, scaled
//                                200);    // Upper Pressure Reading, scaled
//
//    ASSERTION( InhPressSensor != NULL );
//
//    ExhPressSensor = new Sensor(eExhPressSensor,
//                                A2D_CHANNEL_01,    // A2D channel #
//                                0.5, // Low Voltage Limit * 1000
//                                4.5, // High Voltage Limit * 1000
//                                -200,    // Lower Pressure Reading, scaled
//                                200);    // Upper Pressure Reading, scaled
//
//    ASSERTION( ExhPressSensor != NULL );

}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: HandleBadSensor()
//
//    Processing: Performs error handling for a sensor that has been determined
//          to be bad.
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
void Sensor::HandleBadSensor(E_DeviceId  Deviceid)
{
    switch (Deviceid)
    {
        // Flow Sensors
        case eAirFlowSensor:
        case eO2FlowSensor:
        case eExhFlowSensor:
            // Go Vent Inop
//            ErrorHandler::S_GetInstance()->
//            LogEvent(eFlowSensorFailure, eSTR_LOG_FlowSensorError);
            break;

            // Pressure Sensors
        case eInhPressSensor:
            // Go Vent Inop
//            ErrorHandler::S_GetInstance()->
//            LogEvent(eInhPressureSensorFailure, eSTR_LOG_InhPressSensorError);
            break;
        case eExhPressSensor:
            // Go Vent Inop
//            ErrorHandler::S_GetInstance()->
//            LogEvent(eExhPressureSensorFailure, eSTR_LOG_ExhPressSensorError);
            break;

            // case eFiO2Sensor is handled by the Fio2 class

        case eBatteryVoltage:
            AlarmTask::S_GetInstance()->SetIsActive(eBadBatteryVoltageSensor, eActive);
            break;

        case eADCWrapAI:
            AlarmTask::S_GetInstance()->SetIsActive(eBadADCWrapAISensor, eActive);
            break;

        case eEnclosureTemp:
            AlarmTask::S_GetInstance()->SetIsActive(eBadInternalTempSensor, eActive);

            break;

        case eInternalO2:
            AlarmTask::S_GetInstance()->SetIsActive(eBadInternalO2Sensor, eActive);
            break;

            // All other sensors
        default:
            break;
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetLastReading()
//
//    Processing: returns most recently obtained sensor readin in
//          engineering units.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    Sensor reading in engineering units
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
SHORT Sensor::GetLastReading(void)
{
    // Copy current reading to address specified
    return (CurrentReading);
}
