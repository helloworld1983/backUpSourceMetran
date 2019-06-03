/*
 * CMVController.c
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */
#include <CommonTypes.h>
#include <deviceTask/DeviceInterface.h>
#include "CMVController.h"
#include "FlowController.h"
#include "DebugTrace.h"
#include "RootTable.h"
#include "kiss_fftr.h"
#include "_kiss_fft_guts.h"
#include "math.h"

#define SCALE 100
#define FFT_EXAMINE_TIME 500/2
#define OSCILLATION_RANGE 2.5
static E_ParameterType gs_ParameterType=eSmallCompliance;
static int32_t gs_MeasuredAirFlow = 0;
static int32_t gs_MeasuredO2Flow  = 0;
static int32_t gs_PressureMeasured = 0;
static int32_t gs_RS485ProxySensor = 0;
static ProxyStatus gs_RS485Status = eNotCalibrated;
static bool gs_IsExhPhase = false;
static float gs_IntPreFtr1=0;
static uint16_t gs_TargetInspPressure= 0;
static uint16_t gs_TargetPEEPPressure= 0;
static uint16_t gs_SetRiseTime=10;
static const uint16_t gs_iDT=500;           //200 for 5ms interval, 500 for 2ms interval
static int16_t gs_PSet=0;
static uint16_t gs_BiasFlow=0;
static int16_t gs_a11=340;//126 110 //500
static int16_t gs_a12=22;
static int16_t gs_a13=-3000;//-906 -1000
static int16_t gs_a14=20;//20

static int16_t gs_a21=-2;//-2
static int16_t gs_a22=-54;//-54
static int16_t gs_a23=4000;//5674
static int16_t gs_a24=2000;//2166

static int16_t gs_PressureGain=1190;//2000
static int16_t gs_FlowGain=1190;//1957

static unsigned char gs_FlowCons = false;
static int16_t gs_DelayNumber = 0;
static int32_t gs_IntFConsFtr=0;
static uint16_t gs_SwitchTime     =768;//256
static const uint16_t gs_Scalen        =    8;
static int32_t gs_IntExValve=0;
static int32_t gs_IntPrxRootFtr=0;
static uint16_t gs_ValveFtrConst =    30;//100
static uint16_t gs_ValveFtrExhConst =    20;//100

static int32_t gs_IntAirValve=0;
static int16_t gs_Alf = 499;
static int16_t gs_Bat = 1;

static uint16_t gs_InhUpFlowLmt = 2200;//Current Vue: 2200
static uint16_t gs_ExhUpFlowLmt = 	1700;//Current Vue: 1500
static uint16_t gs_DownFlowLmt    =   0;
static uint16_t gs_UpExLmt        = 2000;//Current Vue: 2200
static uint16_t gs_DownExLmt        =    0;

static int32_t gs_IntAirValveFtr=0;
static int32_t gs_IntO2Percent=0;
static uint16_t gs_DownO2PerLmt    =    1;
static uint16_t gs_UpO2PerLmt    =    499;
static int32_t gs_IntO2ValveFtr=0;
static int16_t gs_ExValvePos=0;
static int16_t gs_AirStep=0;
static int16_t gs_O2Step=0;
static int16_t gs_ExhStep=0;
static uint16_t gs_Nco            =    2000;//2000

static int32_t gs_IntExValveFtr=0;
static int32_t gs_IntO2Valve=0;
static int32_t gs_IntAirFFtr=0;
static int32_t gs_IntO2FFtr=0;
static uint16_t gs_O2PerSet=0;
static int32_t gs_IntO2SetFtr=0;
static int16_t gs_O2Percent=0;
static bool gs_IsEnabled=false;
static uint32_t gs_SettlingTimeCnt=0;
static int32_t gs_PeakPressure=0;
static uint32_t gs_RiseTime=0;
#define SETTLING_ERROR_RANGE 10
#define NFFT 512
static kiss_fft_cpx sout[NFFT]={0};
static kiss_fft_scalar rinData[NFFT]={0};
static kiss_fft_scalar rin[NFFT]={0};
static kiss_fftr_cfg  kiss_fftr_state;
static int16_t gs_cntFFT = 0;
static bool gs_IsNewPhase = true;
static desiredMotorPositionStruct gs_DesiredMotorPosition;

