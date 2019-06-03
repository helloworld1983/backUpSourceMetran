/*
 * HFOController.c
 *
 *  Created on: 3 thg 1, 2019
 *      Author: haxua
 */
#include <deviceTask/DeviceInterface.h>
#include <ipcTask/IpcInterface.h>
#include "HFOController.h"
#define  SCALE 100
#define MAP_SERVO_SCALE 8
#define EXH_CLOSE_STEP_POS 2050
#define EXH_OPEN_STEP_POS 0
#define MAX_STEP  2200
#define FLOW_SCALER 3000
#define PDF_SCALE_FACTOR  3 //50
#define PDF_ERROR_GAIN  45 //40;//45
#define MAP_SERVO_RATE  350
#define MAX_HFO_FREQ    17
#define MIN_HFO_FREQ    5
#define PRESS_SCALER  100
#define HFO_BASE_FLOW_LIMIT 13
#define iDT 500
#define HOURS_200 200*60*60*1000 //200 hours in ms
#define UNINITIALIZED_LONG_VALUE 0x80000001

static int16_t Num[] = {   0,   0,   0,   0,   0,  71, 213, 258, 284, 329,
		71, 542, 426, 445, 497, 639, 568, 743, 639, 871,
		142, 755, 781,1072, 852,  71, 923, 968, 994,1387,
		1491,1522,1136,1626,1207,1491,1278, 471,1349,1607,
		284, 897,1491,1252,1562,1917,2007,2317,1704,2447,
		71,2461};

static int16_t Den[] = {   0,   0,   0,   0,   0,1130,2825,2933,2825,2909,
		565,3921,2825,2724,2825,3390,2825,3478,2825,3648,
		565,2861,2825,3709,2825, 226,2825,2853,2825,3806,
		3955,3907,2825,3921,2825,3390,2825,1013,2825,3279,
		565,1741,2825,2317,2825,3390,3472,3923,2825,3974,
		113,3840};
static bool gs_IsEnabled;
static E_SIType gs_SIType;
static int32_t gs_IntAirFFtr;
static int32_t gs_IntO2FFtr;
static bool gs_ConstFlow;
static int32_t gs_Clock200hours;
static int32_t gs_ExhValveLimit=EXH_CLOSE_STEP_POS;
static uint16_t gs_PressFactor;
static int32_t gs_ExhPress ;
static int32_t gs_AirFlowSensorVal;
static int32_t gs_O2FlowSensorVal;
static int16_t gs_AirValve;
static int16_t gs_O2Valve;
static int16_t gs_ExValve;
static int32_t gs_FreqSetting, gs_CurrentFreq;
static int32_t gs_MapSetting;
static int32_t gs_SISetting;
static int32_t gs_FreshGasSetting;
static int32_t gs_DesiredFlow;
static uint16_t gs_InactiveTimer;
static int32_t gs_PreFilterPress,gs_TotalFlowCmd,gs_PressureError ;
static bool gs_SIRequest,gs_HIMP;
static int32_t gs_ExhFirstIntegrator1;
static int32_t gs_ExhSecondIntegrator1;
static int32_t gs_ExhError1;
static int32_t gs_ExhFirstIntegrator2;
static int32_t gs_ExhSecondIntegrator2;
static int32_t gs_ExhError2;
static int32_t gs_ExhFirstIntegrator3;
static int32_t gs_ExhSecondIntegrator3;
static int32_t gs_ExhError3;
static int32_t gs_ExhAmpFirstIntegrator;
static int32_t gs_FirstIntegrator1;
static int32_t gs_SecondIntegrator1;
static int32_t gs_Error1;
static int32_t gs_FirstIntegrator2;
static int32_t gs_SecondIntegrator2;
static int32_t gs_Error2;
static int32_t gs_FirstIntegrator3;
static int32_t gs_SecondIntegrator3;
static int32_t gs_Error3;
static int32_t gs_AmpFirstIntegrator;
static int32_t gs_O2PDFFirstIntegrator;
static int32_t gs_O2PDFSecondIntegrator;
static int32_t gs_AirPDFFirstIntegrator;
static int32_t gs_AirPDFSecondIntegrator;
static int32_t gs_PreFilterIntegrator;
static int32_t gs_LeadCircuitIntegrator;
static int32_t gs_NtchFtrInputIntegrator;
static int32_t gs_AfterNtchFtrIntegrator;
static int32_t gs_FilterPressIntegrator;
static int32_t gs_PressCtrlIntegrator;
static int32_t gs_FilterFlowIntegratorx;
static int32_t gs_PreProcessPress;
static int32_t gs_FilterPressIntegratorx;
static int32_t gs_SIFlowCtrlIntegrator;
static int32_t gs_SIFilterFlowIntegrator;
static int32_t gs_PressCtrl;
static int32_t gs_LPFilterIntegrator;
static int32_t gs_NFltrfirstIntegrator;
static int32_t gs_NFltrsecondIntegrator;
static int32_t gs_PreviousError;
static int32_t gs_ExhPreProcessPress;
static int32_t gs_CurrentMeasured;
static desiredMotorPositionStruct gs_DesiredMotorPosition;
/**
 * Set SIRequest flag and HIMP_Factor based on blower voltage and
 * difference between SI setting and MAP setting.
 * @param SIRequest
 */
