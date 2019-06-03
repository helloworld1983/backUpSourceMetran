/*
 * VCVController.c
 *
 *  Created on: 3 thg 1, 2019
 *      Author: haxua
 */
#include <controller/FlowController.h>
#include <controller/VCVController.h>
#include <CommonTypes.h>
#include <deviceTask/DeviceInterface.h>
#include "DebugTrace.h"
#include "RootTable.h"
#include "kiss_fftr.h"
#include "_kiss_fft_guts.h"
#include "math.h"
#define SCALE 100
#define EXH_CLOSE_STEP_POS 2040
#define EXH_OPEN_STEP_POS 0
#define FFT_EXAMINE_TIME 500/2
#define OSCILLATION_RANGE 2.5
#define SETTLING_ERROR_RANGE 10
#define NFFT 512
static E_ParameterType gs_ParameterType=eSmallCompliance;
static int32_t gs_MeasuredAirFlow = 0;
static int32_t gs_MeasuredO2Flow  = 0;
static int32_t gs_PressureMeasured = 0;
static int32_t gs_RS485ProxySensor = 0;
static ProxyStatus gs_RS485Status = eNotCalibrated;
static E_SpecificPhaseId gs_CurrentPhaseId=eTheNonePhase;
static int32_t gs_DesiredAirFlow = 0;
static int32_t gs_DesiredO2Flow = 0;
static uint16_t gs_TargetInspPressure=0;
static uint16_t gs_TargetPEEPPressure=0;
static bool gs_IsExhPhase = false;
static int16_t gs_PSet=0;
static float gs_IntPreFtr1=0;
static const uint16_t gs_iDT=500;           //200 for 5ms interval, 500 for 2ms interval
static uint16_t gs_BiasFlow=0;
static int16_t gs_a11 = 500;//change from 159 to 190 to suppress inh flow when test with smart lung; 159;//126;//115;//154;
static int16_t gs_a12 = 600;
static int16_t gs_a13 = -2000;//-906;//-1300;
static int16_t gs_a14 = 20;//350;//424;
static int16_t gs_a21 = -100;
static int16_t gs_a22 = -54;
static int16_t gs_a23 = 10000;//1277;//1455;
static int16_t gs_a24 = 12000;//846;
static int16_t gs_PressureGain = 1200;        /* 1.4 * 1024 */
static int16_t gs_FlowGain  = 1200;//363; //2125;      /* Nco * 1.4, Nco=1500*/
static int16_t gs_DelayNumber = 0;
static bool gs_IsPcv_cnt=false;
static bool gs_FlowCons = false;
static const uint16_t gs_Scalen        =    8;
static uint16_t gs_SwitchTime     = 256;
static int32_t gs_IntFConsFtr=0;
static int32_t gs_CurrentExhMotorStepPosition = 0;
static int32_t gs_CurrentAirMotorStepPosition = 0;
static int32_t gs_CurrentO2MotorStepPosition = 0;
static int16_t gs_ExValvePos;
static int32_t gs_IntPrxRootFtr=0;
static int32_t gs_IntExValve=0;
static int32_t gs_IntExValveFtr=0;
static int32_t gs_IntAirValve=0;
static uint16_t gs_Nco            =    2000;
static int16_t gs_Alf = 499;
static double gs_InhTotalFlow=0;
static uint16_t gs_UpExLmt        = EXH_CLOSE_STEP_POS;//Current Vue: 2200
static int16_t gs_ExhStep=0;
static int16_t gs_AirStep=0;
static int16_t gs_O2Step=0;
static  uint16_t gs_DownExLmt        =    0;
static  uint16_t gs_ValveFtrConst =    30;
static uint16_t gs_ValveFtrExhConst =    20;//100
static  uint16_t gs_InhUpFlowLmt    =  2200;//Current Vue: 2200
static  uint16_t gs_DownFlowLmt    =   0;
static  uint16_t gs_ExhUpFlowLmt = 	1700;//Current Vue: 1500
static int32_t gs_IntAirValveFtr=0;
static int32_t gs_IntO2Valve=0;
static int16_t gs_Bat = 1;
static int32_t gs_IntO2ValveFtr=0;
static uint16_t gs_O2PerSet=0;
static int16_t gs_O2Percent=0;
static int32_t gs_IntO2Percent=0;
static int32_t gs_IntO2SetFtr=0;
static int32_t gs_IntAirFFtr=0;
static int32_t gs_IntO2FFtr=0;
static const uint16_t gs_UpO2PerLmt    =    499;
static const uint16_t gs_DownO2PerLmt    =    1;
static const int16_t DELAY_TIME = 1;
static int32_t gs_DelayBeforeExh=20;
static bool gs_IsEnabled=false;
static int32_t  gs_InhOutputVol;
static int32_t gs_BaseFlow=300;
static uint16_t gs_SetRiseTime=10;
static int16_t gs_MaxPress=0;
static uint32_t gs_SettlingTimeCnt=0;
static int32_t gs_PeakPressure=100000;
static uint32_t gs_RiseTime=0;
static uint32_t gs_RiseTimeHalf=0;
static kiss_fft_cpx sout[NFFT]={0};
static kiss_fft_scalar rinData[NFFT]={0};
static kiss_fft_scalar rin[NFFT]={0};
static kiss_fftr_cfg  kiss_fftr_state;
static int16_t gs_cntFFT = 0;
static desiredMotorPositionStruct gs_DesiredMotorPosition;
static int32_t gs_LastStepExhPosition;
static int32_t gs_LastStepAirPosition;
static int32_t gs_LastStepO2Position;
static void VCVController_SetInitialValue();
static void VCVController_InhProcess(int32_t desiredAir,int32_t desiredO2);
static void VCVController_CalExhArgs();
static void VCVController_PreFilter(void);
static float VCVController_CompCompensatePressure(double compensateRate,bool type);
static float VCVController_FirstPreFilter(float u);
static int16_t VCVController_FlowConsFilter(int16_t u);
static void VCVController_CalculateExhValveArgs(int16_t uiex,int16_t upex);
static int16_t  VCVController_PLFilterRoot(int16_t u);
static void VCVController_DoPressureControl();
static int16_t VCVController_IntegratorEx(int16_t ui, int16_t up);
static int16_t VCVController_PressureRootGain(int16_t u);
static int16_t  VCVController_ExValveFilter(int16_t u);
static int16_t VCVController_IntegratorAir(int16_t ui,int16_t up);
static int16_t VCVController_GainNonCom(int16_t u);
static int16_t  VCVController_AirValveFilter(int16_t u);
static int16_t  VCVController_O2ValveFilter(int16_t u);
static int16_t VCVController_IntegratorO2(int16_t ui,int16_t up);
static void VCVController_CalculateO2ValveArgs(int16_t ui,int16_t up);
static void VCVController_CalculateAirValveArgs(int16_t ui,int16_t up);
static void VCVController_DoO2Servo(void);
static int16_t  VCVController_O2SetFilter(int16_t u);
static int16_t  VCVController_O2Filter(int16_t u);
static int16_t  VCVController_AirFilter(int16_t u);
static int16_t VCVController_IntegratorO2Percent(int16_t u);
static void VCVController_ExhProcess();
/**
 * Set parameter a11
 * @param val parameter
 */
