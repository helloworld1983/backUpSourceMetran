/******************************************************************************/
//$COMMON.H$
//    Filename: VCVController.h
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Class:  VCVController
//    This class defines the tasks which perform the Pressure control
//    calculations and sends the desired flow valve step positions
//    the the FlowController objects for the air and oxygen flow paths
//    which in turn calculate step positions for the air and oxygen valve
//    controllers.
/******************************************************************************/
#ifndef INC_VCVCONTROLLER_H_
#define INC_VCVCONTROLLER_H_

#include "CMVController.h"
#include "PhaseMgr.h"

class VCVController : public CMVController
{
public:
    VCVController(void);
    // static instance of CMVController class
//    static VCVController *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //Set desire flow
    void SetDesiredFlow(LONG desiredAir,LONG desiredO2);

    void Calculate(void);
    void SetSepecificPhase(E_SpecificPhaseId specificPhaseId);
    virtual void Disable(void);
    LONG Get_Inh_Vol(void);

protected:

    virtual ~VCVController(void){}
private:

    void InhProcess(LONG desiredAir,LONG desiredO2);
    void ExhProcess(void);

    void CalPressArgs(void);
    void CalExhArgs(void);

    void CalculateExhValveArgs(SHORT uiex,SHORT upex);
    void CalculateAirValveArgs(SHORT ui,SHORT up);
    void CalculateO2ValveArgs(SHORT ui,SHORT up);
    void DoPressureControll(void);

    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of VCVController class
    //
    //    Units: None
    //
    //    Range: N/A
    //
//    static VCVController *S_Instance;

    //$COMMON.ATTRIBUTE$
    //    Name: desiredAirFlow
    //
    //    Description: Air flow desired
    //
    //    Units: None
    //
    //    Range: N/A
    //
    LONG desiredAirFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: desiredO2Flow
    //
    //    Description: O2 flow desired
    //
    //    Units: None
    //
    //    Range: N/A
    //
    LONG desiredO2Flow;

    //$COMMON.ATTRIBUTE$
    //    Name: IsFirstBreath
    //
    //    Description: Is the first breath or not
    //
    //    Units: None
    //
    //    Range: True, false
    //
    bool IsFirstBreath;

    //$COMMON.ATTRIBUTE$
    //    Name: currentPhaseId
    //
    //    Description: Current phase ID
    //
    //    Units: None
    //
    //    Range: None
    //
    E_SpecificPhaseId currentPhaseId;

    //$COMMON.ATTRIBUTE$
    //    Name: IsPcv_cnt
    //
    //    Description: Is Pcv controller or not
    //
    //    Units: None
    //
    //    Range: None
    //
    bool IsPcv_cnt;

    //$COMMON.ATTRIBUTE$
    //    Name: premaxPIP
    //
    //    Description: previously max PIP
    //
    //    Units: None
    //
    //    Range: None
    //
    LONG premaxPIP;

    //$COMMON.ATTRIBUTE$
    //    Name: maxPIP
    //
    //    Description: max PIP
    //
    //    Units: None
    //
    //    Range: None
    //
    LONG maxPIP;

    //$COMMON.ATTRIBUTE$
    //    Name: premaxPlateau
    //
    //    Description: previously max value of Plateu
    //
    //    Units: None
    //
    //    Range: None
    //
    LONG premaxPlateau;

    //$COMMON.ATTRIBUTE$
    //    Name: maxPlateau
    //
    //    Description: max value of Plateu
    //
    //    Units: None
    //
    //    Range: None
    //
    LONG maxPlateau;

    //$COMMON.ATTRIBUTE$
    //    Name: Total_Flow
    //
    //    Description: total of flow
    //
    //    Units: None
    //
    //    Range: N/A
    //
    SHORT Total_Flow;

    //$COMMON.ATTRIBUTE$
    //    Name: delayBeforeExh
    //
    //    Description: Delay time before process exhalation
    //
    //    Units: None
    //
    //    Range: N/A
    //
    SHORT delayBeforeExh;

    //$COMMON.ATTRIBUTE$
    //    Name: Inh_TotalFlow
    //
    //    Description: total of Inhalation volume
    //
    //    Units: None
    //
    //    Range: N/A
    //
    double Inh_TotalFlow;

    //$COMMON.ATTRIBUTE$
    //    Name: Inh_OutputVol
    //
    //    Description: Inhalation volume
    //
    //    Units: None
    //
    //    Range: N/A
    //
    LONG  Inh_OutputVol;

};

#endif /* INC_VCVCONTROLLER_H_ */