void HFOController_SetSIRequest(bool SIRequest)
{
	gs_SIRequest = SIRequest;
	if(gs_SIRequest)
	{
		if(gs_SIType == eAutoSIType)
		{
			Command sendMsg;
			sendMsg.id = eDisableTrigger;
			sendMsg.data.triggerId = eAutoSIOffTimeTrigger;
			ipcIf_sendCommandMsg(&sendMsg);

			sendMsg.id = eEnableTrigger;
			sendMsg.data.triggerId = eAutoSIOnTimeTrigger;
			ipcIf_sendCommandMsg(&sendMsg);
		}
		else
		{
			Command sendMsg;
			sendMsg.id = eDisableTrigger;
			sendMsg.data.triggerId = eAutoSIOnTimeTrigger;
			ipcIf_sendCommandMsg(&sendMsg);

			sendMsg.id = eDisableTrigger;
			sendMsg.data.triggerId = eAutoSIOffTimeTrigger;
			ipcIf_sendCommandMsg(&sendMsg);

			sendMsg.id = eEnableTrigger;
			sendMsg.data.triggerId = eSI10sTimeTrigger;
			ipcIf_sendCommandMsg(&sendMsg);
		}
	}
}
/**
 * Get filter pressure integrator
 * @return int32_t gs_FilterPressIntegrator
 */
int32_t HFOController_GetFilterPressIntegrator()
{
	return gs_FilterPressIntegrator>>8;
}
/**
 * Get Exhalation map
 * @return int32_t gs_ExhPreProcessPress
 */
int32_t HFOController_GetExhMap()
{
	return gs_ExhPreProcessPress;
}
/**
 * Get HIMP
 * @return bool gs_HIMP
 */
bool HFOController_GetHIMP()
{
	return gs_HIMP;
}
/**
 * Get SI Request
 * @return bool gs_SIRequest
 */
bool HFOController_GetSIRequet()
{
	return gs_SIRequest;
}
/**
 * Get current frequency
 * @return int32_t gs_CurrentFreq
 */
int32_t HFOController_GetCurrentFreq()
{
	return gs_CurrentFreq;
}
/**
 * Set SI Type
 * @param SI
 */
void HFOController_SetSIType(E_SIType SI)
{
	gs_SIType = SI;
}
/**
 * Initialize all static variables
 */
void HFOController_ResetHFOStaticVariables()
{
	gs_SIRequest = false;
	gs_IntAirFFtr = 0;
	gs_IntO2FFtr = 0;

	gs_FirstIntegrator1=0;
	gs_SecondIntegrator1=0;
	gs_Error1=0;

	gs_FirstIntegrator2=0;
	gs_SecondIntegrator2=0;
	gs_Error2=0;

	gs_FirstIntegrator3=0;
	gs_SecondIntegrator3=0;
	gs_Error3=0;

	gs_AmpFirstIntegrator=0;

	gs_O2PDFFirstIntegrator=150000000/2;
	gs_O2PDFSecondIntegrator=0;

	gs_AirPDFFirstIntegrator=127995584;
	gs_AirPDFSecondIntegrator=0;

	gs_PreFilterIntegrator=0;

	gs_LeadCircuitIntegrator=0;
	gs_NtchFtrInputIntegrator=0;
	gs_AfterNtchFtrIntegrator=0;
	gs_FilterPressIntegrator=0;

	gs_PressCtrlIntegrator=0;

	gs_FilterFlowIntegratorx=0;

	gs_SIFlowCtrlIntegrator = 0;
	gs_SIFilterFlowIntegrator = 0;
	gs_PressCtrl = 0;

	gs_PreProcessPress=0;
	gs_FilterPressIntegratorx=0;

	gs_LPFilterIntegrator = 0;
	gs_NFltrfirstIntegrator = 0;
	gs_NFltrsecondIntegrator = 0;
	gs_PreviousError = 0;
	gs_ExhPreProcessPress = 0;
}
/**
 * Initialize some static variables
 */
