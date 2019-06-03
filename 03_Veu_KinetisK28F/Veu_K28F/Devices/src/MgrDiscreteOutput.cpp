//******************************************************************************//
//$COMMON.CPP$
//   File Name:  MgrDiscreteOutput.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file contains the methods of the DigitalOutput class.
//
//   Interfaces:
//      Derived Driver classes - interfaces to I/O hardware
//      Location data structure - HW mapping of output signal
//
//   Restrictions: None
//
//******************************************************************************

#include "MgrDiscreteOutput.h"
#include "DebugTrace.h"

MgrDiscreteOutput* MgrDiscreteOutput::S_Instance = NULL;
DiscreteOutput* MgrDiscreteOutput::InpedanceLine = NULL;
DiscreteOutput* MgrDiscreteOutput::SafetyValve = NULL;
DiscreteOutput* MgrDiscreteOutput::ExhPressureZero = NULL;
DiscreteOutput* MgrDiscreteOutput::InhPressureZero = NULL;
DiscreteOutput* MgrDiscreteOutput::PurgeFlow = NULL;
DiscreteOutput* MgrDiscreteOutput::OilPump = NULL;
DiscreteOutput* MgrDiscreteOutput::Enable24V = NULL;

#define SOLENOID_1_PORT             (0x1)             //PORT to control send data
#define SOLENOID_1_PIN              (0)               //PIN to control send data
#define SOLENOID_1_GPIO_PORT        (0)               //GPIO PORT to control send data
#define SOLENOID_1_GPIO_PIN         (4)              //GPIO PIN to control send data

#define SOLENOID_2_PORT             (0x5)             //PORT to control send data
#define SOLENOID_2_PIN              (1)               //PIN to control send data
#define SOLENOID_2_GPIO_PORT        (2)               //GPIO PORT to control send data
#define SOLENOID_2_GPIO_PIN         (10)              //GPIO PIN to control send data

#define SOLENOID_3_PORT             (0x4)             //PORT to control send data
#define SOLENOID_3_PIN              (9)               //PIN to control send data
#define SOLENOID_3_GPIO_PORT        (5)               //GPIO PORT to control send data
#define SOLENOID_3_GPIO_PIN         (13)              //GPIO PIN to control send data

#define SOLENOID_4_PORT             (0x0)             //PORT to control send data
#define SOLENOID_4_PIN              (1)               //PIN to control send data
#define SOLENOID_4_GPIO_PORT        (0)               //GPIO PORT to control send data
#define SOLENOID_4_GPIO_PIN         (1)              //GPIO PIN to control send data

#define SOLENOID_5_PORT             (0x4)             //PORT to control send data
#define SOLENOID_5_PIN              (10)               //PIN to control send data
#define SOLENOID_5_GPIO_PORT        (5)               //GPIO PORT to control send data
#define SOLENOID_5_GPIO_PIN         (14)              //GPIO PIN to control send data

#define SOLENOID_6_PORT             (0x5)             //PORT to control send data
#define SOLENOID_6_PIN              (0)               //PIN to control send data
#define SOLENOID_6_GPIO_PORT        (2)               //GPIO PORT to control send data
#define SOLENOID_6_GPIO_PIN         (9)              //GPIO PIN to control send data

#define SOLENOID_7_PORT             (0x0)             //PORT to control send data
#define SOLENOID_7_PIN              (0)               //PIN to control send data
#define SOLENOID_7_GPIO_PORT        (0)               //GPIO PORT to control send data
#define SOLENOID_7_GPIO_PIN         (0)              //GPIO PIN to control send data

#define RED_LED_LPC_PORT             (0x6)             //PORT to control send data
#define RED_LED_LPC_PIN              (11)               //PIN to control send data
#define RED_LED_LPC_GPIO_PORT        (3)               //GPIO PORT to control send data
#define RED_LED_LPC_GPIO_PIN         (7)              //GPIO PIN to control send data