static void CMVController_Calculate();
static void CMVController_DoPressureControl();
static void CMVController_PreFilter();
static float CMVController_FirstPreFilter(float);
static float CMVController_CompCompensatePressure(double compensateRate,bool type);
static int16_t CMVController_FlowConsFilter(int16_t u);
static int16_t CMVController_IntegratorEx(int16_t ui, int16_t up);
static int16_t CMVController_PressureRootGain(int16_t u);
static int16_t  CMVController_PLFilterRoot(int16_t u);
static int16_t CMVController_IntegratorAir(int16_t ui,int16_t up);
static int16_t CMVController_GainNonCom(int16_t u);
static int16_t CMVController_IntegratorO2Percent(int16_t u);
static int16_t  CMVController_O2ValveFilter(int16_t u);
static int16_t CMVController_ExValveFilter(int16_t u);
static int16_t CMVController_AirValveFilter(int16_t u);
static int16_t CMVController_IntegratorO2(int16_t ui,int16_t up);
static void CMVController_DoO2Servo(void);
static int16_t  CMVController_AirFilter(int16_t u);
static int16_t  CMVController_O2Filter(int16_t u);
static int16_t  CMVController_O2SetFilter(int16_t u);
/**
 * Set parameter a11
 * @param val parameter
 */
void CMVController_SetA11(int16_t val)
{
	gs_a11 = val;
}
/**
 * Set parameter a12
 * @param val parameter
 */
void CMVController_SetA12(int16_t val)
{
	gs_a12 = val;
}
/**
 * Set parameter a13
 * @param val parameter
 */
void CMVController_SetA13(int16_t val)
{
	gs_a13 = val;
}
/**
 * Set parameter a14
 * @param val parameter
 */
void CMVController_SetA14(int16_t val)
{
	gs_a14 = val;
}
/**
 * Set parameter a21
 * @param val parameter
 */
void CMVController_SetA21(int16_t val)
{
	gs_a21 = val;
}
/**
 * Set parameter a22
 * @param val parameter
 */
void CMVController_SetA22(int16_t val)
{
	gs_a22 = val;
}
/**
 * Set parameter a23
 * @param val parameter
 */
void CMVController_SetA23(int16_t val)
{
	gs_a23 = val;
}
/**
 * Set parameter a24
 * @param val parameter
 */
void CMVController_SetA24(int16_t val)
{
	gs_a24 = val;

}
/**
 * Check whether CMV controller is enable
 * @return gs_IsEnabled return True if CMV controller is enable,
 * 						otherwise return False
 */
bool CMVController_IsEnable()
{
	return gs_IsEnabled;
}
/**
 * Set all parameter to be used for small compliance lung
 */
void CMVController_SetSmallComplianceParameter()
{
	gs_a11=130;//300
	gs_a12=22;
	gs_a13=-1500;//-5000
	gs_a14=20;
	gs_a21=-2;
	gs_a22=-54;
	gs_a23=20000;
	gs_a24=3500;
	gs_PressureGain=900;
	gs_FlowGain=900;
	gs_SwitchTime=728;
	gs_ValveFtrConst=50;
	gs_ValveFtrExhConst=40;
	gs_InhUpFlowLmt=2200;
	gs_ExhUpFlowLmt=2200;
	gs_DownFlowLmt=550;
	gs_UpExLmt=2050;
	gs_DownExLmt=0;
	gs_Nco=2000;
}
/**
 * Set all parameter to be used for large compliance lung
 */
void CMVController_SetLargeComplianceParameter()
{
	gs_a11=600;
	gs_a12=22;
	gs_a13=-5200;
	gs_a14=20;
	gs_a21=-2;
	gs_a22=-54;
	gs_a23=20000;//8000
	gs_a24=2500;
	gs_PressureGain=1000;
	gs_FlowGain=1200;
	gs_SwitchTime=768;
	gs_ValveFtrConst=30;
	gs_ValveFtrExhConst=20;
	gs_InhUpFlowLmt=2200;
	gs_ExhUpFlowLmt=2200;
	gs_DownFlowLmt=550;
	gs_UpExLmt=2050;
	gs_DownExLmt=0;
	gs_Nco=2000;
}
/**
 * Get breath phase
 * @return bool gs_IsExhPhase Return True if it is in Exhalation phase,
 * 							  otherwise return False
 */
bool CMVController_GetBreathPhase()
{
	return gs_IsExhPhase;
}
/**
 * Get base flow
 * @return uint16_t gs_BiasFlow Base flow in Lpm*10
 */
uint16_t CMVController_GetBaseFlow()
{
	return gs_BiasFlow;
}
/**
 * Get current set Pressure
 * @return int16_t gs_PSet Pressure in cmH20*10
 */
int16_t CMVController_GetCurrentSetPressure()
{
	return gs_PSet;
}
/**
 * Disable CMV Controller
 */
void CMVController_Disable()
{
	gs_IsEnabled = false;

}
/**
 * Enable CMV Controller
 */
void CMVController_Enable()
{
	gs_IsEnabled = true;

}
/**
 * Initialize all static variables of CMV controller
 */