void HFOController_ResetSomeStaticVariables()
{
	gs_FilterPressIntegrator=0;
	gs_ExhPreProcessPress = 0;
	gs_PressCtrlIntegrator=0;
	gs_PressCtrl = 0;
}
/**
 * Set HFO alarm inactive timer
 * @param value
 */
void HFOController_SetHFOAlarmInactiveTimer(uint16_t value)
{
	gs_InactiveTimer = value;
}
/**
 * Get HFO alarm inactive timer
 * @return uint16_t gs_InactiveTimer
 */
uint16_t HFOController_GetHFOAlarmInactiveTimer()
{
	return gs_InactiveTimer;
}
/**
 * Set constant flow
 * @param constFlowFlag
 */
void HFOController_SetConstFlow(bool constFlowFlag)
{
	gs_ConstFlow = constFlowFlag;
}
/**
 * This is the main processing loop for the HFO Controllers.
 * The routine obtains the current sensor reading, calculates the step
 * motor position, and commands the step motor.
 * @param monitor
 * @return desiredMotorPositionStruct gs_DesiredMotorPosition Desired motor position
 */
desiredMotorPositionStruct HFOController_Run(RealTimeM4ToA53* monitor)
{
	gs_DesiredMotorPosition.desiredAirPosition=-1;
	gs_DesiredMotorPosition.desiredExhPosition=-1;
	gs_DesiredMotorPosition.desiredHFOPosition=-1;
	gs_DesiredMotorPosition.desiredO2Position=-1;
	if(gs_IsEnabled == false)
	{
		return gs_DesiredMotorPosition;
	}

	HFOController_CalculateHFOOperationTime();
	gs_HIMP = gs_SIRequest;


	E_VentilationMode currentMode = devIf_GetSetting()->currentMode;
	if(currentMode == eHFOExtremeLowMAPMode)
	{
		gs_MapSetting = 10*SCALE;
		gs_FreshGasSetting = 8;
		gs_ConstFlow = true;
	}
	else
	{
		gs_MapSetting = devIf_GetSetting()->hfo.map;
		gs_FreshGasSetting = devIf_GetSetting()->hfo.freshGas/100;
		gs_ConstFlow = false;
	}

	gs_SISetting = devIf_GetSetting()->hfo.si;
	gs_FreqSetting = devIf_GetSetting()->hfo.frequency/100;


	HFOController_RampFreq(gs_FreqSetting);
	//  Reading the proxi_pressure. Note that this pressure reading is
	// 	already scaled up SCALE.

	if(devIf_GetSetting()->azTestState != eAzActive )
	{
		gs_CurrentMeasured = monitor->inhPress.lastReading;
	}
	else
	{
		gs_CurrentMeasured =   monitor->exhPress.lastReading;
	}

	//  Reading the flow sensor.
	gs_AirFlowSensorVal = (monitor->airFlow.lastReading*(devIf_GetSetting()->btpsFactor.airFactor))/SCALE;
	gs_O2FlowSensorVal =  (monitor->o2Flow.lastReading*(devIf_GetSetting()->btpsFactor.o2Factor))/SCALE;

	gs_AirFlowSensorVal = HFOController_AirFilter(gs_AirFlowSensorVal);
	gs_O2FlowSensorVal = HFOController_O2Filter(gs_O2FlowSensorVal);

	gs_ExhPress = monitor->exhPress.lastReading;

	{
		// Establish exh MAP pressure
		HFOController_NotchFilter();
	}

	int32_t O2ValCmd, AirValCmd;

	// Filter and process pressure.
	HFOController_PreFilterBlock();
	HFOController_PreProcessPresssureBlock();

	// Control pressure using exhalation valve.
	HFOController_HFOPressControllerByExhValve();
	gs_DesiredFlow = gs_FreshGasSetting;

	// Do flow compensator.
	HFOController_FlowCompensatorBlock();

	//Reading the FiO2 setting.
	int32_t DesiredAirRatio = devIf_GetSetting()->ratio.DesiredO2Ratio;

	AirValCmd = ( gs_TotalFlowCmd * DesiredAirRatio ) >> 7;

	O2ValCmd = gs_TotalFlowCmd - AirValCmd;
	gs_AirValve = HFOController_MAPAirPDFControl(AirValCmd, gs_AirFlowSensorVal);
	gs_O2Valve = HFOController_MAPO2PDFControl(O2ValCmd, gs_O2FlowSensorVal);

//	MotorController_MoveToStepPositionOld(eAirMotor,gs_AirValve);
//	MotorController_MoveToStepPositionOld(eO2Motor,gs_O2Valve);
//	MotorController_MoveToStepPositionOld(eExhMotor,gs_ExValve);
	gs_DesiredMotorPosition.desiredAirPosition = gs_AirValve;
	gs_DesiredMotorPosition.desiredO2Position = gs_O2Valve;
	gs_DesiredMotorPosition.desiredExhPosition = gs_ExValve;
	return gs_DesiredMotorPosition;
}
/**
 * Check whether HFO controller is enable
 * @return bool gs_IsEnabled
 */
