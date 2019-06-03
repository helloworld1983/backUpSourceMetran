/******************************************************************************/
//$COMMON.H$
//    Filename: CMVController.h
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Class:  CMVController
//    This class defines the tasks which perform the Pressure control
//    calculations and sends the desired flow valve step positions
//    the the FlowController objects for the air and oxygen flow paths
//    which in turn calculate step positions for the air and oxygen valve
//    controllers.
/******************************************************************************/
#ifndef INC_CMVCONTROLLER_H_
#define INC_CMVCONTROLLER_H_

#include "Controll.h"

class CMVController : public Controller
{
public:
    //Constructor
    CMVController(void);

//    // static instance of CMVController class
//    static CMVController *S_GetInstance(void);

    // initialize all public access to domain objects
    void Init (void);

    //Set inhalation target
    void SetInhalationTarget(USHORT target,USHORT riseTime);
    void SetExhalationTarget(USHORT target);

    //set bias flow
    void SetBiasFlow(USHORT flowSetting);

    //set target PEEP
    void SetTargetPEEP(USHORT targetPEEP);

    //Set IMVA Alarm Inactive Timer
    void SetIMVAlarmInactiveTimer(USHORT value);

    //Set VCV Peak Flow
    void SetVCVPeakFlow(float desiredFlow);

    // Top Level Calculation routine
    void Calculate(void);

    // Task Entry Point
    void Run(void);

    //Reset IMV static variable
    void ResetIMVStaticVariables(void);

    //Get previous filter pressure
    SHORT  GetPreFilterPressure(void);

    short a11,a12,a13,a14,a21,a22,a23,a24,Pressure_Gain,Flow_Gain;

    //GEt compensate pressure
    double GetCompensatePressure(void);

    //PDF control for air and O2
    LONG AirPDFControl( LONG flowCmd, LONG flowSensorVal);
    LONG O2PDFControl( LONG flowCmd, LONG flowSensorVal);

    //process filter
    void   Pre_Filter(void);
    virtual float  FirstPre_Filter(float u);
    float  SecondPre_Filter(float u);
    float  ThirdPre_Filter(float u);
    SHORT  PLFilter_Root(SHORT u);

    //compute compensate pressure for new tuve compensate
    double CompCompensatePressure(double compensateRate, bool type);

    SHORT  Pressure_RootGain(SHORT u);
    SHORT  Integrator_Ex(SHORT ui, SHORT up);
    SHORT  ExValve_Filter(SHORT u);
    SHORT  Gain_NonCom(SHORT u);
    SHORT  Integrator_Air(SHORT ui,SHORT up);
    SHORT  Integrator_O2(SHORT ui,SHORT up);
    SHORT  AirValve_Filter(SHORT u);
    SHORT  O2Valve_Filter(SHORT u);
    SHORT  Air_Filter(SHORT u);
    SHORT  O2_Filter(SHORT u);
    SHORT  O2Set_Filter(SHORT u);
    SHORT  Integrator_O2Percent(SHORT u);

    //Do O2 servo and pressure control
    void   DoO2Servo(void);
    void   DoPressureControll(void);

    //flow filter
    SHORT  FlowCons_Filter(SHORT u);
    SHORT  ExtraFlow_Comp(SHORT Exp);

    //$COMMON.ATTRIBUTE$
    //  Name: O2_Percent
    //  Description: p
    //  Units: LPM
    //  Range:
    SHORT O2_Percent;

    //$COMMON.ATTRIBUTE$
    //  Name: Pset
    //  Description: Get previous filter pressure
    //  Units: LPM
    //  Range:
    SHORT Pset;


    SHORT Alf;
    SHORT Bat;

    //$COMMON.ATTRIBUTE$
    //  Name: Int_PreFtr
    //  Description: filter value return
    //  Units: LPM
    //  Range:
    float Int_PreFtr1;
    float Int_PreFtr2;
    float Int_PreFtr3;