void CMVController_ResetIMVStaticVariables()
{
	gs_Alf = 499;
	gs_Bat = 1;
	gs_O2Percent = 0;

	gs_IntPreFtr1 = 0;
	gs_IntPrxRootFtr = 0;
	gs_IntExValve = 0;
	gs_IntAirValve = 0;
	gs_IntO2Valve = 0;
	gs_IntAirFFtr = 0;
	gs_IntO2FFtr = 0;
	gs_IntO2SetFtr = 0;
	gs_IntO2Percent = 0;
	gs_IntFConsFtr = 0;

	gs_IntExValveFtr = 0;
	gs_IntAirValveFtr = 0;
	gs_IntO2ValveFtr = 0;

	gs_FlowCons = false;
	gs_ExValvePos = 0;
	gs_DelayNumber = 0;

	gs_AirStep = 0;
	gs_O2Step = 0;
	gs_ExhStep = 0;
	gs_SettlingTimeCnt=0;
	gs_PeakPressure=0;
	gs_RiseTime=0;
	gs_ParameterType= eSmallCompliance;
	CMVController_SetSmallComplianceParameter();
}
/**
 * This is the main processing loop for the Inhalation Controllers.
 * The routine obtains the current sensor reading, calculates the desired
 * total flow rate, splits the flow rate into air and oxygen rates, and
 * then commands the Flow Controllers to their respective desired rates.
 * @param monitor Monitor struct
 * @return desiredMotorPositionStruct gs_DesiredMotorPosition Desired motor position
 */
desiredMotorPositionStruct CMVController_Run(RealTimeM4ToA53* monitor)
{
	gs_DesiredMotorPosition.desiredAirPosition=-1;
	gs_DesiredMotorPosition.desiredExhPosition=-1;
	gs_DesiredMotorPosition.desiredHFOPosition=-1;
	gs_DesiredMotorPosition.desiredO2Position=-1;

	if(gs_IsEnabled==true)
	{
		gs_MeasuredAirFlow = monitor->airFlow.lastReading/10;
		gs_MeasuredO2Flow =  monitor->o2Flow.lastReading/10;
		gs_PressureMeasured = monitor->activePress.lastReading/10;
		gs_RS485ProxySensor = monitor->proxy.currentReading;
		if(!gs_IsExhPhase )
		{
			if( abs(gs_PressureMeasured-gs_TargetInspPressure)>SETTLING_ERROR_RANGE)
				gs_SettlingTimeCnt++;
			if(gs_PressureMeasured<gs_TargetInspPressure*0.9)
				gs_RiseTime++;
			else
			{

			}
			if(gs_PressureMeasured>=gs_PeakPressure)
			{
				gs_PeakPressure=gs_PressureMeasured;
			}
			rinData[gs_cntFFT]=gs_PressureMeasured-gs_TargetInspPressure;
			gs_cntFFT++;
			if(gs_cntFFT>=NFFT)
			{
				gs_cntFFT=0;
			}
		}
		CMVController_Calculate();
	}

	return gs_DesiredMotorPosition;

}
/**
 * This routine calculates the desired total flow rate
 * for the current execution cycle.  The current sensor reading
 * and desired sensor readings are used to generate a desired flow
 * rate
 */
void CMVController_Calculate()
{
	CMVController_DoPressureControl();
	CMVController_DoO2Servo();
}
/**
 * Air Flow filter
 * @param u
 * @return int16_t Output
 */
int16_t  CMVController_AirFilter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntAirFFtr>>10);
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<10;

	gs_IntAirFFtr += (int32_t)Temp32/gs_iDT;

	return(Output);
}
/**
 * O2 Flow filter
 * @param u
 * @return int16_t Output
 */
int16_t  CMVController_O2Filter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntO2FFtr>>10);
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<10;

	gs_IntO2FFtr += Temp32/gs_iDT;

	return(Output);
}
/**
 * O2 Set filter
 * @param u
 * @return
 */
int16_t  CMVController_O2SetFilter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntO2SetFtr>>10);
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<10;

	gs_IntO2SetFtr += Temp32/gs_iDT;

	return(Output);
}
/**
 * Do O2 servo
 */
void CMVController_DoO2Servo(void)
{

	int16_t Temp16,AverageAirF,AverageO2F,O2SetFtr;
	int32_t Temp32A,Temp32B;

	AverageAirF = CMVController_AirFilter(gs_MeasuredAirFlow);
	AverageO2F = CMVController_O2Filter(gs_MeasuredO2Flow);

	gs_O2PerSet = ((79*devIf_GetSetting()->ratio.DesiredO2Ratio) >> 7)+ 21;

	O2SetFtr = CMVController_O2SetFilter(gs_O2PerSet);

	if(O2SetFtr < 21 )
	{
		O2SetFtr = 21;
	}

	{
		Temp16 = AverageAirF + AverageO2F;
		if(Temp16<0)
			Temp16 = - Temp16;


		Temp32A = (int32_t)AverageAirF*21000;
		Temp32A += (int32_t)AverageO2F*100000;
		Temp32B = (int32_t)Temp16*1000;
		Temp32B += 1;
		ASSERTION(Temp32B != 0);
		Temp32A = (int32_t)Temp32A/Temp32B;
		gs_O2Percent = (int16_t)Temp32A;


		if(gs_O2Percent>=100)
			gs_O2Percent = 100;
		else if(gs_O2Percent<21)
			gs_O2Percent = 21;

		if(gs_O2PerSet == 100)
		{
			gs_Bat = 499;
			gs_Alf = 1;
			gs_IntO2Percent = 510977;
			gs_IntO2SetFtr = 102400 ;
		}
		else
		{
			gs_Bat = CMVController_IntegratorO2Percent(O2SetFtr - gs_O2Percent);

			gs_Alf = 500 - gs_Bat;
		}
	}
}
/**
 * Do Pressure control
 */