#define YELLOW_LED_LPC_PORT             (0x6)             //PORT to control send data
#define YELLOW_LED_LPC_PIN              (12)               //PIN to control send data
#define YELLOW_LED_LPC_GPIO_PORT        (2)               //GPIO PORT to control send data
#define YELLOW_LED_LPC_GPIO_PIN         (8)              //GPIO PIN to control send data

#define BUZZER_PWM    LPC_SCT
#define BUZZER_HZ                       (2400)          //set frequency for buzzer

#define BUZZER_LPC_PORT                 (0x2)             //PORT to control send data
#define BUZZER_LED_LPC_PIN              (10)               //PIN to control send data
#define BUZZER_LED_LPC_GPIO_PORT        (0)               //GPIO PORT to control send data
#define BUZZER_LED_LPC_GPIO_PIN         (14)              //GPIO PIN to control send data

#define BUZZER_PWM_PIN_DIMMER           (2)             //SCT output 2.
#define BUZZER_PWM_DIMMER               (2)             //index of your register

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the MgrDiscreteOutput class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to MgrDiscreteOutput class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
MgrDiscreteOutput* MgrDiscreteOutput::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new MgrDiscreteOutput();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: init for MgrDiscreteOutput
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
void MgrDiscreteOutput::Init (void)
{
//    // Exhalation Pressure Auto-Zero Solenoid
//     ExhPressureZero = new DiscreteOutput(eExhPressZero, SOLENOID_1_PORT, SOLENOID_1_PIN, SOLENOID_1_GPIO_PORT, SOLENOID_1_GPIO_PIN, SCU_MODE_FUNC0);
//     ASSERTION( ExhPressureZero != NULL );
//
//     // Inhalation Pressure Auto-Zero Solenoid
//     InhPressureZero = new DiscreteOutput(eInhPressZero, SOLENOID_2_PORT, SOLENOID_2_PIN, SOLENOID_2_GPIO_PORT, SOLENOID_2_GPIO_PIN, SCU_MODE_FUNC0);
//     ASSERTION( InhPressureZero != NULL );
//
//     SafetyValve = new DiscreteOutput(eSafetyValve, SOLENOID_3_PORT, SOLENOID_3_PIN, SOLENOID_3_GPIO_PORT, SOLENOID_3_GPIO_PIN, SCU_MODE_FUNC4);
//     ASSERTION( SafetyValve != NULL );
//
//     InpedanceLine = new DiscreteOutput(eInpedanceLine, SOLENOID_4_PORT, SOLENOID_4_PIN, SOLENOID_4_GPIO_PORT, SOLENOID_4_GPIO_PIN, SCU_MODE_FUNC0);
//     ASSERTION( InpedanceLine != NULL );
//
//     // Create DiscreteOutput objects used by Morph
//     PurgeFlow = new DiscreteOutput(ePurgeFlow, SOLENOID_5_PORT, SOLENOID_5_PIN, SOLENOID_5_GPIO_PORT, SOLENOID_5_GPIO_PIN, SCU_MODE_FUNC4);
//     ASSERTION( PurgeFlow != NULL );
//
//     OilPump = new DiscreteOutput(eOilPump,SOLENOID_6_PORT, SOLENOID_6_PIN, SOLENOID_6_GPIO_PORT, SOLENOID_6_GPIO_PIN, SCU_MODE_FUNC0, eOff,eOff);
//     ASSERTION( OilPump != NULL );
//
//     Enable24V = new DiscreteOutput(eEnable24V, SOLENOID_7_PORT, SOLENOID_7_PIN, SOLENOID_7_GPIO_PORT, SOLENOID_7_GPIO_PIN, SCU_MODE_FUNC0);
//     ASSERTION( Enable24V != NULL );

     //initialize IO
     ExhPressureZero->Init();
     InhPressureZero->Init();
     SafetyValve->Init();
     InpedanceLine->Init();
     PurgeFlow->Init();
     OilPump->Init();
     Enable24V->Init();
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MgrDiscreteOutput
//
//    Processing: Constructor of  MgrDiscreteOutput
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
MgrDiscreteOutput::MgrDiscreteOutput (void)
{
}