bool HFOController_IsEnable()
{
	return gs_IsEnabled;
}
/**
 * Enable HFO controller
 */
void HFOController_Enable()
{
	gs_IsEnabled = true;
	HFOController_GetHFOOper200Hours();
}
/**
 * Disable HFO controller
 */
void HFOController_Diable()
{
	gs_IsEnabled = false;
}
/**
 * Control HFO pressure by Exhalation valve
 */
void HFOController_HFOPressControllerByExhValve()
{
	int32_t  ScaleUpInput;
	int32_t  Temp,Error,ScaleDownIntegrator,ExStepUpLimit,ExStepLowLimit;
	int32_t  PreExStep,FilteredExStep;

	ScaleDownIntegrator = (int32_t) (gs_PressCtrlIntegrator >> MAP_SERVO_SCALE);
	if(gs_HIMP)
	{
		gs_PressureError = gs_PreFilterPress - gs_CurrentMeasured;
	}
	else
	{
		gs_PressureError = gs_PreFilterPress - gs_PreProcessPress;
	}

	PreExStep = (ScaleDownIntegrator - gs_PreProcessPress)*64; //For both MAP SI servo, 0.45*1024//256


	PreExStep = PreExStep >> 11;

	if ( gs_HIMP )
	{
		gs_PressFactor = 18; // speed parameter
		Error = gs_PressureError * gs_PressFactor;
		Error = Error>>1;
	}
	else
	{
		gs_PressFactor = 10;//10
		Error = gs_PressureError * gs_PressFactor; //Press_Factor=2|3
	}

	ScaleUpInput = Error << MAP_SERVO_SCALE; 		// scale up before integration.

	FilteredExStep = (int32_t)(gs_FilterPressIntegratorx >> MAP_SERVO_SCALE);  // scale down before feedback.

	Error =  PreExStep - FilteredExStep;

	Temp = Error << MAP_SERVO_SCALE;            // scale up before go into integrator.

	gs_FilterPressIntegratorx += Temp / 1;

	ExStepLowLimit = EXH_OPEN_STEP_POS;

	ExStepUpLimit = gs_ExhValveLimit;

	if (FilteredExStep >= ExStepUpLimit )
	{
		if ( ScaleUpInput <= 0)
		{
			gs_PressCtrlIntegrator += ScaleUpInput / 1000;
		}
		PreExStep = ExStepUpLimit;
	}
	else if ( FilteredExStep <= ExStepLowLimit)
	{
		if ( ScaleUpInput >= 0)
		{
			gs_PressCtrlIntegrator += ScaleUpInput /1000;
		}
		PreExStep = ExStepLowLimit;
	}
	else
	{
		gs_PressCtrlIntegrator += ScaleUpInput /1000;
	}

	gs_ExValve = PreExStep;
	// Ending of map servo by exahalation valve
}
/**
 * Get HFO Operation time
 */
void HFOController_GetHFOOper200Hours()
{
	gs_Clock200hours = 	devIf_GetSetting()->clock200hours;
	if (gs_Clock200hours == UNINITIALIZED_LONG_VALUE){
		gs_Clock200hours = 0;
	}
}
/**
 * Set limit of Exhalation valve
 * @param limitStep
 */
void HFOController_SetExhValveLimit(int32_t limitStep)
{
	gs_ExhValveLimit = limitStep;
}
/**
 * This block is dedicated to establish
 * exh MAP pressure quickly and provide the
 * constant flow setting .
 */