void CMVController_DoPressureControl()
{
	FlowController_SetEnable(eAirFlowController,false);
	FlowController_SetEnable(eO2FlowController,false);

	int16_t TotalFlow;
	int16_t Temp16,SwitchCons,eP,eF,uiex,uiflow,upex,upflow;
	int32_t Temp32;

	CMVController_PreFilter();

	eP = gs_PSet - (gs_PressureMeasured);

	gs_BiasFlow = (devIf_GetSetting()->pcv.baseFlow/10);

	TotalFlow = gs_MeasuredAirFlow + gs_MeasuredO2Flow;

	eF = gs_BiasFlow - TotalFlow;

	Temp32 = (int32_t)eF*gs_a14;
	Temp32 = - Temp32;
	ASSERTION(gs_a13 != 0);
	Temp32 = (int32_t)Temp32/gs_a13;
	Temp16 = (int16_t)Temp32;
	uiex = Temp16 - eP;

	if( !gs_IsExhPhase )
	{
		gs_FlowCons = false;
		gs_DelayNumber = 0;
	}
	else
	{
		gs_DelayNumber++;
		if(gs_DelayNumber>=100)
		{
			gs_DelayNumber = 100;
			gs_FlowCons = true;
		}

		Temp16 = gs_TargetPEEPPressure - gs_PressureMeasured;
		if((Temp16 >= -10)&&(Temp16 <= 10))
			gs_FlowCons = true;
	}

	if(gs_FlowCons == true)
	{
		SwitchCons = CMVController_FlowConsFilter(0);
	}
	else
	{
		SwitchCons = CMVController_FlowConsFilter(gs_SwitchTime);

	}

	//During the time of exhalation of IMV control , constant flow is required but
	//  not for the phase of inspiration .
	//  There are two conditions for chnaging from normal IMV to constant flow IMV,
	//  One is the abs(Pressure - actual pressure)<1 cmH2O , that is the only chnging
	//  condition up to now.
	//  The other is the time have lasted over 300ms after the exahalation phase coming.
	//		I think the second condition is necessary for the reason of that if the flow
	//  setting is high , physically IMV can not keep the PEEP , so the first condition
	//  (old condition ) will never be true.

	Temp32 = (int32_t)eP*SwitchCons;
	Temp32 = Temp32>>10;
	Temp16 = (int16_t)Temp32;
	Temp32 = (int32_t)Temp16*gs_a23;
	ASSERTION(gs_a24 != 0);
	Temp32 = (int32_t)Temp32/gs_a24;
	Temp16 = (int16_t)Temp32;
	uiflow = Temp16 + eF;

	Temp32 = gs_PressureMeasured*gs_a11;

	Temp32 += (int32_t)TotalFlow*gs_a12;
	Temp32 = (int32_t)Temp32/10;
	upex = (int16_t)Temp32;

	Temp32 = gs_PressureMeasured*SwitchCons;
	Temp32+= (int32_t)10* gs_TargetPEEPPressure*(gs_SwitchTime - SwitchCons);
	Temp32 = Temp32>>10;
	Temp16 = (int16_t)Temp32;
	Temp32 = (int32_t)Temp16*gs_a21;
	Temp32 += (int32_t)TotalFlow*gs_a22;
	Temp32 = (int32_t)Temp32/10;
	upflow = (int16_t)Temp32;

	Temp16 = CMVController_IntegratorEx(uiex,upex);
	gs_ExValvePos = Temp16;
	gs_ExhStep = CMVController_ExValveFilter(gs_UpExLmt - Temp16);

	Temp16  = CMVController_IntegratorAir(uiflow,upflow);
	gs_AirStep = CMVController_AirValveFilter(Temp16);

	Temp16 = CMVController_IntegratorO2(uiflow,upflow);
	gs_O2Step = CMVController_O2ValveFilter(Temp16);

//	MotorController_MoveToStepPositionOld(eExhMotor,gs_ExhStep);
//	MotorController_MoveToStepPositionOld(eAirMotor,gs_AirStep);
//	MotorController_MoveToStepPositionOld(eO2Motor,gs_O2Step);
	gs_DesiredMotorPosition.desiredAirPosition = gs_AirStep;
	gs_DesiredMotorPosition.desiredO2Position = gs_O2Step;
	gs_DesiredMotorPosition.desiredExhPosition = gs_ExhStep;
}
/**
 * O2 Valve filter
 * @param u
 * @return int16_t Output
 */
