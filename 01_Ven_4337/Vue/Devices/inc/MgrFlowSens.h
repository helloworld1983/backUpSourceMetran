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
#ifndef INC_MGRFLOWSENS_H_
#define INC_MGRFLOWSENS_H_

#include "flowsens.h"

class MgrFlowSens
{
public:
    // static instance of PressureSensor class
    static MgrFlowSens *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //$COMMON.ATTRIBUTE$
    //    Name: AirFlowController
    //
    //    Description: Pointer to instance of air flow sensor
    //    Units: None
    //
    //    Range: N/A
    //
    static FlowSensor*  AirFlowSensor;

    //$COMMON.ATTRIBUTE$
    //    Name: O2FlowSensor
    //
    //    Description: Pointer to instance of 02 sensor
    //    Units: None
    //
    //    Range: N/A
    //
    static FlowSensor*  O2FlowSensor;

protected:

private:
    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of MgrFlowSens class
    //
    //    Units: None
    //
    //    Range: N/A
    //
    static MgrFlowSens *S_Instance;

    //MgrFlowSens constructor
    MgrFlowSens(void);
};

#endif /* INC_MGRFLOWSENS_H_ */