void VCVController_SetA11(int16_t val)
{
	gs_a11 = val;
}
/**
 * Set parameter a12
 * @param val parameter
 */
void VCVController_SetA12(int16_t val)
{
	gs_a12 = val;

}
/**
 * Set parameter a13
 * @param val parameter
 */
void VCVController_SetA13(int16_t val)
{
	gs_a13 = val;

}
/**
 * Set parameter a14
 * @param val parameter
 */
void VCVController_SetA14(int16_t val)
{
	gs_a14 = val;

}
/**
 * Set parameter a21
 * @param val parameter
 */
void VCVController_SetA21(int16_t val)
{
	gs_a21 = val;

}
/**
 * Set parameter a22
 * @param val parameter
 */
void VCVController_SetA22(int16_t val)
{
	gs_a22 = val;

}
/**
 * Set parameter a23
 * @param val parameter
 */
void VCVController_SetA23(int16_t val)
{
	gs_a23 = val;

}
/**
 * Set parameter a24
 * @param val parameter
 */
void VCVController_SetA24(int16_t val)
{
	gs_a24 = val;
}
/**
 * Get inhalation volume
 * @return int32_t gs_InhOutputVol
 */
int32_t VCVController_GetInhVol()
{
	return gs_InhOutputVol;
}
/**
 * Set all parameter to be used for small compliance lung
 */