    //inhalation value
    LONG Int_PrxRootFtr;
    LONG Int_ExValve;
    LONG Int_AirValve;
    LONG Int_O2Valve;
    LONG Int_AirFFtr;
    LONG Int_O2FFtr;
    LONG Int_O2SetFtr;
    LONG Int_O2Percent;
    LONG Int_FConsFtr;
    LONG Int_ExtraFlow;

    LONG Int_ExValveFtr;
    LONG Int_AirValveFtr;
    LONG Int_O2ValveFtr;


    LONG AirPDFFirstIntegrator;
    LONG O2PDFFirstIntegrator;

    //step value
    short int AirStep;
    short int O2Step;
    short int ExhStep;

    unsigned char Flow_Cons;
    SHORT ExValve_Pos;
    SHORT Delay_Number;

    //$COMMON.ATTRIBUTE$
    //  Name: MeasuredAirFlow;
    //  Description: The current air flow reading.
    //      Used for flow limiting and oxygen mixing
    //  Units: ml/sec * 100
    //  Range: 0 - 333333
    LONG    MeasuredAirFlow;

    //$COMMON.ATTRIBUTE$
    //  Name: MeasuredO2Flow;
    //  Description: The current oxygen flow reading.
    //      Used for flow limiting and oxygen mixing
    //  Units: ml/sec * 100
    //  Range: 0 - 333333
    LONG    MeasuredO2Flow;


    //$COMMON.ATTRIBUTE$
    //  Name: TargetPeepPressure;
    //  Description: Target pressure which set by the user.
    //  Units: cm H2O
    //  Range: 0 - 200
    USHORT    TargetPeepPressure;

    //$COMMON.ATTRIBUTE$
    //  Name: Target;
    //  Description: Target pressure which set by the user.
    //  Units: cm H2O
    //  Range: 0 - 200
    USHORT    TargetInspPressure;

    //$COMMON.ATTRIBUTE$
    //  Name: RiseTime;
    //  Description: Rise time for inhalation phase.
    //  Units: second
    //  Range: .1 to .3
    USHORT    Rise_Time;


    //$COMMON.ATTRIBUTE$
    //  Name: BiasFlow;
    //  Description: Bias Flow during exhalation
    //  Units: LPM
    //  Range: 0 - 200
    USHORT    BiasFlow;

    //$COMMON.ATTRIBUTE$
    //  Name: MeasuredPressure;
    //  Description: The current Pressure Reading.
    //  Units: cm H2O
    //  Range: 0 - 200
    LONG    pressMeasured;

    //$COMMON.ATTRIBUTE$
    //  Name: FiO2Setting;
    //  Description: The O2 percentage setting
    //  Units: None.
    //  Range: 21 - 100
    USHORT O2_PerSet;

    //$COMMON.ATTRIBUTE$
    //  Name: exhlphase;
    //  Description: This flag will distinguish from exhalation & inhalation.
    //  Units: None.
    //  Range: True or False;
    E_Bool exhlphase;

    //$COMMON.ATTRIBUTE$
    //  Name: desiredPeakFlow
    //  Description:  desire peak flow.
    //  Units: None
    //  Range: n/a
    float desiredPeakFlow;

protected:
    //Destructor
    virtual ~CMVController(void){}

private:


    //$COMMON.ATTRIBUTE$
    //    Name: S_Instance
    //
    //    Description: Pointer to instance of ProxyFlowSensor class
    //
    //    Units: None
    //
    //    Range: N/A
    //
//    static CMVController *S_Instance;

    //$COMMON.ATTRIBUTE$
    //  Name: BiasFlow;
    //  Description: Bias Flow during exhalation
    //  Units: LPM
    //  Range: 0 - 200
    USHORT    FlowSetting;

    //$COMMON.ATTRIBUTE$
    //  Name: InactiveTimer
    //  Description:  Keep track the time of being inactive.
    //  Units: None
    //  Range: n/a
    USHORT InactiveTimer;


    //$COMMON.ATTRIBUTE$
    //  Name: short mCompensatePress;
    //  Description:  compensate pressure
    //  Units: None
    //  Range: n/a
    short mCompensatePress;;
};

#endif /* INC_CMVCONTROLLER_H_ */
