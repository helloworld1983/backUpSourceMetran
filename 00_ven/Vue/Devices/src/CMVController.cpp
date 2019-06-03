/******************************************************************************/
//$COMMON.CPP$
//    Filename: PRES_CNT.cpp
//    Copyright 1996 Metran Medical Inc. All Rights Reserved.
//
//    Purpose:  This file contains the routines for the CMVController class
//
//    Interfaces:
//      Air and O2 StepMotor objects
//
//    Restrictions:   None
//
//    Revision History
/******************************************************************************/
#include "CMVController.h"
#include "O2Setting.h"
#include "MgrBdSetting.h"
#include "PhaseMgr.h"
#include "MgrBreathData.h"
#include "ExhPhase.h"
#include "NormBreathingMode.h"
#include "Btps.h"
#include "RootTable.h"
#include "ProxyFlowSensor.h"


#define Kp 0.1
#define Ki 0.00002
#define Kd 0.00006
static const SHORT PDF_ERROR_GAIN = 50; //40;
static const SHORT PDF_SCALE_FACTOR = 5; //50
static const SHORT MAP_SERVO_SCALE = 8;
static const SHORT MAP_SERVO_RATE = 500;
static const SHORT CONSTANT_FLOW =10;  //10 Lpm
static const UNSIGNED FLOW_SCALER = 1667;
static const SHORT MAX_STEP = 2300;

const USHORT Nco            =    2000;
const USHORT ValveFtr_Const =    100;
const USHORT InhUp_FlowLmt    =    2200;
const USHORT ExhUp_FlowLmt =    1500;
const USHORT Down_FlowLmt    =   0;          //Old 150
USHORT Up_ExLmt        =    2200;
const USHORT Down_ExLmt        =    0;
const USHORT Down_O2PerLmt    =    1;
const USHORT Up_O2PerLmt    =    499;
const USHORT iDT            =    500;           //200 for 5ms interval, 500 for 2ms interval
const USHORT Scale_n        =    8;
const USHORT SwitchTime     = 256;

const double p = 1.112; // [kg/m3]
const double Pi = 3.14;

//CMVController *CMVController::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the ProxyFlowSensor class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to ProxyFlowSensor class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
//CMVController* CMVController::S_GetInstance (void)
//{
//    //Create object if it is null
//    if (S_Instance == NULL)
//    {
//        S_Instance = new CMVController();
//    }
//    //check object is null or not
//    ASSERTION(S_Instance != NULL);
//
//    return(S_Instance);
//}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Init()
//
//    Processing: initialize CMVController
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void CMVController::Init (void)
{
    //Initialize proxy sensor
    //add code
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetBiasFlow (USHORT flowSetting)
//
//    Processing: Set bias flow with flowsetting
//
//    Input Parameters: USHORT flowSetting - bias flow value
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void CMVController::SetBiasFlow (USHORT flowSetting)
{
    FlowSetting = flowSetting;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetBiasFlow (USHORT flowSetting)
//
//    Processing: set target PEEP
//
//    Input Parameters: USHORT targetPEEP - target PEEP
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void CMVController::SetTargetPEEP (USHORT targetPEEP)
{
    //set target PEEP
    TargetPeepPressure = targetPEEP;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetIMVAlarmInactiveTimer (USHORT value)
//
//    Processing: set alarm inactive or not
//
//    Input Parameters: USHORT value - set alarm inactive timer or not
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void CMVController::SetIMVAlarmInactiveTimer (USHORT value)
{
    //Set IMVA Alarm Inactive Timer
    InactiveTimer = value;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetVCVPeakFlow (float desiredFlow)
//
//    Processing: Set VCV Peak Flow
//
//    Input Parameters: float desiredFlow - VCV Peak Flow
//
//    Output Parameters: None
//
//    Return Values:     None
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
void CMVController::SetVCVPeakFlow (float desiredFlow)
{
    desiredPeakFlow = desiredFlow;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetPreFilterPressure (void)
//
//    Processing: Get previous filter pressure
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     Pset - previous filter pressure
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
SHORT CMVController::GetPreFilterPressure (void)
{
    //Get previous filter pressure
    return Pset;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: GetCompensatePressure (void)
//
//    Processing: GEt compensate pressure
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values:     mCompensatePress - compensate pressure
//
//    Pre-Conditions:    None
//
//    Miscellaneous:     None
//
//    Requirements:
//
//******************************************************************************
double CMVController::GetCompensatePressure (void)
{
    return mCompensatePress;
}

//*****************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CMVController()
//
//    Processing: This is the constructor for the pressure controller.
//          This routine calls the Controller() constructor.
//
//    Input Parameters:
//          name - task name
//          pri  - task priority
//          ticks - task scheduling interval
//          sensor - Pressure Sensor used to close the loop
//    Output Parameters:  None
//    Return Values:      ptr to the CMVController object created.
//    Pre-Conditions: The sensor object is assumed
//              to already have been instantiated.
//
//    Miscellaneous:  None
//
//    Requirements:
//
//*****************************************************************************
CMVController::CMVController() :
    Controller()
{
    Pset = 0;
    MeasuredAirFlow = 0;
    TargetPeepPressure = 0;
    TargetInspPressure = 0;
    Rise_Time = 0;
    FlowSetting = 0;
    BiasFlow = 0;
    pressMeasured = 0;
    O2_PerSet = 0;
    exhlphase = eFalse;
    InactiveTimer = 0;
    desiredPeakFlow = 0;

    mCompensatePress = 0;
    O2_Percent = 21;
    MeasuredO2Flow=0;

    a11 = 126;//115;//154;
    a12 = 22;
    a13 = -906;//-1300;
    a14 = 20;//350;//424;

    a21 = -2;
    a22 = -54;
    a23 = 5674;//1277;//1455;
    a24 = 2166;//846;

    Pressure_Gain = 2000;        /* 1.4 * 1024 */
    Flow_Gain  = 1957; //2125;      /* Nco * 1.4, Nco=1500*/

    ResetIMVStaticVariables();
#ifdef TESTING
    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVa11Setting)->SetValue(a11*100);
    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVa12Setting)->SetValue(a12*100);
    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVa13Setting)->SetValue(a13*100);
    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVa14Setting)->SetValue(a14*100);

    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVa21Setting)->SetValue(a21*100);
    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVa22Setting)->SetValue(a22*100);
    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVa23Setting)->SetValue(a23*100);
    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVa24Setting)->SetValue(a24*100);

    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVPressureGainSetting)->SetValue(Pressure_Gain*100);
    MainHandlerDelegate::S_GetInstance()->GetNumSetting(ePCVFlowGainSetting)->SetValue(Flow_Gain*100);