void VCVController_SetSmallComplianceParameter()
{
	gs_a11=60;
	gs_a12=10;
	gs_a13=-800;
	gs_a14=20;
	gs_a21=-20;
	gs_a22=-5;
	gs_a23=1000;
	gs_a24=23000;
	gs_PressureGain=1000;
	gs_FlowGain=1000;
	gs_SwitchTime=256;
	gs_ValveFtrConst=50;
	gs_ValveFtrExhConst=20;
	gs_InhUpFlowLmt=2200;
	gs_ExhUpFlowLmt=1700;
	gs_DownFlowLmt=500;
	gs_UpExLmt=EXH_CLOSE_STEP_POS;
	gs_DownExLmt=EXH_OPEN_STEP_POS;
	gs_Nco=2000;
	//	gs_a11=180;
	//	gs_a12=22;
	//	gs_a13=-4000;//-906
	//	gs_a14=20;
	//	gs_a21=-2;
	//	gs_a22=-54;
	//	gs_a23=6000;
	//	gs_a24=2500;
	//	gs_PressureGain=1200;
	//	gs_FlowGain=1200;
	//	gs_SwitchTime=256;
	//	gs_ValveFtrConst=30;
	//	gs_ValveFtrExhConst=20;
	//	gs_InhUpFlowLmt=2200;
	//	gs_ExhUpFlowLmt=2200;
	//	gs_DownFlowLmt=500;
	//	gs_UpExLmt=EXH_CLOSE_STEP_POS;
	//	gs_DownExLmt=EXH_OPEN_STEP_POS;
	//	gs_Nco=2000;
}
/**
 * Set all parameter to be used for large compliance lung
 */
void VCVController_SetLargeComplianceParameter()
{
	gs_a11=400;
	gs_a12=200;
	gs_a13=-5000;
	gs_a14=20;
	gs_a21=-20;
	gs_a22=-54;
	gs_a23=1000;
	gs_a24=25000;
	gs_PressureGain=900;
	gs_FlowGain=900;
	gs_SwitchTime=256;
	gs_ValveFtrConst=50;
	gs_ValveFtrExhConst=20;
	gs_InhUpFlowLmt=2200;
	gs_ExhUpFlowLmt=1700;
	gs_DownFlowLmt=500;
	gs_UpExLmt=EXH_CLOSE_STEP_POS;
	gs_DownExLmt=EXH_OPEN_STEP_POS;
	gs_Nco=2000;
	//	gs_a11=400;
	//	gs_a12=22;
	//	gs_a13=-3000;//-906
	//	gs_a14=20;
	//	gs_a21=-2;
	//	gs_a22=-54;
	//	gs_a23=4000;
	//	gs_a24=2000;
	//	gs_PressureGain=1200;
	//	gs_FlowGain=1200;
	//	gs_SwitchTime=256;
	//	gs_ValveFtrConst=30;
	//	gs_ValveFtrExhConst=20;
	//	gs_InhUpFlowLmt=2200;
	//	gs_ExhUpFlowLmt=2200;
	//	gs_DownFlowLmt=600;
	//	gs_UpExLmt=EXH_CLOSE_STEP_POS;
	//	gs_DownExLmt=EXH_OPEN_STEP_POS;
	//	gs_Nco=2000;
}
/**
 * Set initial value for VCV controller
 */
void VCVController_SetInitialValue()
{
	VCVController_CalExhArgs();
	gs_DelayNumber=0;
	gs_FlowCons=false;
}
/**
 * Get breath phase
 * @return bool gs_IsExhPhase Return True if it is in Exhalation phase,
 * 							  otherwise return False
 */
uint8_t VCVController_GetBreathPhase()
{
	return gs_IsExhPhase;
}

/**
 * Set Inhalation target
 * @param target
 * @param riseTime
 */
void VCVController_SetInhalationTarget(uint16_t target,uint16_t riseTime)
{
	gs_TargetInspPressure = target;
	gs_SetRiseTime = riseTime;
	gs_IsExhPhase = false;
}
/**
 * Set Exhalation target
 * @param target
 */
void VCVController_SetExhalationTarget(uint16_t target)
{
	gs_TargetPEEPPressure = target;
	gs_IsExhPhase = true;
}
/**
 * Reset static variables
 */
void VCVController_ResetIMVStaticVariables()
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
	gs_RiseTime=0;
	gs_RiseTimeHalf=0;
	gs_MaxPress=0;
	VCVController_SetSmallComplianceParameter();
}
/**
 * Set bias flow
 * @param flow
 */
void VCVController_SetBiasFlow(int32_t flow)
{
	gs_BaseFlow = flow;
}
/**
 * Enable VCV Controller
 */
void VCVController_Enable()
{
	gs_IsEnabled = true;
}
/**
 * This is the main processing loop for the Inhalation Controllers.
 * The routine obtains the current sensor reading, calculates the desired
 * total flow rate, splits the flow rate into air and oxygen rates, and
 * then commands the Flow Controllers to their respective desired rates.
 * @param monitor Monitor struct
 * @return desiredMotorPositionStruct gs_DesiredMotorPosition Desired motor position
 */