void HFOController_NotchFilter(void)
{
	if(!gs_SIRequest)
	{
		int32_t ScaleUpInput,Temp32, CosFeedback, SinFeedback, Error;

		int32_t ExhPressReading = gs_ExhPress;

		Temp32 = gs_PreviousError; // previous error.

		CosFeedback = ( gs_NFltrfirstIntegrator >> MAP_SERVO_SCALE );

		SinFeedback = ( gs_NFltrsecondIntegrator >> MAP_SERVO_SCALE);

		gs_PreviousError = ExhPressReading - CosFeedback - SinFeedback;

		ScaleUpInput = gs_PreviousError << MAP_SERVO_SCALE;

		gs_NFltrsecondIntegrator += ((gs_NFltrfirstIntegrator * Num[gs_CurrentFreq])/Den[gs_CurrentFreq]);
		gs_NFltrfirstIntegrator += ((ScaleUpInput * Num[gs_CurrentFreq])/Den[gs_CurrentFreq]);
		Temp32 += SinFeedback;
		gs_ExhPreProcessPress = (int32_t) (gs_LPFilterIntegrator >> MAP_SERVO_SCALE);
		Error = Temp32 - gs_ExhPreProcessPress;
		ScaleUpInput = Error << MAP_SERVO_SCALE;
		gs_LPFilterIntegrator += ScaleUpInput / 25;
	}
}
/**
 * Air Flow control loop
 * @param flowCmd
 * @param airFlowVal
 * @return int ValStep step position
 */
int32_t HFOController_MAPAirPDFControl( int32_t flowCmd, int32_t airFlowVal)
{
	int32_t Error, ErrorGain, ScaleDownIntegrator, PreFilterOutput, ValStep;

	int32_t ScaleUpInput;

	Error = flowCmd - airFlowVal;

	ErrorGain = Error * PDF_ERROR_GAIN;
	ScaleUpInput = ErrorGain / MAP_SERVO_RATE ;
	ScaleUpInput = (int32_t) ( ScaleUpInput << MAP_SERVO_SCALE );


	ScaleDownIntegrator = (int32_t) (gs_AirPDFFirstIntegrator >> MAP_SERVO_SCALE);

	PreFilterOutput = ScaleDownIntegrator * PDF_SCALE_FACTOR;

	PreFilterOutput /= (int16_t)FLOW_SCALER;

	if ( PreFilterOutput >= MAX_STEP)
	{
		if ( ErrorGain <= 0 )
		{
			gs_AirPDFFirstIntegrator += ScaleUpInput;
		}
		PreFilterOutput = MAX_STEP;
	}
	else if ( PreFilterOutput <= 0 )
	{
		if ( ErrorGain >= 0 )
		{
			gs_AirPDFFirstIntegrator += ScaleUpInput;
		}
		PreFilterOutput = 0;
	}
	else
	{
		gs_AirPDFFirstIntegrator += ScaleUpInput;
	}

	ValStep = PreFilterOutput;

	return ValStep;
}
/**
 * O2 flow control loop
 * @param flowCmd
 * @param o2FlowVal
 * @return int32_t ValStep Step position
 */
int32_t HFOController_MAPO2PDFControl ( int32_t flowCmd, int32_t o2FlowVal)
{
	int32_t Error, ErrorGain, ScaleDownIntegrator, PreFilterOutput, ValStep;
	int32_t ScaleUpInput;

	Error = flowCmd - o2FlowVal;

	ErrorGain = Error * PDF_ERROR_GAIN;
	ScaleUpInput =  ErrorGain / MAP_SERVO_RATE ;
	ScaleUpInput = (int32_t) (ScaleUpInput << MAP_SERVO_SCALE );


	ScaleDownIntegrator = (int32_t) (gs_O2PDFFirstIntegrator >> MAP_SERVO_SCALE);

	PreFilterOutput = ScaleDownIntegrator * PDF_SCALE_FACTOR;

	PreFilterOutput /= (int16_t)FLOW_SCALER;

	if ( PreFilterOutput >= MAX_STEP)
	{
		if ( ErrorGain <= 0 )
		{
			gs_O2PDFFirstIntegrator += ScaleUpInput ;
		}
		PreFilterOutput = MAX_STEP;
	}
	else if ( PreFilterOutput <= 0 )
	{
		if ( ErrorGain >= 0 )
		{
			gs_O2PDFFirstIntegrator += ScaleUpInput ;
		}
		PreFilterOutput = 0;
	}
	else
	{
		gs_O2PDFFirstIntegrator += ScaleUpInput ;
	}

	ValStep = PreFilterOutput;

	return ValStep;
}
/**
 * Filter out the HFO frquency signal also
 * its 2 and 3 times frequency signals.
 * @param notchFilterInput
 * @param frequency
 * @return int32_t Output
 */
