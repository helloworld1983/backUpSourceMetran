//******************************************************************************
//$COMMON.H$
//   File Name:  AnalogInput.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//    Class:  A2DConverter
//
//      This class provides an interface to the Analog-to-Digital hardware.
//******************************************************************************
#include "AnalogInput.h"
#include "DebugTrace.h"

AnalogInput *AnalogInput::S_Instance = NULL;

#define ADC_10BIT_MASK                      1023
#define ADC_MAX_VOLTAGE                     3.3
#define ADC_BIT_RATE                        400000//500000
#define ADC_ID                              0//LPC_ADC0

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the WatchDogTimer class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to WatchDogTimer class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
AnalogInput* AnalogInput::S_GetInstance (void)
{
    //Create object if it is null
    if (S_Instance == NULL)
    {
        S_Instance = new AnalogInput();
    }
    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return(S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: getCurrentValue (UCHAR address)
//
//    Processing: Get current value
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: CurrentValue - current value
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
float AnalogInput::getCurrentValue (UCHAR channel)
{
    float voltVal = 0;
    uint16_t dataADC = 0;

//    //initialize for ADC
//    Init();
//
//    Chip_ADC_EnableChannel(ADC_ID, (ADC_CHANNEL_T)channel, ENABLE);
//    //Start A/D conversion if not using burst mode
//    Chip_ADC_SetStartMode(ADC_ID, ADC_START_NOW, ADC_TRIGGERMODE_RISING);
//    //Waiting for A/D conversion complete
//    while (Chip_ADC_ReadStatus(ADC_ID, channel, ADC_DR_DONE_STAT) != SET) {}
//
//    //Read ADC value
//    Chip_ADC_ReadValue(ADC_ID, channel, &dataADC);
//
//
//    //DEBUG_ADC("voltVal = %f\n", dataADC* ADC_MAX_VOLTAGE / ADC_10BIT_MASK);
//    // convert binary val to real voltage val
//    voltVal = dataADC * ADC_MAX_VOLTAGE / ADC_10BIT_MASK;
    return voltVal;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: Initialize for AnalogInput
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
void AnalogInput::Init (void)
{
//    ADC_CLOCK_SETUP_T ADCSetup;
//    uint32_t bitRate = ADC_BIT_RATE;
//    ADC_RESOLUTION_T bitAccuracy = ADC_10BITS;
//    Board_ADC_Init();
//    /*ADC Init */
//    Chip_ADC_Init(ADC_ID, &ADCSetup);
//    /* Disable Burst mode */
//    Chip_ADC_SetBurstCmd(ADC_ID, DISABLE);
//    /*Set sample rate */
//    Chip_ADC_SetSampleRate(ADC_ID, &ADCSetup, bitRate);
//    /* Set resolution */
//    Chip_ADC_SetResolution(ADC_ID, &ADCSetup, bitAccuracy);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: AnalogInput()
//
//    Processing: This is the private constructor for the A2DConverter class.
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    Ptr to static A2DConverter object
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
AnalogInput::AnalogInput(int range)
{

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: AnalogInput(void)
//
//    Processing: constructor for AnalogInput
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
AnalogInput::AnalogInput (void)
{
}