desiredMotorPositionStruct VCVController_Run(RealTimeM4ToA53* monitor)
{
	gs_DesiredMotorPosition.desiredAirPosition=-1;
	gs_DesiredMotorPosition.desiredExhPosition=-1;
	gs_DesiredMotorPosition.desiredHFOPosition=-1;
	gs_DesiredMotorPosition.desiredO2Position=-1;

	if(gs_IsEnabled)
	{
		gs_LastStepExhPosition = monitor->exhValve.stepPosition;
		gs_LastStepAirPosition = monitor->airValve.stepPosition;
		gs_LastStepO2Position = monitor->o2Valve.stepPosition;

		gs_MeasuredAirFlow = monitor->airFlow.lastReading/10;
		gs_MeasuredO2Flow =  monitor->o2Flow.lastReading/10;
		gs_PressureMeasured = monitor->activePress.lastReading/10;
		gs_RS485ProxySensor = monitor->proxy.currentReading;
		gs_RS485Status = monitor->proxy.lastStatus;
		if(gs_IsExhPhase==false)
		{
			if(gs_PressureMeasured>gs_MaxPress)
			{
				gs_MaxPress=gs_PressureMeasured;
			}
		}
		gs_CurrentExhMotorStepPosition = ((double)gs_LastStepExhPosition/(double)MAX_STEP_POSITION_CURRENT)*((double)MAX_STEP_POSITION_OLD);
		gs_CurrentAirMotorStepPosition = ((double)gs_LastStepAirPosition/(double)MAX_STEP_POSITION_CURRENT)*((double)MAX_STEP_POSITION_OLD);
		gs_CurrentO2MotorStepPosition = ((double)gs_LastStepO2Position/(double)MAX_STEP_POSITION_CURRENT)*((double)MAX_STEP_POSITION_OLD);
		VCVController_Calculate();
	}
	return gs_DesiredMotorPosition;
}
void VCVController_SetDesiredFlow(int32_t desiredAir,int32_t desiredO2)
{
	gs_DesiredAirFlow = desiredAir;
	gs_DesiredO2Flow = desiredO2;
}
/**
 * This routine calculates the desired total flow rate
 * for the current execution cycle. The current sensor reading
 * and desired sensor readings are used to generate a desired flow
 * rate
 */
void VCVController_Calculate()
{
	if(gs_CurrentPhaseId == eVcvInhPhase)
	{
		VCVController_InhProcess(gs_DesiredAirFlow,gs_DesiredO2Flow);
		gs_InhTotalFlow += (gs_MeasuredAirFlow+ gs_MeasuredO2Flow);
	}
	else if(gs_CurrentPhaseId == eThePlateauPhase)
	{
		VCVController_SetDesiredFlow(0,0);
		VCVController_InhProcess(0,0);
		gs_InhTotalFlow += (gs_MeasuredAirFlow + gs_MeasuredO2Flow);
	}
	else if(gs_CurrentPhaseId == ePsvInhPhase)//for PSV trigger
	{
		gs_IsPcv_cnt = true;
		VCVController_DoPressureControl();
	}
	else // exh phase
	{
		if(gs_InhTotalFlow != 0){
			//change flow to volume data at 2ms cycle data
			gs_InhOutputVol = (int32_t)(2*gs_InhTotalFlow/60);
		}
		gs_InhTotalFlow = 0;
		VCVController_ExhProcess();
	}
}
/**
 * Check whether VCV Controller is Enable
 * @return bool gs_IsEnabled Return True if VCV Controller is enable.
 * 							 Otherwise, return False.
 */
bool VCVController_IsEnable()
{
	return gs_IsEnabled;
}
/**
 * Disable VCV controller
 */
void VCVController_Disable()
{
	gs_IsEnabled = false;
	FlowController_Reset(eAirFlowController);
	FlowController_Reset(eO2FlowController);
	FlowController_SetEnable(eAirFlowController,false);
	FlowController_SetEnable(eO2FlowController,false);
	gs_DelayBeforeExh = DELAY_TIME;
	gs_TargetInspPressure = 0;
	gs_InhOutputVol = 0;
	gs_InhTotalFlow = 0;
}
/**
 * Set specific phase
 * @param specificPhaseId
 */