int32_t HFOController_AmplitudeMinimizer(int32_t notchFilterInput, int32_t frequency)
{
	int32_t Temp32, CosFeedback, SinFeedback, Output;
	int16_t Freq;

	if (gs_HIMP)
		frequency=10;

	if(frequency < MIN_HFO_FREQ)
		frequency = MIN_HFO_FREQ;
	else if(frequency > MAX_HFO_FREQ)
		frequency = MAX_HFO_FREQ;

	notchFilterInput = notchFilterInput>>2;

	Freq = frequency;
	Output = gs_Error1;
	CosFeedback = (int32_t)( gs_FirstIntegrator1 >> MAP_SERVO_SCALE );
	SinFeedback = (int32_t)( gs_SecondIntegrator1 >> MAP_SERVO_SCALE);
	gs_Error1 = notchFilterInput - CosFeedback - SinFeedback;
	Temp32 = gs_Error1;
	Temp32 <<= MAP_SERVO_SCALE;
	gs_SecondIntegrator1 += (gs_FirstIntegrator1 * Num[Freq]/Den[Freq]);
	gs_FirstIntegrator1 += (Temp32 * Num[Freq]/Den[Freq]);
	notchFilterInput = Output + SinFeedback;


	Freq = frequency + frequency;
	Output = gs_Error2;
	CosFeedback = (int32_t)( gs_FirstIntegrator2 >> MAP_SERVO_SCALE );
	SinFeedback = (int32_t)( gs_SecondIntegrator2 >> MAP_SERVO_SCALE);
	gs_Error2 = notchFilterInput - CosFeedback - SinFeedback;
	Temp32 = gs_Error2;
	Temp32 <<= MAP_SERVO_SCALE;
	gs_SecondIntegrator2 += (gs_FirstIntegrator2 * Num[Freq]/Den[Freq]);
	gs_FirstIntegrator2 += (Temp32 * Num[Freq]/Den[Freq]);
	notchFilterInput = Output + SinFeedback;


	Freq = frequency + frequency + frequency;
	Output = gs_Error3;
	CosFeedback = (int32_t)( gs_FirstIntegrator3 >> MAP_SERVO_SCALE );
	SinFeedback = (int32_t)( gs_SecondIntegrator3 >> MAP_SERVO_SCALE);
	gs_Error3 = notchFilterInput - CosFeedback - SinFeedback;
	Temp32 = gs_Error3;
	Temp32 <<= MAP_SERVO_SCALE;
	gs_SecondIntegrator3 += (gs_FirstIntegrator3 * Num[Freq]/Den[Freq]);
	gs_FirstIntegrator3 += (Temp32 * Num[Freq]/Den[Freq]);
	Temp32 = Output + SinFeedback;


	Output = (gs_AmpFirstIntegrator >> MAP_SERVO_SCALE);
	Temp32 =  Temp32 - Output;
	Temp32 = Temp32 << MAP_SERVO_SCALE;

	Temp32 *= 80;
	gs_AmpFirstIntegrator += Temp32 / MAP_SERVO_RATE;

	Output = Output<<2;
	return(Output);
}
/**
 *  Filter out the HFO frquency signal also
 *  its 2 and 3 times frequency signals.
 * @param notchFilterInput
 * @param frequency
 * @return int32_t Output
 */
int32_t HFOController_ExhAmplitudeMinimizer(int32_t notchFilterInput, int32_t frequency)
{
	int32_t Temp32, CosFeedback, SinFeedback, Output;
	int16_t Freq;

	notchFilterInput = notchFilterInput>>2;

	if ( frequency < MIN_HFO_FREQ )
		frequency = MIN_HFO_FREQ;
	else if ( frequency > MAX_HFO_FREQ )
		frequency = MAX_HFO_FREQ;

	Freq = frequency;

	Output = gs_ExhError1;
	CosFeedback = (int32_t)( gs_ExhFirstIntegrator1 >> MAP_SERVO_SCALE );
	SinFeedback = (int32_t)( gs_ExhSecondIntegrator1 >> MAP_SERVO_SCALE);
	gs_ExhError1 = notchFilterInput - CosFeedback - SinFeedback;
	Temp32 = gs_ExhError1;
	Temp32 <<= MAP_SERVO_SCALE;
	gs_ExhSecondIntegrator1 += (gs_ExhFirstIntegrator1 * Num[Freq]/Den[Freq]);
	gs_ExhFirstIntegrator1 += (Temp32 * Num[Freq]/Den[Freq]);
	notchFilterInput = Output + SinFeedback;


	Freq = frequency + frequency;
	Output = gs_ExhError2;
	CosFeedback = (int32_t)( gs_ExhFirstIntegrator2 >> MAP_SERVO_SCALE );
	SinFeedback = (int32_t)( gs_ExhSecondIntegrator2 >> MAP_SERVO_SCALE);
	gs_ExhError2 = notchFilterInput - CosFeedback - SinFeedback;
	Temp32 = gs_ExhError2;
	Temp32 <<= MAP_SERVO_SCALE;
	gs_ExhSecondIntegrator2 += (gs_ExhFirstIntegrator2 * Num[Freq]/Den[Freq]);
	gs_ExhFirstIntegrator2 += (Temp32 * Num[Freq]/Den[Freq]);
	notchFilterInput = Output + SinFeedback;


	Freq = frequency + frequency + frequency;
	Output = gs_ExhError3;
	CosFeedback = (int32_t)( gs_ExhFirstIntegrator3 >> MAP_SERVO_SCALE );
	SinFeedback = (int32_t)( gs_ExhSecondIntegrator3 >> MAP_SERVO_SCALE);
	gs_ExhError3 = notchFilterInput - CosFeedback - SinFeedback;
	Temp32 = gs_ExhError3;
	Temp32 <<= MAP_SERVO_SCALE;
	gs_ExhSecondIntegrator3 += (gs_ExhFirstIntegrator3 * Num[Freq]/Den[Freq]);
	gs_ExhFirstIntegrator3 += (Temp32 * Num[Freq]/Den[Freq]);
	Temp32 = Output + SinFeedback;


	Output = (gs_ExhAmpFirstIntegrator >> MAP_SERVO_SCALE);
	Temp32 =  Temp32 - Output;
	Temp32 = Temp32 << MAP_SERVO_SCALE;

	Temp32 *= 100;
	gs_ExhAmpFirstIntegrator += Temp32 / MAP_SERVO_RATE;

	Output = Output<<2;
	return(Output);
}
/**
 * Pre filter block
 */
