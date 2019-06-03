//******************************************************************************
//$COMMON.CPP$
//    Filename: virtualpressure.cpp
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Purpose:  This file currently contains no code.
//      ALL of the METHODS of this class are INLINE and are in the .h file.
//      The only purpose served currently by this file is to allocate the
//      pointers to the global instances of the class.
//
//    Interfaces: None
//
//    Restrictions: None
//
//    Revision History
//******************************************************************************

// All this file does is include the .h file so the memory
// for the global data pointers will be allocated.
#include "VirtualPressure.h"
#include "sensor.h"

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetPrimary()
//
//    Processing: This routine assigns the Primary attribute to the
//      Sensor object passed as a parameter.
//    Input Parameters: Pointer to an actual pressure sensor.
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void VirtualPressureSensor::SetPrimary(Sensor* sensorPtr)
{
    Primary = sensorPtr;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCurrentReading()
//
//    Processing: This routine gets the current reading from the
//      actual pressure sensor that is designated as the Primary.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     Pressure reading in cmH20 * SCALE
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
SHORT VirtualPressureSensor::GetCurrentReading(void)
{
    return( Primary->GetCurrentReading() );
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Init (void)
//
//    Processing: Initialize for class
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void VirtualPressureSensor::Init (void)
{
//    ActivePressureSensor = new VirtualPressureSensor );
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetLastReading()
//
//    Processing: This routine gets the previously obtained reading from the
//      actual pressure sensor that is designated as the Primary.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     Pressure reading in cmH20 * SCALE
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
SHORT VirtualPressureSensor::GetLastReading(void)
{
    return( Primary->GetLastReading() );
}
