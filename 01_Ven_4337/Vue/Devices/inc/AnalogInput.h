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

class AnalogInput
{
public:
    // static instance of AlarmControl
    static AnalogInput* S_GetInstance (void);

    // Reads an A2D Channel
    USHORT GetRawCounts(UCHAR Address);

    //read current value in float
    float getCurrentValue(UCHAR address);

    static void Init(void);

private:
    //Constructor
    AnalogInput(int range);
    AnalogInput(void);

    //$COMMON.ATTRIBUTE$
    //  Name: Range
    //  Description: value of adc channel
    //  Units:
    //  Range:
    int Range;

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