int16_t  CMVController_O2ValveFilter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntO2ValveFtr>>gs_Scalen);

	Error = u - Output;
	Temp32 = Error*2;
	Temp32 = Temp32<<gs_Scalen;
	gs_IntO2ValveFtr += (int32_t)Temp32*gs_ValveFtrConst/gs_iDT;
	return(Output);
}
/**
 * Integrator O2
 * @param ui
 * @param up
 * @return int16_t Output
 */
int16_t CMVController_IntegratorO2(int16_t ui,int16_t up)
{
	int32_t Temp32,Temp32B;
	int16_t Temp16 ,UpFlowLmt, Output;

	Temp32 = ui;
	Temp32 = Temp32<<gs_Scalen;
	Temp32 = (int32_t)Temp32*gs_a24/10;
	Temp32 = (int32_t)Temp32/gs_iDT;

	if(gs_IntO2Valve>=(int32_t)0x003fffff)
		gs_IntO2Valve = (int32_t)0x003fffff;
	else
		if(gs_IntO2Valve<=(int32_t)0xffc00000)
			gs_IntO2Valve = (int32_t)0xffc00000;

	Temp16 = (int16_t)(gs_IntO2Valve>>gs_Scalen);
	Temp16 += up;
	Temp32B = (int32_t)Temp16*gs_Bat/500;
	Temp16 = (int16_t)Temp32B;

	Temp16 = CMVController_GainNonCom(Temp16);

	if ( !gs_IsExhPhase )
	{
		UpFlowLmt = gs_InhUpFlowLmt;
	}
	else
	{
		UpFlowLmt = gs_ExhUpFlowLmt;
	}

	if(Temp16>=UpFlowLmt)
	{
		if(ui<0)
			gs_IntO2Valve += Temp32;
		Output = UpFlowLmt;
	}
	else if(Temp16<=gs_DownFlowLmt)
	{
		if(ui>0)
			gs_IntO2Valve += Temp32;
		Output = gs_DownFlowLmt;
	}
	else
	{
		gs_IntO2Valve += Temp32;
		Output = Temp16;
	}

	return(Output);
}
/**
 * Integrator O2 percent
 * @param u
 * @return int16_t Output
 */
int16_t CMVController_IntegratorO2Percent(int16_t u)
{
	int32_t Temp32;
	int16_t Temp16 ,Output;

	Temp32 = u;
	Temp32 = Temp32<<10;
	Temp32 = Temp32/100;
	Temp16 = (int16_t)(gs_IntO2Percent>>10);

	if(Temp16>=gs_UpO2PerLmt)
	{
		if(u<0)
			gs_IntO2Percent += Temp32;
		Output = gs_UpO2PerLmt;
	}
	else if(Temp16<=gs_DownO2PerLmt)
	{
		if(u>0)
			gs_IntO2Percent += Temp32;
		Output = gs_DownO2PerLmt;
	}
	else
	{
		gs_IntO2Percent += Temp32;
		Output = Temp16;
	}
	return(Output);
}
/**
 * Air valve filter
 * @param u
 * @return int16_t Output
 */
int16_t  CMVController_AirValveFilter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntAirValveFtr>>gs_Scalen);
	Error = u - Output;
	Temp32 = Error*2;
	Temp32 = Temp32<<gs_Scalen;
	gs_IntAirValveFtr += (int32_t)Temp32*gs_ValveFtrConst/gs_iDT;
	return(Output);
}
/**
 * Integrator Air
 * @param ui
 * @param up
 * @return int16_t Output
 */
