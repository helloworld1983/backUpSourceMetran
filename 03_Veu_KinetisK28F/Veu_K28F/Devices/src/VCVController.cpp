/******************************************************************************/
//$COMMON.H$
//    Filename: VCVController.cpp
//    Copyright 1996 InVentive Technologies, Inc. All Rights Reserved.
//
//    Class:  VCVController
//    This class defines the tasks which perform the Pressure control
//    calculations and sends the desired flow valve step positions
//    the the FlowController objects for the air and oxygen flow paths
//    which in turn calculate step positions for the air and oxygen valve
//    controllers.
/******************************************************************************/

#include "VCVController.h"

#include "MgrFlowSens.h"
#include "PressureSensor.h"
#include "MgrFlowController.h"
#include "MgrStemMotor.h"

#include "MgrBdSetting.h"
#include "RootTable.h"

#include "DebugTrace.h"

const SHORT DELAY_TIME = 20;
const SHORT MAX_STEP = 2300;
const USHORT DOWN_FLOW_LMT = 0;
const USHORT INT_UP_FLOW_LMT = 2200;
const USHORT SCALE_N = 8;
const USHORT SWITCH_TIME = 256;
const USHORT Nco = 2000;
const USHORT iDT = 500;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance
//
//    Processing: returns the static instance of the VCVController class
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: pointer to VCVController class
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
//VCVController* VCVController::S_GetInstance (void)
//{
//    //Create object if it is null
//    if (S_Instance == NULL)
//    {
//        S_Instance = new VCVController();
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
//    Processing: initialize VCVController
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
void VCVController::Init (void)
{
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetDesiredFlow(LONG desiredAir,LONG desiredO2)
//
//    Processing:
//
//    Input Parameters:   desiredAir,desiredO2
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void VCVController::SetDesiredFlow(LONG desiredAir,LONG desiredO2)
{
    desiredAirFlow = desiredAir;
    desiredO2Flow = desiredO2;
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
void VCVController::Calculate(void)
{
    MeasuredAirFlow = (MgrFlowSens::AirFlowSensor->GetLastReading())/10;
    MeasuredO2Flow  = (MgrFlowSens::O2FlowSensor->GetLastReading())/10;

    pressMeasured = PressureSensor::ActivePressureSensor->GetLastReading()/10;

    if(currentPhaseId == eVcvInhPhase)
    {
        InhProcess(desiredAirFlow,desiredO2Flow);
        Inh_TotalFlow += (MgrFlowSens::AirFlowSensor->GetLastReading()
                + MgrFlowSens::O2FlowSensor->GetLastReading());
    }
    else if(currentPhaseId == eThePlateauPhase)
    {
        SetDesiredFlow(0,0);
        InhProcess(0,0);
        Inh_TotalFlow += (MgrFlowSens::AirFlowSensor->GetLastReading()
                + MgrFlowSens::O2FlowSensor->GetLastReading());
    }
    else if(currentPhaseId == ePsvInhPhase)//for PSV trigger
    {
        IsPcv_cnt = true;
        DoPressureControll();
    }
    else // exh phase
    {
        if(Inh_TotalFlow != 0){
            //change flow to volume data at 2ms cycle data
            Inh_OutputVol = (LONG)(2*Inh_TotalFlow/60);
        }
        Inh_TotalFlow = 0;
        //                a13 = -670;
        ExhProcess();
    }
}

//*****************************************************************************
//$COMMON.OPERATION$
//    Operation Name: SetSepecificPhase(E_SpecificPhaseId specificPhaseId)
//
//    Processing:
//
//    Input Parameters:  E_SpecificPhaseId specificPhaseId
//    Output Parameters: None
//    Return Values:     None
//    Pre-Conditions:
//    Miscellaneous:
//
//    Requirements:
//
//******************************************************************************
void VCVController::SetSepecificPhase(E_SpecificPhaseId specificPhaseId)
{
    currentPhaseId = specificPhaseId;
    switch(currentPhaseId)
    {
        case ePsvInhPhase:
            MgrFlowController::O2FlowController->Disable();
            MgrFlowController::AirFlowController->Disable();
            exhlphase = eFalse;
            break;

        case eVcvInhPhase:
            MgrFlowController::AirFlowController->SetStaticAgr(MgrStemMotor::AirValve->GetStepPosition());
            MgrFlowController::O2FlowController->SetStaticAgr(MgrStemMotor::O2Valve->GetStepPosition());
            MgrFlowController::O2FlowController->Enable();
            MgrFlowController::AirFlowController->Enable();
            exhlphase = eFalse;
            IsFirstBreath = false;
            delayBeforeExh = 0;
            break;

        case eThePlateauPhase:
            MgrFlowController::O2FlowController->Enable();
            MgrFlowController::AirFlowController->Enable();
            exhlphase = eFalse;
            IsFirstBreath = false;
            delayBeforeExh = 0;
            break;

        case eNoTrigExhPhase:
            exhlphase = eTrue;
            break;

        default:
            break;
    }
}

//*****************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Disable()
//
//    Processing: Disable VCV controller
//
//    Input Parameters:
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous:  None
//
//    Requirements:
//
//*****************************************************************************
void VCVController::Disable(void)
{
    Enabled = eFalse;

    MgrFlowController::AirFlowController->Reset();
    MgrFlowController::O2FlowController->Reset();

    MgrFlowController::AirFlowController->Disable();
    MgrFlowController::O2FlowController->Disable();

    delayBeforeExh = DELAY_TIME;
    IsFirstBreath = true;
    maxPIP = 0;

    maxPlateau = 0;
    premaxPlateau=0;

    TargetInspPressure = 0;

    Inh_OutputVol = 0;
    Inh_TotalFlow = 0;
}

//*****************************************************************************
//$COMMON.OPERATION$
//    Operation Name: Get_Inh_Vol()
//
//    Processing: Get inhalation volume
//
//    Input Parameters:
//
//    Output Parameters:  None
//    Return Values: Inh_OutputVol - inhalation volume
//    Pre-Conditions:
//    Miscellaneous:  None
//
//    Requirements:
//
//*****************************************************************************
LONG VCVController::Get_Inh_Vol (void)
{
    return Inh_OutputVol;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: VCVController()
//
//    Processing:   Constructor for VCVController
//
//    Input Parameters:   None
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
VCVController::VCVController (void) : CMVController()
{
    desiredPeakFlow = 0; // edit
    currentPhaseId = eTheNonePhase ;

    IsFirstBreath = true;
    delayBeforeExh = DELAY_TIME;
    Total_Flow = 0;
    premaxPIP=0;
    maxPIP = 0;

    maxPlateau = 0;
    premaxPlateau=0;

    IsPcv_cnt= false;
    desiredAirFlow = 0;
    desiredO2Flow = 0;

    a11 = 159;//126;//115;//154;
    a12 = 22;
    a13 = -1230;//-906;//-1300;
    a14 = 20;//350;//424;

    a21 = -2;
    a22 = -54;
    a23 = 5674;//1277;//1455;
    a24 = 2166;//846;

    Pressure_Gain = 2000;        /* 1.4 * 1024 */
    Flow_Gain  = 1957;//363; //2125;      /* Nco * 1.4, Nco=1500*/

    Inh_OutputVol = 0;
    Inh_TotalFlow = 0;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: InhProcess()
//
//    Processing:
//
//    Input Parameters:   desiredAir,desiredO2
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void VCVController::InhProcess(LONG desiredAir, LONG desiredO2)
{
    MgrStemMotor::ExhValve->RampToPosition(MAX_STEP, eTrue);
    MgrFlowController::AirFlowController->SetDesired(desiredAir);
    MgrFlowController::O2FlowController->SetDesired(desiredO2);
    CalExhArgs();
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ExhProcess()
//
//    Processing:
//
//    Input Parameters:   None
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void VCVController::ExhProcess(void)
{
    exhlphase = eTrue;
    if(delayBeforeExh < DELAY_TIME)
    {
        delayBeforeExh++;

        MgrStemMotor::ExhValve->RampToPosition(0);
        MgrFlowController::AirFlowController->SetDesired(0);
        MgrFlowController::O2FlowController->SetDesired(0);
        CalExhArgs();
        return;
    }

    if(maxPIP!=0)
        premaxPIP = maxPIP;
    maxPIP=0;

    if(maxPlateau != 0)
    {
        premaxPlateau = maxPlateau;
    }
    maxPlateau=0;

    MgrFlowController::O2FlowController->Disable();
    MgrFlowController::AirFlowController->Disable();


    DoPressureControll();
    DoO2Servo();
    return;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ExhProcess()
//
//    Processing:
//
//    Input Parameters:   None
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void VCVController::CalPressArgs(void)
{
    int32_t Temp32;
    int16_t Temp16;
    pressMeasured = (int32_t)( pressMeasured);
    int16_t InhFlow = (MeasuredAirFlow + MeasuredO2Flow);

    // Air Valve
    int16_t CurrentAirValvePos = (int16_t)((MgrStemMotor::AirValve->GetStepPosition()));

    if(CurrentAirValvePos < DOWN_FLOW_LMT)
    {
        CurrentAirValvePos = DOWN_FLOW_LMT;
    }
    else if (CurrentAirValvePos > INT_UP_FLOW_LMT)
    {
        CurrentAirValvePos = INT_UP_FLOW_LMT;
    }

    Int_AirValveFtr = (int32_t)(CurrentAirValvePos) << SCALE_N;

    int16_t u;
    if(CurrentAirValvePos < Flow_Gain)
    {
        u = CurrentAirValvePos * Nco / (Flow_Gain - CurrentAirValvePos) * 500 / Alf;
    }
    else
    {   // Must set the Flow_Gain > INT_UP_FLOW_LMT to
        // this condition don't be triggered.
        u = Flow_Gain;
    }

    Temp32 = pressMeasured * SWITCH_TIME;
    Temp32 = Temp32 >> 10;
    Temp16 = (SHORT)Temp32;
    Temp32 = (LONG)Temp16 * a21;
    Temp32 += (LONG)InhFlow * a22;
    Temp32 = (LONG)Temp32 / 10;
    Temp16 = (SHORT)Temp32;

    Temp16 = u - Temp16;
    Int_AirValve = (int32_t)(Temp16) << SCALE_N;

    // O2 Valve
    int16_t CurrentO2ValvePos = (int16_t)((MgrStemMotor::O2Valve->GetStepPosition()));

    if(CurrentO2ValvePos < DOWN_FLOW_LMT)
    {
        CurrentO2ValvePos = DOWN_FLOW_LMT;
    }
    else if (CurrentO2ValvePos > INT_UP_FLOW_LMT)
    {
        CurrentO2ValvePos = INT_UP_FLOW_LMT;
    }

    Int_O2ValveFtr = (int32_t)(CurrentO2ValvePos) << SCALE_N;

    if(CurrentO2ValvePos < Flow_Gain)
    {
        u = CurrentO2ValvePos * Nco/(Flow_Gain - CurrentO2ValvePos) * 500 / Bat;
    }
    else
    {   // Must set the Flow_Gain > INT_UP_FLOW_LMT to
        // this condition don't be triggered.
        u = Flow_Gain;
    }

    Temp32 = pressMeasured*SWITCH_TIME;
    Temp32 = Temp32>>10;
    Temp16 = (SHORT)Temp32;
    Temp32 = (LONG)Temp16*a21;
    Temp32 += (LONG)InhFlow*a22;
    Temp32 = (LONG)Temp32/10;
    Temp16 = (SHORT)Temp32;

    Temp16 = u - Temp16;
    Int_O2Valve = (int32_t)(Temp16) << SCALE_N;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: ExhProcess()
//
//    Processing:
//
//    Input Parameters:   None
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void VCVController::CalExhArgs(void)
{
    SHORT Total_Flow;
    SHORT Temp16,Switch_Cons,eP,eF,uiex,uiflow,upex,upflow;
    LONG Temp32;


    if(maxPIP < pressMeasured)
        maxPIP = pressMeasured;

    if(maxPlateau<pressMeasured)
        maxPlateau = pressMeasured;

    if(currentPhaseId==eVcvInhPhase)
    {
        TargetInspPressure = premaxPIP;
    }
    else
    {
        TargetInspPressure = premaxPlateau;
    }

    TargetInspPressure=premaxPIP;
    exhlphase = eFalse;
    Pre_Filter();

    eP = Pset - (pressMeasured);


    BiasFlow =  MgrBdSetting::S_GetInstance()->GetCurrentValue(eBaseFlow)/10;

    Total_Flow = (MeasuredAirFlow + MeasuredO2Flow);

    eF = (BiasFlow - Total_Flow);

    Temp32 = (LONG)eF * a14;
    Temp32 = - Temp32;

    ASSERTION(a13 != 0);

    Temp32 = (LONG)Temp32 / a13;
    Temp16 = (SHORT)Temp32;
    uiex = Temp16 - eP;


    Flow_Cons = false;
    Delay_Number = 0;
    IsPcv_cnt = false;

    Switch_Cons = FlowCons_Filter(SWITCH_TIME);

    Temp32 = (LONG)eP*SWITCH_TIME;
    Temp32 = Temp32 >> 10;
    Temp16 = (SHORT)Temp32;
    Temp32 = (LONG)Temp16 * a23;

    ASSERTION(a24 != 0);

    Temp32 = (LONG)Temp32 / a24;
    Temp16 = (SHORT)Temp32;
    uiflow = Temp16 + eF;

    Temp32 = pressMeasured * a11;
    Temp32 += (LONG)Total_Flow * a12;
    Temp32 = (LONG)Temp32 / 10;
    upex = (SHORT)Temp32;

    Temp32 = pressMeasured * SWITCH_TIME;
    Temp32 += (LONG)10* TargetPeepPressure*(SWITCH_TIME - Switch_Cons);
    Temp32 = Temp32>>10;
    Temp16 = (SHORT)Temp32;
    Temp32 = (LONG)Temp16*a21;
    Temp32 += (LONG)Total_Flow*a22;
    Temp32 = (LONG)Temp32/10;
    upflow = (SHORT)Temp32;


    //Calculate Exh Valve Args
    CalculateExhValveArgs(uiex,upex);

    //Calculate Air O2 Valve Args
    CalculateAirValveArgs(uiflow,upflow);
    CalculateO2ValveArgs(uiflow,upflow);
    DoO2Servo();
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CalculateExhValveArgs(SHORT uiex,SHORT upex)
//
//    Processing:
//
//    Input Parameters:   None
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void VCVController::CalculateExhValveArgs(SHORT uiex,SHORT upex)
{
    SHORT Temp16;
    LONG Temp32;
    uint16_t Root16;
    SHORT Temp16_1;
    LONG Temp32_1;
    USHORT exhStep = MgrStemMotor::ExhValve->GetStepPosition();

    Temp16 = MAX_STEP_POSITION - exhStep;

    ExValve_Pos = Temp16;

    Temp16_1 = PLFilter_Root(pressMeasured);
    if(Temp16_1 < 10)
    {
        Temp16_1 = 10;
    }
    else
    {
        if(Temp16_1>1000)
        {
            Temp16_1 = 1000;
        }
    }

    if(Temp16_1<255)
    {
        Root16 = m_table[Temp16_1];
        Temp32_1 = (LONG)Root16*1000;
        Temp32_1 = (LONG)Temp32_1/3162; /* Temp32 = Root16/sqrt(10) */
        Temp32_1 = Temp32_1>>2;
        Temp16_1 = (SHORT)Temp32_1;
    }
    else
    {
        Temp16_1 = Temp16_1/10;
        Root16 = m_table[Temp16_1];
        Temp16_1 = Root16>>2;
    }

    Temp16_1 += 2048;

    Temp32_1 = (LONG)Temp16*Temp16_1;

    Temp16 = (SHORT)(Temp32_1 / Pressure_Gain);
    Temp16 = Temp16 - upex;
    Int_ExValve = Temp16 << SCALE_N;

    Temp32 = uiex;
    Temp32 = Temp32<<SCALE_N;
    Temp32 = -Temp32*a13/10;
    Temp32 = Temp32/iDT;
    Int_ExValve += Temp32;

    Int_ExValveFtr = ((LONG)exhStep) << SCALE_N;

    Rise_Time = 10;
    Int_PrxRootFtr = (int32_t)(TargetInspPressure) << SCALE_N;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CalculateAirValveArgs(SHORT ui,SHORT up)
//
//    Processing:
//
//    Input Parameters:   None
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void VCVController::CalculateAirValveArgs(SHORT ui,SHORT up)
{
    SHORT Temp16;
    LONG Temp32;
    SHORT airStep = MgrStemMotor::AirValve->GetStepPosition();
    Temp16 = airStep;

    if(airStep < Flow_Gain)
        Int_AirValve = ((LONG)((LONG)(((LONG)(Nco*Temp16/(Flow_Gain - Temp16))*500)/Alf) - up))<<SCALE_N;
    else
        Int_AirValve = (LONG)(Flow_Gain - up)<<SCALE_N;

    Temp32 = ui;
    Temp32 = Temp32<<SCALE_N;
    Temp32 = (LONG)Temp32*a24/10;
    Temp32 = (LONG)Temp32/iDT;
    Int_AirValve += Temp32;

    if(Int_AirValve>=(LONG)0x003fffff){
        Int_AirValve = (LONG)0x003fffff;
    }
    else if(Int_AirValve<=(LONG)0xffc00000){
        Int_AirValve = (LONG)0xffc00000;
    }

    Int_AirValveFtr = ((LONG)airStep) << SCALE_N;
}

//******************************************************************************
//$COMMON.OPERATION$
//    Operation Name: CalculateO2ValveArgs(SHORT ui,SHORT up)
//
//    Processing:
//
//    Input Parameters:   None
//
//    Output Parameters:  None
//    Return Values:
//    Pre-Conditions:
//    Miscellaneous: None
//    Requirements:
//
//******************************************************************************
void VCVController::CalculateO2ValveArgs(SHORT ui,SHORT up)
{
    SHORT Temp16 , Up_FlowLmt;
    LONG Temp32;

    Up_FlowLmt = INT_UP_FLOW_LMT;

    SHORT o2Step = MgrStemMotor::AirValve->GetStepPosition();
    Temp16 = o2Step;
    if(o2Step < Flow_Gain)
        Int_O2Valve = ((LONG)((LONG)(((LONG)(Nco*Temp16/(Flow_Gain - Temp16))*500)/Bat) - up))<<SCALE_N;
    else
        Int_O2Valve = (LONG)(Flow_Gain - up)<<SCALE_N;
    Temp32 = ui;
    Temp32 = Temp32 << SCALE_N;
    Temp32 = (LONG)Temp32 * a24 / 10;
    Temp32 = (LONG)Temp32 / iDT;
    Int_O2Valve += Temp32;

    if(Int_O2Valve >= (LONG)0x003fffff)
    {
        Int_O2Valve = (LONG)0x003fffff;
    }
    else
    {
        if(Int_O2Valve<=(LONG)0xffc00000)
        {
            Int_O2Valve = (LONG)0xffc00000;
        }
    }

    Int_O2ValveFtr = ((LONG)o2Step) << SCALE_N;
}

void VCVController::DoPressureControll (void)
{
}