void VCVController_SetSepecificPhase(E_SpecificPhaseId specificPhaseId)
{
	gs_CurrentPhaseId = specificPhaseId;
	switch(gs_CurrentPhaseId)
	{
	case ePsvInhPhase:
		FlowController_SetEnable(eO2FlowController,false);
		FlowController_SetEnable(eAirFlowController,false);
		gs_IsExhPhase = false;
		break;
	case eVcvInhPhase:
		FlowController_SetStaticAgr(eAirFlowController,gs_CurrentAirMotorStepPosition);
		FlowController_SetStaticAgr(eO2FlowController,gs_CurrentO2MotorStepPosition);
		FlowController_SetEnable(eO2FlowController,true);
		FlowController_SetEnable(eAirFlowController,true);
		gs_IsExhPhase = false;
		gs_DelayBeforeExh = 0;
		if(gs_SettlingTimeCnt*2>0 && gs_RiseTime*2>50)
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
				DEBUG_VCV("Using Large Compliance ");
			}
			else
			{
				DEBUG_VCV("Using Small Compliance ");

			}
			DEBUG_VCV("Max Amp: %f at Freq: %d, Settling Time: %d, PeakPressure: %d, Rise Time: %d %d %d\n",maxAmp,maxFreq,gs_SettlingTimeCnt*2,gs_PeakPressure,gs_RiseTime*2,gs_RiseTimeHalf*2,gs_MaxPress);
			if(gs_SettlingTimeCnt*2>1300  || gs_RiseTime*2>800)
			{
				gs_ParameterType=eLargeCompliance;
				DEBUG_VCV("Switch to Large Compliance\n");
			}
			if(maxAmp>=OSCILLATION_RANGE || gs_PeakPressure<0 ||gs_RiseTimeHalf*2>150 )
			{
				gs_ParameterType=eSmallCompliance;
				DEBUG_VCV("Switch to Small Compliance\n");
			}
			gs_SettlingTimeCnt=0;
			gs_PeakPressure=100000;
			gs_RiseTime=0;
			gs_cntFFT=0;
			gs_RiseTimeHalf=0;
		}
		break;
	case eThePlateauPhase:
		FlowController_SetEnable(eAirFlowController,true);
		FlowController_SetEnable(eO2FlowController,true);
		gs_IsExhPhase = false;
		gs_DelayBeforeExh = 0;
		break;
	case eNoTrigExhPhase:
		FlowController_SetEnable(eAirFlowController,false);
		FlowController_SetEnable(eO2FlowController,false);
		gs_IsExhPhase = true;
		break;
	default:
		break;
	}

}
/**
 * Handle Exhalation process
 */
void VCVController_ExhProcess()
{
	gs_IsExhPhase = true;
	if(gs_DelayBeforeExh < DELAY_TIME)
	{
		gs_DelayBeforeExh++;
		VCVController_SetInitialValue();
		return;
	}
	//Calculate FFT
	/*******************************************/
	if( abs(gs_PressureMeasured-gs_TargetPEEPPressure)>SETTLING_ERROR_RANGE)
		gs_SettlingTimeCnt++;
	if(gs_PressureMeasured>gs_TargetPEEPPressure*1.1)
	{
		gs_RiseTime++;
	}
	if(gs_PressureMeasured>gs_MaxPress*0.7)
	{
		gs_RiseTimeHalf++;
	}
	if(gs_PressureMeasured<=gs_PeakPressure)
	{
		gs_PeakPressure=gs_PressureMeasured;
	}
	rinData[gs_cntFFT]=gs_PressureMeasured-gs_TargetPEEPPressure;
	gs_cntFFT++;
	if(gs_cntFFT>=NFFT)
	{
		gs_cntFFT=0;
	}
	/*******************************************/
	VCVController_DoPressureControl();
	VCVController_DoO2Servo();
	return;
}
/**
 * Pressure root gain
 * @param u
 * @return int16_t Temp16
 */
