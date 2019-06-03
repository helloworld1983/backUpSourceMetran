//******************************************************************************
//$COMMON.H$
//   File Name:  MgrFlowSens.H
//   Copyright 2012 MeTranVietNam Co. All Rights Reserved.
//
//    Class:    FlowSensor
//    According to 840101/840102 Mass Flow Transducers 1980350SP_F.doc: DOCUMENT
//    This class provides an interface to the TSI Mass Flow Sensor type: 840101/840102
//
//   Revision History:
//
//******************************************************************************
#include "MgrFlowSens.h"
#include "DebugTrace.h"
#include "assignments.h"
#include "hardware.h"

MgrFlowSens *MgrFlowSens::S_Instance = NULL;
FlowSensor *MgrFlowSens::AirFlowSensor = NULL;
FlowSensor *MgrFlowSens::O2FlowSensor = NULL;

#define AIR_FLOWSENSOR           (0)
#define O2_FLOWSENSOR            (1)


/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the MgrFlowSens class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to MgrFlowSens class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrFlowSens* MgrFlowSens::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new MgrFlowSens();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for MgrFlowSens
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void MgrFlowSens::Init (void)
{
    O2FlowSensor = new FlowSensor(eO2FlowSensor,
                                  AIR_FLOWSENSOR,
                                  eOxygen);    // Upper Flow Reading, scaled

    ASSERTION( O2FlowSensor != NULL );

    AirFlowSensor = new FlowSensor(eAirFlowSensor,
                                   O2_FLOWSENSOR,
                                   eAir);    // Upper Flow Reading, scaled

    ASSERTION( AirFlowSensor != NULL );


    O2FlowSensor->Init();
    AirFlowSensor->Init();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrFlowSens
//
//    Processing: Constructor of  MgrFlowSens
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrFlowSens::MgrFlowSens (void)
{
}