int16_t CMVController_IntegratorAir(int16_t ui,int16_t up)
{
	int32_t Temp32,Temp32B;
	int16_t Temp16 , UpFlowLmt, Output;

	Temp32 = ui;
	Temp32 = Temp32<<gs_Scalen;
	Temp32 = (int32_t)Temp32*gs_a24/10;
	Temp32 = (int32_t)Temp32/gs_iDT;

	if(gs_IntAirValve>=(int32_t)0x003fffff){
		gs_IntAirValve = (int32_t)0x003fffff;
	}
	else if(gs_IntAirValve<=(int32_t)0xffc00000){
		gs_IntAirValve = (int32_t)0xffc00000;
	}


	Temp16 = (int16_t)(gs_IntAirValve>>gs_Scalen);
	Temp16 += up;
	Temp32B = (int32_t)Temp16*gs_Alf/500;
	Temp16 = (int16_t)Temp32B;
	Temp16 = CMVController_GainNonCom(Temp16);


	if ( !gs_IsExhPhase )
	{
		UpFlowLmt = gs_InhUpFlowLmt;
	}
	else
	{
		UpFlowLmt = gs_ExhUpFlowLmt;
	}

	if(Temp16>=UpFlowLmt)
	{
		if(ui<0)
			gs_IntAirValve += Temp32;
		Output = UpFlowLmt;
	}
	else if(Temp16<=gs_DownFlowLmt)
	{
		if(ui>0)
			gs_IntAirValve += Temp32;
		Output = gs_DownFlowLmt;
	}
	else
	{
		gs_IntAirValve += Temp32;
		Output = Temp16;
	}
	return((int16_t)Output);
}
/**
 * Gain
 * @param u
 * @return
 */
int16_t CMVController_GainNonCom(int16_t u)
{
	int32_t x;
	int16_t  y;
	x = (int32_t)gs_FlowGain*u;

	ASSERTION((u + gs_Nco) !=0 );
	// Avoid division by zero
	if ((u + gs_Nco) != 0)
	{
		x = (int32_t)x/(u + gs_Nco);
	}
	y = (int16_t)x;
	return(y);
}
/**
 * Exhalation valve filter
 * @param u
 * @return
 */
int16_t  CMVController_ExValveFilter(int16_t u)
{

	int32_t Temp32;
	int16_t Error,Output;
	if(abs(gs_ExhStep-(gs_IntExValveFtr>>gs_Scalen))>5)
	{
		Output = (int16_t)(gs_IntExValveFtr>>gs_Scalen);
	}
	else
	{
		Output=gs_ExhStep;
	}
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<gs_Scalen;
	gs_IntExValveFtr += (int32_t)Temp32*gs_ValveFtrExhConst/gs_iDT;
	return(Output);
}
/**
 * PL filter root
 * @param u
 * @return
 */
int16_t  CMVController_PLFilterRoot(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntPrxRootFtr>>gs_Scalen);
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<gs_Scalen;

	gs_IntPrxRootFtr += Temp32*250/gs_iDT;

	return(Output);
}
/**
 * Pressure root gain
 * @param u
 * @return
 */
int16_t CMVController_PressureRootGain(int16_t u)
{
	uint16_t Root16;
	int16_t Temp16;
	int32_t Temp32;

	Temp16 = CMVController_PLFilterRoot(gs_PressureMeasured);
	if(Temp16<10)
		Temp16 = 10;
	else
		if(Temp16>1000)
			Temp16 = 1000;
	if(Temp16<255)
	{
		Root16 = m_table[Temp16];
		Temp32 = (int32_t)Root16*1000;
		Temp32 = (int32_t)Temp32/3162; /* Temp32 = Root16/sqrt(10) */
		Temp32 = Temp32>>2;
		Temp16 = (int16_t)Temp32;
	}
	else
	{
		Temp16 = Temp16/10;
		Root16 = m_table[Temp16];
		Temp16 = Root16>>2;
	}

	Temp16 += 2048;		 				/* 2*1024 */
	Temp32 = (int32_t)u*gs_PressureGain;
	ASSERTION(Temp16 != 0);
	Temp32 = (int32_t)Temp32/Temp16;
	Temp16 = (int16_t)Temp32;
	return(Temp16);
}
/**
 * Integrator Exhalation valve
 * @param ui
 * @param up
 * @return int16_t Output
 */
int16_t CMVController_IntegratorEx(int16_t ui, int16_t up)
{
	int32_t Temp32;
	int16_t Temp16 ,Output;

	Temp32 = ui;
	Temp32 = Temp32<<gs_Scalen;
	Temp32 = -Temp32*gs_a13/10;
	Temp32 = Temp32/gs_iDT;
	Temp16 = (int16_t)(gs_IntExValve>>gs_Scalen);
	Temp16 += up;

	Temp16 = CMVController_PressureRootGain(Temp16);


	if(Temp16>=gs_UpExLmt)
	{
		if(ui<0)
		{
			gs_IntExValve += Temp32;
		}
		Output = gs_UpExLmt;
	}
	else if(Temp16<=gs_DownExLmt)
	{
		if(ui>0)
		{
			gs_IntExValve += Temp32;
		}
		Output = gs_DownExLmt;
	}
	else
	{
		gs_IntExValve += Temp32;
		Output = Temp16;
	}

	return(Output);
}
/**
 * Constant flow switch filter
 * @param u
 * @return int16_t Output
 */
int16_t CMVController_FlowConsFilter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntFConsFtr>>gs_Scalen);
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<gs_Scalen;
	gs_IntFConsFtr += Temp32*200/gs_iDT;

	return(Output);
}