void HFOController_PreFilterBlock()
{
	int32_t  ScaleUpInput,Error;
	int32_t PressSetting;

	//  Get information reading for servo.
	if (gs_HIMP)
	{
		PressSetting = gs_SISetting;
	}
	else
	{
		PressSetting = gs_MapSetting;
	}

	gs_PreFilterPress = (int32_t)(gs_PreFilterIntegrator >> MAP_SERVO_SCALE);  // scale down before feedback.

	Error = PressSetting - gs_PreFilterPress;
	ScaleUpInput = Error << MAP_SERVO_SCALE;   // scale up before go into integrator.

	// It will generate coverity check warning but we will keep this code for future tuning.
	if (gs_HIMP)
	{
		gs_PreFilterIntegrator += ScaleUpInput / 10;
	}
	else
	{
		gs_PreFilterIntegrator += ScaleUpInput / 10;
	}
}
/**
 * Pre process pressure block
 */
void HFOController_PreProcessPresssureBlock()
{
	int32_t  NotchFilterInput;
	int32_t  ScaleUpInput,Error;

	// scale down before feedback.
	NotchFilterInput = (int32_t) (gs_NtchFtrInputIntegrator >> MAP_SERVO_SCALE);

	Error = gs_CurrentMeasured - NotchFilterInput;

	// scale up before go into integrator.
	ScaleUpInput = Error << MAP_SERVO_SCALE;

	gs_NtchFtrInputIntegrator += ((ScaleUpInput * 120) / MAP_SERVO_RATE);


	gs_PreProcessPress = HFOController_AmplitudeMinimizer(NotchFilterInput,gs_CurrentFreq);

	int32_t AfterOutput;

	AfterOutput = (int32_t) (gs_AfterNtchFtrIntegrator >> MAP_SERVO_SCALE);

	Error = gs_PreProcessPress - AfterOutput;

	ScaleUpInput = Error << MAP_SERVO_SCALE;

	gs_AfterNtchFtrIntegrator += ((ScaleUpInput * 500) / MAP_SERVO_RATE);

	gs_PreProcessPress = AfterOutput;

	//// This block is used to filter out the overshoot before sending preProcessPress
	//// to front panel. The filter is 20/ (s + 20)

	int16_t Temp;

	// scale down before feedback.
	Temp = (gs_FilterPressIntegrator >> MAP_SERVO_SCALE);

	Error =  gs_PreProcessPress - Temp;

	// scale up before go into integrator.
	ScaleUpInput = Error << MAP_SERVO_SCALE;

	gs_FilterPressIntegrator += ScaleUpInput /350;
}
/**
 * Lead circuit block
 * @return int32_t ErrorGain + gs_CurrentMeasured
 */
int32_t HFOController_LeadCircuitBlock()
{
	int32_t ScaleDownIntegrator, Error, ErrorGain, ScaleUpInput;

	ScaleDownIntegrator = (int32_t) (gs_LeadCircuitIntegrator >> MAP_SERVO_SCALE);

	Error = gs_CurrentMeasured - ScaleDownIntegrator;

	ScaleUpInput = Error << MAP_SERVO_SCALE;

	gs_LeadCircuitIntegrator += ScaleUpInput / 5;

	ErrorGain = Error << 2;
	ErrorGain += Error;


	return (ErrorGain + gs_CurrentMeasured);
}
/**
 * This block is dedicated to establish
 * MAP pressure quickly and provide the
 * constant flow setting.
 */
