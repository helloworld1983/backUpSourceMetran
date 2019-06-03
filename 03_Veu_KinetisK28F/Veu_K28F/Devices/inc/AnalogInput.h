//******************************************************************************
//$COMMON.H$
//   File Name:  AnalogInput.H
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//    Class:  A2DConverter
//
//      This class provides an interface to the Analog-to-Digital hardware.
//******************************************************************************
#ifndef INC_ANALOGINPUT_H_
#define INC_ANALOGINPUT_H_

#include "CommonTypes.h"

#define PRESSURE1                           ADC_CH2
#define PRESSURE2                           ADC_CH5
#define TEMP                                ADC_CH6
#define FIO2                                ADC_CH3

class AnalogInput
{
public:
    // static instance of AlarmControl
    static AnalogInput* S_GetInstance (void);

    //read current value in float
    float getCurrentValue(UCHAR channel);

    //initialize adc
    void Init(void);

private:
    //Constructor
    AnalogInput(int range);
    AnalogInput(void);

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of AnalogInput class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static AnalogInput *S_Instance;
};

#endif /* INC_ANALOGINPUT_H_ */