/**
 * Set Inhalation target
 * @param target
 * @param riseTime
 */
void CMVController_SetInhalationTarget(uint16_t target,uint16_t riseTime)
{
	//	if(gs_ParameterType==eLargeCompliance)
	//	{
	//		CMVController_SetLargeComplianceParameter();
	//	}
	//	else
	//	{
	//		CMVController_SetSmallComplianceParameter();
	//	}
	gs_TargetInspPressure = target;
	gs_SetRiseTime = riseTime;
	gs_IsExhPhase = false;
	gs_IsNewPhase=true;
}
/**
 * Set Exhalation target
 * @param target
 */
void CMVController_SetExhalationTarget(uint16_t target)
{
	if(!gs_IsExhPhase && gs_SettlingTimeCnt*2>0 && gs_PeakPressure>50 && gs_RiseTime*2>50)
	{
		for(int i = FFT_EXAMINE_TIME;i>=0;i--)
		{
			gs_cntFFT--;
			if(gs_cntFFT <0)
			{
				gs_cntFFT = NFFT-1;
			}
			rin[i]=rinData[gs_cntFFT];
		}
		kiss_fftr_state = kiss_fftr_alloc(NFFT,0,0,0);
		kiss_fftr(kiss_fftr_state,rin,sout);
		free(kiss_fftr_state);
		uint8_t maxFreq=0;
		float maxAmp=0;
		for(int i=0;i<NFFT;i++)
		{
			if(i<=((NFFT/2)+1) && i>=2)
			{
				float currAmp= (float)sqrt(sout[i].r*sout[i].r+sout[i].i*sout[i].i);
				if(currAmp>maxAmp)
				{
					maxAmp=currAmp;
					maxFreq=i;
				}
			}
			rinData[i]=0;
			rin[i]=0;
		}
		maxAmp/=NFFT;
		if(gs_ParameterType==eLargeCompliance)
		{
			DEBUG_CMV("Using Large Compliance ");
		}
		else
		{
			DEBUG_CMV("Using Small Compliance ");
		}
		DEBUG_CMV("Max Amp: %f at Freq: %d, Settling Time: %d, PeakPressure: %d, Rise Time: %d\n",maxAmp,maxFreq,gs_SettlingTimeCnt*2,gs_PeakPressure,gs_RiseTime*2);
		if(/*gs_SettlingTimeCnt*2>900  ||*/ gs_RiseTime*2>400)
		{
			gs_ParameterType=eLargeCompliance;
			DEBUG_CMV("Switch to Large Compliance\n");
		}
		if(maxAmp>=OSCILLATION_RANGE && maxFreq>3)
		{
			gs_ParameterType=eSmallCompliance;
			DEBUG_CMV("Switch to Small Compliance\n");
		}
		gs_SettlingTimeCnt=0;
		gs_PeakPressure=0;
		gs_RiseTime=0;
		gs_cntFFT=0;
	}
	else
	{
		if(gs_IsNewPhase==true)
			gs_ParameterType=eSmallCompliance;
	}
	gs_IsNewPhase=false;
	gs_TargetPEEPPressure = target;
	gs_IsExhPhase = true;
}
/**
 * Pre filter
 */
void CMVController_PreFilter()
{
	int16_t Temp16;
	double C = devIf_GetSetting()->pcv.compRate/(double)SCALE;
	if(!gs_IsExhPhase)
	{
		Temp16 = (int16_t) CMVController_FirstPreFilter(gs_TargetInspPressure
				+CMVController_CompCompensatePressure(C, true)*10.0);//for new tube compensate
	}
	else
	{
		Temp16 = (int16_t) CMVController_FirstPreFilter(gs_TargetPEEPPressure
				+CMVController_CompCompensatePressure(C, true)*10.0);//for new tube compensate
	}
	gs_PSet = Temp16;
}
/**
 * Compute compensate pressure
 * @param compensateRate
 * @param type
 * @return float
 */