void HFOController_FlowCompensatorBlock()
{
	int32_t Temp32,Error;

	Temp32 = gs_PressureError/(int16_t)PRESS_SCALER;

	if(Temp32 < 0)
		Temp32 = 0;

	if(!gs_HIMP)
	{
		if (gs_DesiredFlow > HFO_BASE_FLOW_LIMIT)
		{
			Temp32 = gs_DesiredFlow;
		}
		else
		{
			Temp32 += gs_DesiredFlow;
			if(Temp32 > HFO_BASE_FLOW_LIMIT)
				Temp32 = HFO_BASE_FLOW_LIMIT;
		}

		E_VentilationMode CurrentMode = devIf_GetSetting()->currentMode;
		if(CurrentMode == eHFOExtremeLowMAPMode)
		{
			Temp32 = gs_DesiredFlow;
		}
	}
	else
	{
		Temp32 += gs_DesiredFlow;
	}



	if(Temp32<0)
		Temp32 = 0;
	else if(Temp32>50)
		Temp32 = 50;

	Temp32 = Temp32*(int16_t)100;
	gs_TotalFlowCmd = (gs_FilterFlowIntegratorx >> MAP_SERVO_SCALE);  // scale down before feedback.

	Error =  Temp32 - gs_TotalFlowCmd;

	Temp32 = Error << MAP_SERVO_SCALE;            // scale up before go into integrator.

	gs_FilterFlowIntegratorx += Temp32 / 25;
}
/**
 * Pressure filter
 */
void HFOController_PressFilter()
{
	int16_t Temp;
	int32_t  ScaleUpInput,Error;

	// scale down before feedback.
	Temp = (gs_FilterPressIntegrator >> MAP_SERVO_SCALE);

	Error =  gs_PreProcessPress - Temp;

	// scale up before go into integrator.
	ScaleUpInput = Error << MAP_SERVO_SCALE;

	gs_FilterPressIntegrator += ScaleUpInput / 500;
}
/**
 * Ramp frequency
 * @param freq
 */
void HFOController_RampFreq(int32_t freq)
{
	static int32_t fiveHundredMsCounter = 0;
	fiveHundredMsCounter++;

	int32_t Freq = (freq > MAX_HFO_FREQ)?MAX_HFO_FREQ:(freq < MIN_HFO_FREQ)?MIN_HFO_FREQ:freq;

	if(fiveHundredMsCounter>250)
	{
		fiveHundredMsCounter = 0;
		if(gs_CurrentFreq>Freq)
			gs_CurrentFreq--;
		else if(gs_CurrentFreq<Freq)
			gs_CurrentFreq++;
	}
}
/**
 * Air filter
 * @param u
 * @return int16_t Output
 */
int16_t HFOController_AirFilter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntAirFFtr>>10);
	Error = u - Output;
	Temp32 = Error/**2*/;
	Temp32 = Temp32<<10;

	gs_IntAirFFtr += (int32_t)Temp32/**100*//iDT;

	return(Output);
}
/**
 * O2 filter
 * @param u
 * @return int16_t Output
 */
int16_t HFOController_O2Filter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntO2FFtr>>10);
	Error = u - Output;
	Temp32 = Error/**2*/;
	Temp32 = Temp32<<10;

	gs_IntO2FFtr += Temp32/**100*//iDT;

	return(Output);
}
/**
 * Calculate HFO Operation time
 */
void  HFOController_CalculateHFOOperationTime()
{
	gs_Clock200hours = gs_Clock200hours+ 2;

	if ((gs_Clock200hours % 60000) == 0)//Write to file every 1 minute
	{
		Command sendMsg;
		sendMsg.id = eLogPostCustomEvent;
		sendMsg.data.logEvent.fileType = eLogHandlerSystemConfigFileType;
		sendMsg.data.logEvent.infoId = eHFOOilPumpTime;
		sendMsg.data.logEvent.infoValue = gs_Clock200hours;
		ipcIf_sendCommandMsg(&sendMsg);
	}

	if(gs_Clock200hours > HOURS_200)//200*60*60*1000)//200 hours
	{
		Command sendMsg;
		sendMsg.id = eSendOilPumpRequestKey;
		ipcIf_sendCommandMsg(&sendMsg);
		gs_Clock200hours = 0;
	}

}