#endif

}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetInhalationTarget()
//
//    Processing: The
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void CMVController::SetInhalationTarget(USHORT target, USHORT riseTime)
{
    TargetInspPressure = target;
    Rise_Time = riseTime;

    exhlphase = eFalse;

}   // end SetInhalationTarget()
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetExhalationTarget()
//
//    Processing: The
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void CMVController::SetExhalationTarget(USHORT target)
{

    TargetPeepPressure = target;
    exhlphase = eTrue;
}   // end SetExhalationTarget()

//*****************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Entry()
//
//    Processing: This is the main processing loop for the Inhalation Controllers.
//          The routine obtains the current sensor reading, calculates the desired
//          total flow rate, splits the flow rate into air and oxygen rates, and
//          then commands the Flow Controllers to their respective desired rates.
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:    None
//    Miscellaneous:     None
//    Requirements:
//
//******************************************************************************
void CMVController::Run()
{
    if (eTrue == Enabled)
    {
        Calculate();
    }
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:FirstPre_Filter(float u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
float  CMVController::FirstPre_Filter(float u)
{
    double Gain;
    float Error;

    Error = u - Int_PreFtr1;

    if( !exhlphase )
    {
        Gain = Error*330.0/(float)iDT;
        ASSERTION(Rise_Time != 0);
        Int_PreFtr1 += Gain/(float)Rise_Time;
    }
    else
        Int_PreFtr1 += Error*100.0/(float)iDT;
    return(Int_PreFtr1);
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:SecondPre_Filter(float u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
float  CMVController::SecondPre_Filter(float u)
{
    double Gain;
    float Error;

    Error = u - Int_PreFtr2;

    if( !exhlphase )
    {
        Gain = Error*330/iDT;
        ASSERTION(Rise_Time != 0);
        Int_PreFtr2 += Gain/Rise_Time;
    }
    else
        Int_PreFtr2 += Error*33/iDT;
    return(Int_PreFtr2);
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:ThirdPre_Filter(float u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
float  CMVController::ThirdPre_Filter(float u)
{
    float Error;

    Error = u - Int_PreFtr3;

    Int_PreFtr3 += Error*30/iDT;

    return(Int_PreFtr3);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:Pre_Filter(void)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
void CMVController::Pre_Filter(void)
{
    SHORT Temp16;

    // PIP was set inside of SERVO_InspInit
    double C = (double)MgrBdSetting::S_GetInstance()->GetCurrentValue(eCompRate)/(double)SCALE; //[%]
    if(!exhlphase)
    {
         Temp16 = (SHORT) FirstPre_Filter(TargetInspPressure+CompCompensatePressure(C, true)*10.0);//for new tube compensate
    }
    else // use the peep this will help the CPAP to have PEEP change right away.
    {
        Temp16 = (SHORT) FirstPre_Filter(TargetPeepPressure+CompCompensatePressure(C, true)*10.0);//for new tube compensate
    }

    Pset = Temp16;
}

/*------ The end of Pre_Filter -------------------*/

/*  Integration for exahalation valve             */
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:PLFilter_Root(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT  CMVController::PLFilter_Root(SHORT u)
{
    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_PrxRootFtr>>Scale_n);
    Error = u - Output;
    Temp32 = Error;
    Temp32 = Temp32<<Scale_n;

    Int_PrxRootFtr += Temp32*250/iDT;

    return(Output);
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:Pressure_RootGain(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT CMVController::Pressure_RootGain(SHORT u)
{
    uint16_t Root16;
    SHORT Temp16;
    LONG Temp32;

    Temp16 = PLFilter_Root(pressMeasured);
    if(Temp16<10)
        Temp16 = 10;
    else
        if(Temp16>1000)
            Temp16 = 1000;

    if(Temp16<255)
    {
        Root16 = m_table[Temp16];
        Temp32 = (LONG)Root16*1000;
        Temp32 = (LONG)Temp32/3162; /* Temp32 = Root16/sqrt(10) */
        Temp32 = Temp32>>2;
        Temp16 = (SHORT)Temp32;
    }
    else
    {
        Temp16 = Temp16/10;
        Root16 = m_table[Temp16];
        Temp16 = Root16>>2;
    }

    Temp16 += 2048;                     /* 2*1024 */
    Temp32 = (LONG)u*Pressure_Gain;
    ASSERTION(Temp16 != 0);
    Temp32 = (LONG)Temp32/Temp16;
    Temp16 = (SHORT)Temp32;
    return(Temp16);
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:Integrator_Ex(SHORT ui, SHORT up)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT CMVController::Integrator_Ex(SHORT ui, SHORT up)
{
    LONG Temp32;
    SHORT Temp16 ,Output;

    Temp32 = ui;
    Temp32 = Temp32<<Scale_n;
    Temp32 = -Temp32*a13/10;
    Temp32 = Temp32/iDT;
    Temp16 = (SHORT)(Int_ExValve>>Scale_n);
    Temp16 += up;
    Temp16 = Pressure_RootGain(Temp16);

    if(Temp16>=Up_ExLmt)
    {
        if(ui<0)
            Int_ExValve += Temp32;
        Output = Up_ExLmt;
    }
    else if(Temp16<=Down_ExLmt)
    {
        if(ui>0)
            Int_ExValve += Temp32;
        Output = Down_ExLmt;
    }
    else
    {
        Int_ExValve += Temp32;
        Output = Temp16;
    }

    return(Output);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:ExValve_Filter(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT  CMVController::ExValve_Filter(SHORT u)
{

    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_ExValveFtr>>Scale_n);
    Error = u - Output;
    Temp32 = Error;
    Temp32 = Temp32<<Scale_n;
    Int_ExValveFtr += (LONG)Temp32*ValveFtr_Const/iDT;
    return(Output);
}

/*  Integration for Air Flow valve                 */
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:Gain_NonCom(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT CMVController::Gain_NonCom(SHORT u)
{
    LONG x;
    SHORT  y;
    x = (LONG)Flow_Gain*u;
    ASSERTION((u + Nco) !=0 );
    // Avoid division by zero
    if ((u + Nco) != 0)
    {
        x = (LONG)x/(u + Nco);
    }
    y = (SHORT)x;
    return(y);
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Integrator_Air(SHORT ui,SHORT up)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT CMVController::Integrator_Air(SHORT ui,SHORT up)
{
    LONG Temp32,Temp32B;
    SHORT Temp16 , Up_FlowLmt, Output;

    Temp32 = ui;
    Temp32 = Temp32<<Scale_n;
    Temp32 = (LONG)Temp32*a24/10;
    Temp32 = (LONG)Temp32/iDT;

    if(Int_AirValve>=(LONG)0x003fffff){
        Int_AirValve = (LONG)0x003fffff;
    }
    else if(Int_AirValve<=(LONG)0xffc00000){
        Int_AirValve = (LONG)0xffc00000;
    }

    Temp16 = (SHORT)(Int_AirValve>>Scale_n);
    Temp16 += up;
    Temp32B = (LONG)Temp16*Alf/500;
    Temp16 = (SHORT)Temp32B;

    Temp16 = Gain_NonCom(Temp16);

    if ( !exhlphase )
    {
        Up_FlowLmt = InhUp_FlowLmt;
    }
    else
    {
        Up_FlowLmt = ExhUp_FlowLmt;
    }

    if(Temp16>=Up_FlowLmt)
    {
        if(ui<0)
            Int_AirValve += Temp32;
        Output = Up_FlowLmt;
    }
    else if(Temp16<=Down_FlowLmt)
    {
        if(ui>0)
            Int_AirValve += Temp32;
        Output = Down_FlowLmt;
    }
    else
    {
        Int_AirValve += Temp32;
        Output = Temp16;
    }
    return((SHORT)Output);
}
/*-- End of Integration of Air Valve --------*/

/*  Integration for O2 Flow   valve             */
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Integrator_O2(SHORT ui,SHORT up)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT CMVController::Integrator_O2(SHORT ui,SHORT up)
{
    LONG Temp32,Temp32B;
    SHORT Temp16 ,Up_FlowLmt, Output;

    Temp32 = ui;
    Temp32 = Temp32<<Scale_n;
    Temp32 = (LONG)Temp32*a24/10;
    Temp32 = (LONG)Temp32/iDT;

    if(Int_O2Valve>=(LONG)0x003fffff)
        Int_O2Valve = (LONG)0x003fffff;
    else
        if(Int_O2Valve<=(LONG)0xffc00000)
            Int_O2Valve = (LONG)0xffc00000;

    Temp16 = (SHORT)(Int_O2Valve>>Scale_n);
    Temp16 += up;
    Temp32B = (LONG)Temp16*Bat/500;
    Temp16 = (SHORT)Temp32B;

    Temp16 = Gain_NonCom(Temp16);

    if ( !exhlphase )
    {
        Up_FlowLmt = InhUp_FlowLmt;
    }
    else
    {
        Up_FlowLmt = ExhUp_FlowLmt;
    }

    if(Temp16>=Up_FlowLmt)
    {
        if(ui<0)
            Int_O2Valve += Temp32;
        Output = Up_FlowLmt;
    }
    else if(Temp16<=Down_FlowLmt)
    {
        if(ui>0)
            Int_O2Valve += Temp32;
        Output = Down_FlowLmt;
    }
    else
    {
        Int_O2Valve += Temp32;
        Output = Temp16;
    }

    return(Output);
}
/*-- End of Integration of O2 Valve --------*/

/*        Filter for Air Valve     */
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: AirValve_Filter(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT  CMVController::AirValve_Filter(SHORT u)
{
    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_AirValveFtr>>Scale_n);
    Error = u - Output;
    Temp32 = Error*2;
    Temp32 = Temp32<<Scale_n;
    Int_AirValveFtr += (LONG)Temp32*ValveFtr_Const/iDT;
    return(Output);
}


/* For O2 percent Control                            */
/*     1/(S + 1)  */
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Air_Filter(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT  CMVController::Air_Filter(SHORT u)
{
    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_AirFFtr>>10);
    Error = u - Output;
    Temp32 = Error;
    Temp32 = Temp32<<10;

    Int_AirFFtr += (LONG)Temp32/iDT;

    return(Output);
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: O2Valve_Filter(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT  CMVController::O2Valve_Filter(SHORT u)
{
    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_O2ValveFtr>>Scale_n);

    Error = u - Output;
    Temp32 = Error*2;
    Temp32 = Temp32<<Scale_n;
    Int_O2ValveFtr += (LONG)Temp32*ValveFtr_Const/iDT;
    return(Output);
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: O2_Filter(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT  CMVController::O2_Filter(SHORT u)
{
    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_O2FFtr>>10);
    Error = u - Output;
    Temp32 = Error;
    Temp32 = Temp32<<10;

    Int_O2FFtr += Temp32/iDT;

    return(Output);
}


//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: O2Set_Filter(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT  CMVController::O2Set_Filter(SHORT u)
{
    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_O2SetFtr>>10);
    Error = u - Output;
    Temp32 = Error;
    Temp32 = Temp32<<10;

    Int_O2SetFtr += Temp32/iDT;

    return(Output);
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Integrator_O2Percent(SHORT u)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT CMVController::Integrator_O2Percent(SHORT u)
{
    LONG Temp32;
    SHORT Temp16 ,Output;

    Temp32 = u;
    Temp32 = Temp32<<10;
    Temp32 = Temp32;
    Temp32 = Temp32/100;
    Temp16 = (SHORT)(Int_O2Percent>>10);

    if(Temp16>=Up_O2PerLmt)
    {
        if(u<0)
            Int_O2Percent += Temp32;
        Output = Up_O2PerLmt;
    }
    else if(Temp16<=Down_O2PerLmt)
    {
        if(u>0)
            Int_O2Percent += Temp32;
        Output = Down_O2PerLmt;
    }
    else
    {
        Int_O2Percent += Temp32;
        Output = Temp16;
    }
    return(Output);
}

/* This routine should be run every 100ms            */
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: DoO2Servo()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
void CMVController::DoO2Servo(void)
{

    //    static SHORT Counter = 0;
    SHORT Temp16,Average_AirF,Average_O2F,O2Set_Ftr;
    LONG Temp32A,Temp32B;

    Average_AirF = Air_Filter(MeasuredAirFlow);
    Average_O2F = O2_Filter(MeasuredO2Flow);

    O2_PerSet = ((79*O2Setting::GetDesiredO2Ratio()) >> 7)+ 21;

    O2Set_Ftr = O2Set_Filter(O2_PerSet);

    if(O2Set_Ftr < 21 )
    {
        O2Set_Ftr = 21;
    }

    {
        Temp16 = Average_AirF + Average_O2F;
        if(Temp16<0)
            Temp16 = - Temp16;


        Temp32A = (LONG)Average_AirF*21000;
        Temp32A += (LONG)Average_O2F*100000;
        Temp32B = (LONG)Temp16*1000;
        Temp32B += 1;
        ASSERTION(Temp32B != 0);
        Temp32A = (LONG)Temp32A/Temp32B;
        O2_Percent = (SHORT)Temp32A;


        if(O2_Percent>=100)
            O2_Percent = 100;
        else if(O2_Percent<21)
            O2_Percent = 21;

        if(O2_PerSet == 100)
        {
            Bat = 499;
            Alf = 1;
            Int_O2Percent = 510977;
            Int_O2SetFtr = 102400 ;
        }
        else
        {
            Bat = Integrator_O2Percent(O2Set_Ftr - O2_Percent);

            Alf = 500 - Bat;
        }
    }
}

/*        TE Constant Flow Switch Filter                         */
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: FlowCons_Filter()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
SHORT  CMVController::FlowCons_Filter(SHORT u)
{
    LONG Temp32;
    SHORT Error,Output;

    Output = (SHORT)(Int_FConsFtr>>Scale_n);
    Error = u - Output;
    Temp32 = Error;
    Temp32 = Temp32<<Scale_n;
    Int_FConsFtr += Temp32*200/iDT;

    return(Output);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ExtraFlow_Comp()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************                                  */
SHORT  CMVController::ExtraFlow_Comp(SHORT Exp)
{
    LONG Temp32;
    SHORT Output;

    Output = (SHORT)(Int_ExtraFlow>>Scale_n);
    if( !exhlphase )
    {
        if((Exp==(SHORT)Down_ExLmt)&&(Output<=20))
            Temp32 = 1;
        else
            Temp32 = 0;
    }
    else
        Temp32 = -Output;

    Temp32 = Temp32<<Scale_n;
    Int_ExtraFlow += Temp32*150/iDT;

    return(Output);
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: DoPressureControll()
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
void CMVController::DoPressureControll(void)
{
//    AirFlowController->Disable();
//    O2FlowController->Disable();

    SHORT Total_Flow;
    SHORT Temp16,Switch_Cons,eP,eF,uiex,uiflow,upex,upflow;
    LONG Temp32;


    Pre_Filter();

    eP = Pset - (pressMeasured);

    BiasFlow = MgrBdSetting::S_GetInstance()->GetCurrentValue(eBaseFlow)/10;

    Total_Flow = MeasuredAirFlow + MeasuredO2Flow;

    eF = BiasFlow - Total_Flow;
    Temp32 = (LONG)eF*a14;
    Temp32 = - Temp32;
    ASSERTION(a13 != 0);
    Temp32 = (LONG)Temp32/a13;
    Temp16 = (SHORT)Temp32;
    uiex = Temp16 - eP;

    if( !exhlphase )
    {
        Flow_Cons = false;
        Delay_Number = 0;
    }
    else
    {

        Delay_Number++;
        if(Delay_Number>=100)
        {
            Delay_Number = 100;
            Flow_Cons = true;
        }

        Temp16 = TargetPeepPressure - pressMeasured;
        if((Temp16 >= -10)&&(Temp16 <= 10))
            Flow_Cons = true;
    }



    if(Flow_Cons == true)
        Switch_Cons = FlowCons_Filter(0);
    else
        Switch_Cons = FlowCons_Filter(SwitchTime);

    //During the time of exahalation of IMV control , constant flow is required but
    //  not for the phase of inspiration .
    //  There are two conditions for chnaging from normal IMV to constant flow IMV,
    //  One is the abs(Pressure - actual pressure)<1 cmH2O , that is the only chnging
    //  condition up to now.
    //  The other is the time have lasted over 300ms after the exahalation phase coming.
    //      I think the second condition is necessary for the reason of that if the flow
    //  setting is high , physically IMV can not keep the PEEP , so the first condition
    //  (old condition ) will never be true.

    Temp32 = (LONG)eP*Switch_Cons;
    Temp32 = Temp32>>10;
    Temp16 = (SHORT)Temp32;
    Temp32 = (LONG)Temp16*a23;
    ASSERTION(a24 != 0);
    Temp32 = (LONG)Temp32/a24;
    Temp16 = (SHORT)Temp32;
    uiflow = Temp16 + eF;

    Temp32 = pressMeasured*a11;
    Temp32 += (LONG)Total_Flow*a12;
    Temp32 = (LONG)Temp32/10;
    upex = (SHORT)Temp32;

    Temp32 = pressMeasured*Switch_Cons;
    Temp32+= (LONG)10* TargetPeepPressure*(SwitchTime - Switch_Cons);
    Temp32 = Temp32>>10;
    Temp16 = (SHORT)Temp32;
    Temp32 = (LONG)Temp16*a21;
    Temp32 += (LONG)Total_Flow*a22;
    Temp32 = (LONG)Temp32/10;
    upflow = (SHORT)Temp32;

    Temp16 = Integrator_Ex(uiex,upex);
    ExValve_Pos = Temp16;
    ExhStep = ExValve_Filter(Up_ExLmt - Temp16);


    Temp16  = Integrator_Air(uiflow,upflow);
    AirStep = AirValve_Filter(Temp16);

    Temp16 = Integrator_O2(uiflow,upflow);
    O2Step = O2Valve_Filter(Temp16);

//    ExhValve->SetStepPosition(ExhStep);
//    AirValve->SetStepPosition(AirStep);
//    O2Valve->SetStepPosition(O2Step);
}

//*****************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Calculate()
//
//    Processing: This routine calculates the desired total flow rate
//          for the current execution cycle.  The current sensor reading
//          and desired sensor readings are used to generate a desired flow
//          rate
//
//    Input Parameters:  None
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:
//    Miscellaneous:
//
//    Requirements:
//
//******************************************************************************
void CMVController::Calculate()
{
    // Read Flow Sensors
//    MeasuredAirFlow = (AirFlowSensor->GetLastReading())/10;
//    MeasuredO2Flow  = (O2FlowSensor->GetLastReading())/10;

//    pressMeasured = ActivePressureSensor->GetLastReading()/10;
    DoPressureControll();
    DoO2Servo();
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ResetHFOStaticVariables()
//
//    Processing: Initialize all statics of MAP_Servo()
//
//    Input Parameters: None
//    Output Parameters:None
//    Return Values:    None
//    Pre-Conditions:   None
//    Miscellaneous:    None
//    Requirements:
//
//******************************************************************************
void CMVController::ResetIMVStaticVariables(void)
{
    Alf = 499;
    Bat = 1;

    Int_PreFtr1 = 0;
    Int_PreFtr2 = 0;
    Int_PreFtr3 = 0;
    Int_PrxRootFtr = 0;
    Int_ExValve = 0;
    Int_AirValve = 0;
    Int_O2Valve = 0;
    Int_AirFFtr = 0;
    Int_O2FFtr = 0;
    Int_O2SetFtr = 0;
    Int_O2Percent = 0;
    Int_FConsFtr = 0;
    Int_ExtraFlow = 0;

    Int_ExValveFtr = 0;
    Int_AirValveFtr = 0;
    Int_O2ValveFtr = 0;

    AirPDFFirstIntegrator = 0;
    O2PDFFirstIntegrator = 0;

    Flow_Cons = false;
    ExValve_Pos = 0;
    Delay_Number = 0;

    AirStep = 0;
    O2Step = 0;
    ExhStep = 0;
}

//******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MAP_AirPDFControl()
//
//    Processing: Air Flow control loop
//
//    Input Parameters:
//    Output Parameters:
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous:
//    Requirements:
//******************************************************************************/
LONG CMVController::AirPDFControl(LONG flowCmd, LONG airFlowVal)
{
    LONG error, errorGain, scaleDownIntegrator, preFilterOutput, valStep;

    LONG scaleUpInput;

    error = flowCmd - airFlowVal;

    errorGain = error * PDF_ERROR_GAIN;
    scaleUpInput = errorGain / MAP_SERVO_RATE ;
    scaleUpInput = (LONG) ( scaleUpInput << MAP_SERVO_SCALE );


    scaleDownIntegrator = (LONG) (AirPDFFirstIntegrator >> MAP_SERVO_SCALE);

    preFilterOutput = scaleDownIntegrator * PDF_SCALE_FACTOR;

    preFilterOutput /= (SHORT)FLOW_SCALER;

    if ( preFilterOutput >= MAX_STEP)
    {
        if ( errorGain <= 0 )
        {
            AirPDFFirstIntegrator += scaleUpInput;
        }
        preFilterOutput = MAX_STEP;
    }
    else if ( preFilterOutput <= 0 )
    {
        if ( errorGain >= 0 )
        {
            AirPDFFirstIntegrator += scaleUpInput;
        }
        preFilterOutput = 0;
    }
    else
    {
        AirPDFFirstIntegrator += scaleUpInput;
    }

    valStep = preFilterOutput;

    return valStep;
}
//******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: MAP_O2PDFControl()
//
//    Processing: O2 flow control loop
//
//    Input Parameters:
//    Output Parameters:
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous:
//    Requirements:
//******************************************************************************/

LONG CMVController::O2PDFControl (LONG flowCmd, LONG o2FlowVal)
{

    LONG error, errorGain, scaleDownIntegrator, preFilterOutput, valStep;
    LONG scaleUpInput;

    error = flowCmd - o2FlowVal;

    errorGain = error * PDF_ERROR_GAIN;
    scaleUpInput =  errorGain / MAP_SERVO_RATE ;
    scaleUpInput = (LONG) (scaleUpInput << MAP_SERVO_SCALE );


    scaleDownIntegrator = (LONG) (O2PDFFirstIntegrator >> MAP_SERVO_SCALE);

    preFilterOutput = scaleDownIntegrator * PDF_SCALE_FACTOR;

    preFilterOutput /= (SHORT)FLOW_SCALER;

    if ( preFilterOutput >= MAX_STEP)
    {
        if ( errorGain <= 0 )
        {
            O2PDFFirstIntegrator += scaleUpInput ;
        }
        preFilterOutput = MAX_STEP;
    }
    else if ( preFilterOutput <= 0 )
    {
        if ( errorGain >= 0 )
        {
            O2PDFFirstIntegrator += scaleUpInput ;
        }
        preFilterOutput = 0;
    }
    else
    {
        O2PDFFirstIntegrator += scaleUpInput ;
    }

    valStep = preFilterOutput;

    return valStep;
}
//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name:CompCompensatePressure(double compensateRate)
//
//    Processing:
//
//    Input Parameters: None
//
//    Output Parameters:None
//
//    Return Values:    None
//
//    Pre-Conditions:   None
//
//    Miscellaneous:    None
//
//    Requirements:
//
//******************************************************************************
//for new tube compensation
double CMVController::CompCompensatePressure(double compensateRate, bool type)
{
    double L =((double)MgrBdSetting::S_GetInstance()->GetCurrentValue(eTubeLength)/(double)SCALE);//[mm]
    double D = ((double)MgrBdSetting::S_GetInstance()->GetCurrentValue(eTubeDiameter)/(double)SCALE);//[mm]
    double C = compensateRate;
    double P_Compensate = 0;
    double Q =0;

    ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();

    if((Status==eCalibrated)||(Status==eResetAndRecovered))
    {
//        Q = ((double)ProxyFlowSensor::S_GetInstance()->GetLastReading()*(double)Btps::S_GetBtpsExhFactor()/ (double)SCALE)/100.0; // LPM

        //These are approximatary expression from experiment.
        if(D == 2){
            //about 80% from equation from experiment is match to actual pressure
            if(Q >= 0)
            P_Compensate = ((0.00007*Q*Q*Q + 0.0025*Q*Q + 0.002*Q)*L + (0.0072*Q*Q*Q + 0.0002*Q*Q + 1.1521*Q))*0.8;
            else
            P_Compensate = ((0.00007*Q*Q*Q - 0.0025*Q*Q + 0.002*Q)*L + (0.0072*Q*Q*Q - 0.0002*Q*Q + 1.1521*Q))*0.8;

            //pressure control does not good until about 60% from compensate pressure
            if(type == true){
               P_Compensate = P_Compensate * 0.6;
            }
        }
        else if(D == 2.5){
            if(Q >= 0)
            P_Compensate = ((0.00002*Q*Q*Q + 0.0005*Q*Q + 0.0026*Q)*L + (0.0014*Q*Q*Q + 0.0319*Q*Q + 0.4114*Q))*1.0;
            else
            P_Compensate = ((0.00002*Q*Q*Q - 0.0005*Q*Q + 0.0026*Q)*L + (0.0014*Q*Q*Q - 0.0319*Q*Q + 0.4114*Q))*1.0;

            if(type == true){
               P_Compensate = P_Compensate * 0.7;
            }
        }
        else if(D == 3){
            if(Q >= 0)
            P_Compensate = ((0.000004*Q*Q*Q + 0.0002*Q*Q + 0.0007*Q)*L+(0.0003*Q*Q*Q + 0.0291*Q*Q + 0.1918*Q))*1.0;
            else
            P_Compensate = ((0.000004*Q*Q*Q - 0.0002*Q*Q + 0.0007*Q)*L+(0.0003*Q*Q*Q - 0.0291*Q*Q + 0.1918*Q))*1.0;

            if(type == true){
               P_Compensate = P_Compensate * 0.9;
            }
        }
        else if(D == 3.5){
            if(Q >= 0)
            P_Compensate = (0.0000006*Q*Q*Q + 0.0002*Q*Q + 0.0001*Q)*L + (0.0001*Q*Q*Q + 0.0203*Q*Q + 0.1328*Q);
            else
            P_Compensate = (0.0000006*Q*Q*Q - 0.0002*Q*Q + 0.0001*Q)*L + (0.0001*Q*Q*Q - 0.0203*Q*Q + 0.1328*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 4){
            if(Q >= 0)
            P_Compensate = (-0.00000003*Q*Q*Q + 0.00008*Q*Q + 0.0002*Q)*L + (0.00002*Q*Q*Q + 0.0172*Q*Q + 0.0718*Q);
            else
            P_Compensate = (-0.00000003*Q*Q*Q - 0.00008*Q*Q + 0.0002*Q)*L + (0.00002*Q*Q*Q - 0.0172*Q*Q + 0.0718*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 4.5){
            if(Q >= 0)
            P_Compensate = (0.00000001*Q*Q*Q + 0.00004*Q*Q + 0.00003*Q)*L + (-0.000003*Q*Q*Q + 0.0122*Q*Q + 0.0755*Q);
            else
            P_Compensate = (0.00000001*Q*Q*Q - 0.00004*Q*Q + 0.00003*Q)*L + (-0.000003*Q*Q*Q - 0.0122*Q*Q + 0.0755*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 5){
            if(Q >= 0)
            P_Compensate = (0.00000008*Q*Q*Q + 0.00002*Q*Q + 0.0001*Q)*L + (-0.00002*Q*Q*Q + 0.011*Q*Q + 0.0539*Q);
            else
            P_Compensate = (0.00000008*Q*Q*Q - 0.00002*Q*Q + 0.0001*Q)*L + (-0.00002*Q*Q*Q - 0.011*Q*Q + 0.0539*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 5.5){
            if(Q >= 0)
            P_Compensate = (0.000000005*Q*Q*Q + 0.00001*Q*Q + 0.00007*Q)*L + (-0.00002*Q*Q*Q + 0.0092*Q*Q + 0.0496*Q);
            else
            P_Compensate = (0.000000005*Q*Q*Q - 0.00001*Q*Q + 0.00007*Q)*L + (-0.00002*Q*Q*Q - 0.0092*Q*Q + 0.0496*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 6){
            if(Q >= 0)
            P_Compensate = (0.00000004*Q*Q*Q + 0.000008*Q*Q + 0.00006*Q)*L + (-0.00002*Q*Q*Q + 0.0088*Q*Q + 0.0481*Q);
            else
            P_Compensate = (0.00000004*Q*Q*Q - 0.000008*Q*Q + 0.00006*Q)*L + (-0.00002*Q*Q*Q - 0.0088*Q*Q + 0.0481*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 6.5){
            if(Q >= 0)
            P_Compensate = (0.00000002*Q*Q*Q + 0.000006*Q*Q + 0.00005*Q)*L + (-0.00002*Q*Q*Q + 0.0081*Q*Q + 0.0437*Q);
            else
            P_Compensate = (0.00000002*Q*Q*Q - 0.000006*Q*Q + 0.00005*Q)*L + (-0.00002*Q*Q*Q - 0.0081*Q*Q + 0.0437*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 7){
            if(Q >= 0)
            P_Compensate = (-0.00000002*Q*Q*Q + 0.000006*Q*Q + 0.00002*Q)*L + (-0.00002*Q*Q*Q + 0.0076*Q*Q + 0.0442*Q);
            else
            P_Compensate = (-0.00000002*Q*Q*Q - 0.000006*Q*Q + 0.00002*Q)*L + (-0.00002*Q*Q*Q - 0.0076*Q*Q + 0.0442*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 7.5){
            if(Q >= 0)
            P_Compensate = (-0.00000004*Q*Q*Q + 0.000004*Q*Q + 0.00001*Q)*L + (-0.00002*Q*Q*Q + 0.0073*Q*Q + 0.0455*Q);
            else
            P_Compensate = (-0.00000004*Q*Q*Q - 0.000004*Q*Q + 0.00001*Q)*L + (-0.00002*Q*Q*Q - 0.0073*Q*Q + 0.0455*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }
        else if(D == 8){
            if(Q >= 0)
            P_Compensate = (0.000000005*Q*Q*Q + 0.000001*Q*Q + 0.00004*Q)*L + (-0.00002*Q*Q*Q + 0.0076*Q*Q + 0.0377*Q);
            else
            P_Compensate = (0.000000005*Q*Q*Q - 0.000001*Q*Q + 0.00004*Q)*L + (-0.00002*Q*Q*Q - 0.0076*Q*Q + 0.0377*Q);

            if(type == true){
               P_Compensate = P_Compensate;
            }
        }

    }


    return P_Compensate*C/100.0 ;
}