int16_t VCVController_PressureRootGain(int16_t u)
{
	uint16_t Root16;
	int16_t Temp16;
	int32_t Temp32;

	Temp16 = VCVController_PLFilterRoot(gs_PressureMeasured);
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
int16_t VCVController_IntegratorEx(int16_t ui, int16_t up)
{
	int32_t Temp32;
	int16_t Temp16 ,Output;

	Temp32 = ui;
	Temp32 = Temp32<<gs_Scalen;
	Temp32 = -Temp32*gs_a13/10;
	Temp32 = Temp32/gs_iDT;
	Temp16 = (int16_t)(gs_IntExValve>>gs_Scalen);
	Temp16 += up;

	Temp16 = VCVController_PressureRootGain(Temp16);

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
int16_t  VCVController_ExValveFilter(int16_t u)
{

	int32_t Temp32;
	int16_t Error,Output;

	Output = (int16_t)(gs_IntExValveFtr>>gs_Scalen);
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<gs_Scalen;
	gs_IntExValveFtr += (int32_t)Temp32*gs_ValveFtrExhConst/gs_iDT;
	return(Output);
}
/**
 * Gain non com
 * @param u
 * @return int16_t y
 */
int16_t VCVController_GainNonCom(int16_t u)
{
	int32_t x;
	int16_t y;
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
 * Integrator Air
 * @param ui
 * @param up
 * @return int16_t Output
 */
int16_t VCVController_IntegratorAir(int16_t ui,int16_t up)
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

	Temp16 = VCVController_GainNonCom(Temp16);

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
 * Air valve filter
 * @param u
 * @return int16_t Output
 */
int16_t  VCVController_AirValveFilter(int16_t u)
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
 * Integrator O2
 * @param ui
 * @param up
 * @return int16_t Output
 */
int16_t VCVController_IntegratorO2(int16_t ui,int16_t up)
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

	Temp16 = VCVController_GainNonCom(Temp16);

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
 * Do Pressure control
 */
void VCVController_DoPressureControl()
{
//	if(gs_ParameterType==eLargeCompliance)
//	{
//		VCVController_SetLargeComplianceParameter();
//	}
//	else
//	{
//		VCVController_SetSmallComplianceParameter();
//	}

	int16_t TotalFlow;
	int16_t Temp16,SwitchCons,eP,eF,uiex,uiflow,upex,upflow;
	int32_t Temp32;

	VCVController_PreFilter();
	eP = gs_PSet - (gs_PressureMeasured);
	gs_BiasFlow = gs_BaseFlow/10;

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
		if(gs_DelayNumber>=100/DEVICE_TASK_DELAY_TIME)
		{
			gs_DelayNumber = 100;
			//			gs_FlowCons = true;
		}
		Temp16 = (int32_t)gs_TargetPEEPPressure - gs_PressureMeasured;
		if((Temp16 >= -10)&&(Temp16 <= 10))
			gs_FlowCons = true;
	}
	if(gs_FlowCons == true)
		SwitchCons = VCVController_FlowConsFilter(0);
	else
		SwitchCons = VCVController_FlowConsFilter(gs_SwitchTime);

	//During the time of exahalation of IMV control , constant flow is required but
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

	Temp16 = VCVController_IntegratorEx(uiex,upex);
	gs_ExValvePos = Temp16;

	gs_ExhStep = VCVController_ExValveFilter(gs_UpExLmt - Temp16);

	Temp16  = VCVController_IntegratorAir(uiflow,upflow);
	gs_AirStep = VCVController_AirValveFilter(Temp16);

	Temp16 = VCVController_IntegratorO2(uiflow,upflow);
	gs_O2Step = VCVController_O2ValveFilter(Temp16);

//	MotorController_MoveToStepPositionOld(eAirMotor,gs_AirStep);
//	MotorController_MoveToStepPositionOld(eO2Motor,gs_O2Step);
//	MotorController_MoveToStepPositionOld(eExhMotor,gs_ExhStep);
	gs_DesiredMotorPosition.desiredAirPosition=gs_AirStep;
	gs_DesiredMotorPosition.desiredExhPosition=gs_ExhStep;
	gs_DesiredMotorPosition.desiredO2Position=gs_O2Step;
}
/**
 * O2 Valve filter
 * @param u
 * @return int16_t Output
 */
int16_t  VCVController_O2ValveFilter(int16_t u)
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
 * Handle Inhalation process
 * @param desiredAir
 * @param desiredO2
 */
void VCVController_InhProcess(int32_t desiredAir,int32_t desiredO2)
{
//	MotorController_MoveToStepPositionOld(eExhMotor,EXH_CLOSE_STEP_POS);
	gs_DesiredMotorPosition.desiredExhPosition = EXH_CLOSE_STEP_POS;
	FlowController_SetDesiredFlow(eAirFlowController,gs_DesiredAirFlow);
	FlowController_SetDesiredFlow(eO2FlowController,gs_DesiredO2Flow);
}
/**
 * Pre filter
 */
void VCVController_PreFilter(void)
{
	int16_t Temp16;
	double C = devIf_GetSetting()->vcv.compRate/(double)SCALE;
	if(!gs_IsExhPhase)
	{
		Temp16 = (int16_t) VCVController_FirstPreFilter(gs_TargetInspPressure
		+VCVController_CompCompensatePressure(C, true)*10.0);//for new tube compensate
	}
	else
	{
		Temp16 = (int16_t) VCVController_FirstPreFilter(gs_TargetPEEPPressure
		+VCVController_CompCompensatePressure(C, true)*10.0);//for new tube compensate
	}
	gs_PSet = Temp16;
}
/**
 * First pre filter
 * @param u
 * @return float gs_IntPreFtr1
 */
float VCVController_FirstPreFilter(float u)
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
		gs_IntPreFtr1 += Error*100.0/(float)gs_iDT;
	return(gs_IntPreFtr1);
}
/**
 * Compute compensate pressure
 * @param compensateRate
 * @param type
 * @return float
 */
float VCVController_CompCompensatePressure(double compensateRate,bool type)
{
	double L = devIf_GetSetting()->vcv.tubeLength/(double)SCALE;//[mm]
	double D = devIf_GetSetting()->vcv.tubeDiameter/(double)SCALE;//[mm]
	double C = compensateRate;
	double P_Compensate = 0;
	double Q =0;
	ProxyStatus Status =gs_RS485Status;
	if((Status==eCalibrated)||(Status==eResetAndRecovered))
	{
		Q = (gs_RS485ProxySensor*devIf_GetSetting()->btpsFactor.exhFactor/(double)SCALE)/100.0; // LPM

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
 * Constant flow switch filter
 * @param u
 * @return int16_t Output
 */
int16_t VCVController_FlowConsFilter(int16_t u)
{
	int32_t Temp32;
	int16_t Error,Output;

	Output = (int32_t)(gs_IntFConsFtr>>gs_Scalen);
	Error = u - Output;
	Temp32 = Error;
	Temp32 = Temp32<<gs_Scalen;
	gs_IntFConsFtr += Temp32*200/gs_iDT;

	return(Output);
}
/**
 * Calculate O2 Valve arguments
 * @param ui
 * @param up
 */
void VCVController_CalculateO2ValveArgs(int16_t ui,int16_t up)
{
	int16_t Temp16 ;
	int32_t Temp32;

	int16_t o2Step= gs_CurrentO2MotorStepPosition;
	Temp16 = o2Step;
	if(o2Step < gs_FlowGain)
		gs_IntO2Valve = ((int32_t)((int32_t)(((int32_t)(gs_Nco*Temp16/(gs_FlowGain - Temp16))*500)/gs_Bat) - up))<<gs_Scalen;
	else
		gs_IntO2Valve = (int32_t)(gs_FlowGain - up)<<gs_Scalen;
	Temp32 = ui;
	Temp32 = Temp32<<gs_Scalen;
	Temp32 = (int32_t)Temp32*gs_a24/10;
	Temp32 = (int32_t)Temp32/gs_iDT;
	gs_IntO2Valve += Temp32;

	if(gs_IntO2Valve>=(int32_t)0x003fffff)
		gs_IntO2Valve = (int32_t)0x003fffff;
	else
		if(gs_IntO2Valve<=(int32_t)0xffc00000)
			gs_IntO2Valve = (int32_t)0xffc00000;

	gs_IntO2ValveFtr = ((int32_t)o2Step) << gs_Scalen;
}
/**
 * Calculate Exhalation valve arguments
 */
void VCVController_CalExhArgs()
{
	int16_t TotalFlow;
	int16_t Temp16,SwitchCons,eP,eF,uiex,uiflow,upex,upflow;
	int32_t Temp32;

	gs_TargetInspPressure=gs_PressureMeasured;
	gs_IsExhPhase = false;
	gs_SetRiseTime = 100;
	VCVController_PreFilter();

	eP = gs_PSet - (gs_PressureMeasured);

	gs_BiasFlow =  gs_BaseFlow/10;

	TotalFlow = (gs_MeasuredAirFlow + gs_MeasuredO2Flow);

	eF = (gs_BiasFlow - TotalFlow);

	Temp32 = (int32_t)eF*gs_a14;
	Temp32 = - Temp32;
	ASSERTION(gs_a13 != 0);
	Temp32 = (int32_t)Temp32/gs_a13;
	Temp16 = (int16_t)Temp32;
	uiex = Temp16 - eP;


	gs_FlowCons = false;
	gs_DelayNumber = 0;
	gs_IsPcv_cnt = false;

	SwitchCons = VCVController_FlowConsFilter(gs_SwitchTime);

	Temp32 = (int32_t)eP*gs_SwitchTime;
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

	Temp32 = gs_PressureMeasured*gs_SwitchTime;
	Temp32+= (int32_t)10* gs_TargetPEEPPressure*(gs_SwitchTime - SwitchCons);
	Temp32 = Temp32>>10;
	Temp16 = (int16_t)Temp32;
	Temp32 = (int32_t)Temp16*gs_a21;
	Temp32 += (int32_t)TotalFlow*gs_a22;
	Temp32 = (int32_t)Temp32/10;
	upflow = (int16_t)Temp32;


	//Calculate Exh Valve Args
	VCVController_CalculateExhValveArgs(uiex,upex);

	//Calculate Air O2 Valve Args
	VCVController_CalculateAirValveArgs(uiflow,upflow);
	VCVController_CalculateO2ValveArgs(uiflow,upflow);
	VCVController_DoO2Servo();
}
/**
 * Air Flow filter
 * @param u
 * @return int16_t Output
 */
int16_t  VCVController_AirFilter(int16_t u)
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
int16_t  VCVController_O2Filter(int16_t u)
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
 * @return int16_t Output
 */
int16_t  VCVController_O2SetFilter(int16_t u)
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
 * Integrator O2 percent
 * @param u
 * @return int16_t Output
 */
int16_t VCVController_IntegratorO2Percent(int16_t u)
{
	int32_t Temp32;
	int16_t Temp16 ,Output;

	Temp32 = u;
	Temp32 = Temp32<<10;
//	Temp32 = Temp32;
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
 * Do O2 Servo
 */
void VCVController_DoO2Servo(void)
{

	int16_t Temp16,AverageAirF,AverageO2F,O2SetFtr;
	int32_t Temp32A,Temp32B;

	AverageAirF = VCVController_AirFilter(gs_MeasuredAirFlow);
	AverageO2F = VCVController_O2Filter(gs_MeasuredO2Flow);

	gs_O2PerSet = ((79*devIf_GetSetting()->ratio.DesiredO2Ratio) >> 7)+ 21;

	O2SetFtr = VCVController_O2SetFilter(gs_O2PerSet);

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
			gs_Bat = VCVController_IntegratorO2Percent(O2SetFtr - gs_O2Percent);

			gs_Alf = 500 - gs_Bat;
		}
	}
}
/**
 * Calculate Air valve arguments
 * @param ui
 * @param up
 */
void VCVController_CalculateAirValveArgs(int16_t ui,int16_t up)
{
	int16_t Temp16;
	int32_t Temp32;
	int16_t airStep= gs_CurrentAirMotorStepPosition;
	Temp16 = airStep;

	if(airStep < gs_FlowGain)
		gs_IntAirValve = ((int32_t)((int32_t)(((int32_t)(gs_Nco*Temp16/(gs_FlowGain - Temp16))*500)/gs_Alf) - up))<<gs_Scalen;
	else
		gs_IntAirValve = (int32_t)(gs_FlowGain - up)<<gs_Scalen;

	Temp32 = ui;
	Temp32 = Temp32<<gs_Scalen;
	Temp32 = (int32_t)Temp32*gs_a24/10;
	Temp32 = (int32_t)Temp32/gs_iDT;
	gs_IntAirValve += Temp32;

	if(gs_IntAirValve>=(int32_t)0x003fffff){
		gs_IntAirValve = (int32_t)0x003fffff;
	}
	else if(gs_IntAirValve<=(int32_t)0xffc00000){
		gs_IntAirValve = (int32_t)0xffc00000;
	}

	gs_IntAirValve = ((int32_t)airStep) << gs_Scalen;
}
/**
 * PL Filter root
 * @param u
 * @return
 */
int16_t  VCVController_PLFilterRoot(int16_t u)
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
 * Calculate Exhaltion valve arguments
 * @param uiex
 * @param upex
 */
void VCVController_CalculateExhValveArgs(int16_t uiex,int16_t upex)
{
	int16_t Temp16;
	int32_t Temp32;
	uint16_t Root16;
	int16_t Temp16_1;
	int32_t Temp32_1;
	double currentExhStep =gs_CurrentExhMotorStepPosition;
	uint16_t exhStep = currentExhStep;

	Temp16 = MAX_STEP_POSITION_OLD - exhStep;

	gs_ExValvePos = Temp16;

	Temp16_1 = VCVController_PLFilterRoot(gs_PressureMeasured);
	if(Temp16_1<10)
		Temp16_1 = 10;
	else
		if(Temp16_1>1000)
			Temp16_1 = 1000;

	if(Temp16_1<255)
	{
		Root16 = m_table[Temp16_1];
		Temp32_1 = (int32_t)Root16*1000;
		Temp32_1 = (int32_t)Temp32_1/3162; /* Temp32 = Root16/sqrt(10) */
		Temp32_1 = Temp32_1>>2;
		Temp16_1 = (int16_t)Temp32_1;
	}
	else
	{
		Temp16_1 = Temp16_1/10;
		Root16 = m_table[Temp16_1];
		Temp16_1 = Root16>>2;
	}

	Temp16_1 += 2048;

	Temp32_1 = (int32_t)Temp16*Temp16_1;

	Temp16 = (int16_t)(Temp32_1/gs_PressureGain);
	Temp16 = Temp16 - upex;
	gs_IntExValve = Temp16 << gs_Scalen;

	Temp32 = uiex;
	Temp32 = Temp32<<gs_Scalen;
	Temp32 = -Temp32*gs_a13/10;
	Temp32 = Temp32/gs_iDT;
	gs_IntExValve += Temp32;

	gs_IntExValveFtr = ((int32_t)exhStep) << gs_Scalen;
	gs_IntPrxRootFtr = (int32_t)(gs_TargetInspPressure) << gs_Scalen;
}