float CMVController_CompCompensatePressure(double compensateRate,bool type)
{
	double L = devIf_GetSetting()->pcv.tubeLength/(double)SCALE;//[mm]
	double D = devIf_GetSetting()->pcv.tubeDiameter/(double)SCALE;//[mm]
	double C = compensateRate;
	double P_Compensate = 0;
	double Q =0;
	ProxyStatus Status =gs_RS485Status;
	if((Status==eCalibrated)||(Status==eResetAndRecovered))
	{
		Q = ((double)gs_RS485ProxySensor*(double)devIf_GetSetting()->btpsFactor.exhFactor/(double)SCALE)/100.0; // LPM

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
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 4){
			if(Q >= 0)
				P_Compensate = (-0.00000003*Q*Q*Q + 0.00008*Q*Q + 0.0002*Q)*L + (0.00002*Q*Q*Q + 0.0172*Q*Q + 0.0718*Q);
			else
				P_Compensate = (-0.00000003*Q*Q*Q - 0.00008*Q*Q + 0.0002*Q)*L + (0.00002*Q*Q*Q - 0.0172*Q*Q + 0.0718*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 4.5){
			if(Q >= 0)
				P_Compensate = (0.00000001*Q*Q*Q + 0.00004*Q*Q + 0.00003*Q)*L + (-0.000003*Q*Q*Q + 0.0122*Q*Q + 0.0755*Q);
			else
				P_Compensate = (0.00000001*Q*Q*Q - 0.00004*Q*Q + 0.00003*Q)*L + (-0.000003*Q*Q*Q - 0.0122*Q*Q + 0.0755*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 5){
			if(Q >= 0)
				P_Compensate = (0.00000008*Q*Q*Q + 0.00002*Q*Q + 0.0001*Q)*L + (-0.00002*Q*Q*Q + 0.011*Q*Q + 0.0539*Q);
			else
				P_Compensate = (0.00000008*Q*Q*Q - 0.00002*Q*Q + 0.0001*Q)*L + (-0.00002*Q*Q*Q - 0.011*Q*Q + 0.0539*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 5.5){
			if(Q >= 0)
				P_Compensate = (0.000000005*Q*Q*Q + 0.00001*Q*Q + 0.00007*Q)*L + (-0.00002*Q*Q*Q + 0.0092*Q*Q + 0.0496*Q);
			else
				P_Compensate = (0.000000005*Q*Q*Q - 0.00001*Q*Q + 0.00007*Q)*L + (-0.00002*Q*Q*Q - 0.0092*Q*Q + 0.0496*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 6){
			if(Q >= 0)
				P_Compensate = (0.00000004*Q*Q*Q + 0.000008*Q*Q + 0.00006*Q)*L + (-0.00002*Q*Q*Q + 0.0088*Q*Q + 0.0481*Q);
			else
				P_Compensate = (0.00000004*Q*Q*Q - 0.000008*Q*Q + 0.00006*Q)*L + (-0.00002*Q*Q*Q - 0.0088*Q*Q + 0.0481*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 6.5){
			if(Q >= 0)
				P_Compensate = (0.00000002*Q*Q*Q + 0.000006*Q*Q + 0.00005*Q)*L + (-0.00002*Q*Q*Q + 0.0081*Q*Q + 0.0437*Q);
			else
				P_Compensate = (0.00000002*Q*Q*Q - 0.000006*Q*Q + 0.00005*Q)*L + (-0.00002*Q*Q*Q - 0.0081*Q*Q + 0.0437*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 7){
			if(Q >= 0)
				P_Compensate = (-0.00000002*Q*Q*Q + 0.000006*Q*Q + 0.00002*Q)*L + (-0.00002*Q*Q*Q + 0.0076*Q*Q + 0.0442*Q);
			else
				P_Compensate = (-0.00000002*Q*Q*Q - 0.000006*Q*Q + 0.00002*Q)*L + (-0.00002*Q*Q*Q - 0.0076*Q*Q + 0.0442*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 7.5){
			if(Q >= 0)
				P_Compensate = (-0.00000004*Q*Q*Q + 0.000004*Q*Q + 0.00001*Q)*L + (-0.00002*Q*Q*Q + 0.0073*Q*Q + 0.0455*Q);
			else
				P_Compensate = (-0.00000004*Q*Q*Q - 0.000004*Q*Q + 0.00001*Q)*L + (-0.00002*Q*Q*Q - 0.0073*Q*Q + 0.0455*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}
		else if(D == 8){
			if(Q >= 0)
				P_Compensate = (0.000000005*Q*Q*Q + 0.000001*Q*Q + 0.00004*Q)*L + (-0.00002*Q*Q*Q + 0.0076*Q*Q + 0.0377*Q);
			else
				P_Compensate = (0.000000005*Q*Q*Q - 0.000001*Q*Q + 0.00004*Q)*L + (-0.00002*Q*Q*Q - 0.0076*Q*Q + 0.0377*Q);

			if(type == true){
				//				P_Compensate = P_Compensate;
			}
		}

	}


	return P_Compensate*C/100.0 ;
}
/**
 * First pre filter
 * @param u
 * @return float gs_IntPreFtr1
 */
float CMVController_FirstPreFilter(float u)
{
	double Gain;
	float Error;

	Error = u - gs_IntPreFtr1;

	if( !gs_IsExhPhase )
	{
		Gain = Error*330.0/(float)gs_iDT;
		ASSERTION(gs_SetRiseTime != 0);
		gs_IntPreFtr1 += Gain/(float)gs_SetRiseTime;
	}
	else
		gs_IntPreFtr1 += Error*50.0/(float)gs_iDT;
	return(gs_IntPreFtr1);
}

