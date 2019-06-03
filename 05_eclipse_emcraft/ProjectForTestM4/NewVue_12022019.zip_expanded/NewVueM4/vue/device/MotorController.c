/*
 * MotorController.c
 *
 *  Created on: 2 thg 1, 2019
 *      Author: haxua
 */
#include <board/spi.h>
#include "MotorController.h"
#include "stdlib.h"
#include "fsl_gpio.h"
#include "Delay.h"
#include "math.h"
#include "fsl_debug_console.h"
#include "DebugTrace.h"
#include "CommonTypes.h"
#define PI 3.14159265

#define CS_HFO_PORT                     (GPIO3)
#define CS_HFO_PIN                      (9U)

#define CS_EXH_PORT                     (GPIO3)
#define CS_EXH_PIN                      (8U)

#define CS_O2_PORT                     (GPIO3)
#define CS_O2_PIN                      (0U)

#define CS_AIR_PORT                     (GPIO3)
#define CS_AIR_PIN                      (17U)

static int32_t gs_DesiredAirStep =0;
static int32_t gs_DesiredO2Step  =0;
static int32_t gs_DesiredExhStep =0;
static int32_t gs_DesiredHFOStep =0;
static int32_t gs_CurrentAirStep =0;
static int32_t gs_CurrentO2Step =0;
static int32_t gs_CurrentExhStep =0;
static int32_t gs_CurrentHFOStep =0;
static unsigned short gs_LiftOffAirStep =0;
static unsigned short gs_LiftOffO2Step =0;
static unsigned short gs_LiftOffExhStep =0;
static unsigned short gs_LiftOffHFOStep =0;
static int32_t gs_MaxAirStep =2200;
static int32_t gs_MaxO2Step  =2200;
static int32_t gs_MaxExhStep =2200;
static int32_t gs_MaxHFOStep =2200;
static int32_t gs_Freq = 0;
static int32_t gs_StrokeVolume = 0;
static int32_t gs_OffSetPosition = 0;
static int32_t gs_OffSetCurrentPosition = 0;
static int32_t gs_t=0;
static int32_t gs_T=0;
/**
 * Initialize all Lift Off value to 0
 */
void MotorController_InitLiftOff()
{
	gs_LiftOffAirStep=0;
	gs_LiftOffO2Step =0;
	gs_LiftOffExhStep=0;
	gs_LiftOffHFOStep=0;
}
/**
 * Get current desired Air valve step
 * @return int32_t gs_DesiredAirStep current desired step
 */
int32_t MotorController_GetDesiredAirStep(void)
{
	return -gs_DesiredAirStep;
}
/**
 * Get current desired O2 valve step
 * @return int32_t gs_DesiredO2Step current desired step
 */
int32_t MotorController_GetDesiredO2Step(void)
{
	return -gs_DesiredO2Step;
}
/**
 * Get current desired Exhalation valve step
 * @return int32_t gs_DesiredExhStep current desired step
 */
int32_t MotorController_GetDesiredExhStep(void)
{
	return -gs_DesiredExhStep;
}
/**
 * Get current desired HFO valve step
 * @return int32_t gs_DesiredHFOStep current desired step
 */
int32_t MotorController_GetDesiredHFOStep(void)
{
	return -gs_DesiredHFOStep;
}
/**
 * Send data to TMC4361 Motor controller via SPI communication
 * @param MotorId Specify which Motor controller to communicate
 * @param address Specify Address of register
 * @param datagram Specify the value of register
 */
void MotorController_SendData (uint8_t MotorId, uint8_t address, int32_t datagram)
{
	//TMC4361 takes 40 bit data: 8 address and 32 data
	switch (MotorId)
	{
	case eHFOMotor:
		GPIO_PinWrite(CS_HFO_PORT, CS_HFO_PIN, 0U);
		break;

	case eExhMotor:
		GPIO_PinWrite(CS_EXH_PORT, CS_EXH_PIN, 0U);
		break;

	case eO2Motor:
		GPIO_PinWrite(CS_O2_PORT, CS_O2_PIN, 0U);
		break;

	case eAirMotor:
		GPIO_PinWrite(CS_AIR_PORT, CS_AIR_PIN, 0U);
		break;

	default:
		break;
	}
	delay_us(10);
	spi_SendData(address,datagram);
	switch (MotorId)
	{
	case eHFOMotor:
		GPIO_PinWrite(CS_HFO_PORT, CS_HFO_PIN, 1U);
		break;

	case eExhMotor:
		GPIO_PinWrite(CS_EXH_PORT, CS_EXH_PIN, 1U);
		break;

	case eO2Motor:
		GPIO_PinWrite(CS_O2_PORT, CS_O2_PIN, 1U);
		break;

	case eAirMotor:
		GPIO_PinWrite(CS_AIR_PORT, CS_AIR_PIN, 1U);
		break;

	default:
		break;
	}
}

/**
 * Read data from TMC4361 Motor controller via SPI communication
 * @param MotorId Specify which Motor controller to communicate
 * @param address Specify Address of register
 * @param datagram Specify the value of register
 */
void MotorController_ReadData(uint8_t MotorId, uint8_t address, uint32_t* datagram)
{
	//TMC4361 takes 40 bit data: 8 address and 32 data
	switch (MotorId)
	{
	case eHFOMotor:
		GPIO_PinWrite(CS_HFO_PORT, CS_HFO_PIN, 0U);
		break;

	case eExhMotor:
		GPIO_PinWrite(CS_EXH_PORT, CS_EXH_PIN, 0U);
		break;

	case eO2Motor:
		GPIO_PinWrite(CS_O2_PORT, CS_O2_PIN, 0U);
		break;

	case eAirMotor:
		GPIO_PinWrite(CS_AIR_PORT, CS_AIR_PIN, 0U);
		break;

	default:
		break;
	}
	delay_us(10);
	spi_ReadData(address,datagram);
	switch (MotorId)
	{
	case eHFOMotor:
		GPIO_PinWrite(CS_HFO_PORT, CS_HFO_PIN, 1U);
		break;

	case eExhMotor:
		GPIO_PinWrite(CS_EXH_PORT, CS_EXH_PIN, 1U);
		break;

	case eO2Motor:
		GPIO_PinWrite(CS_O2_PORT, CS_O2_PIN, 1U);
		break;

	case eAirMotor:
		GPIO_PinWrite(CS_AIR_PORT, CS_AIR_PIN, 1U);
		break;

	default:
		break;
	}
}

/**
 * Config Closed Loop control for specified Motor
 * @param MotorID Specify which Motor controller to be configured
 */
static void MotorController_InitClosedLoopControl(E_MOTORID MotorID)
{
	if(MotorID!=eHFOMotor)
	{
		MotorController_SendData(MotorID, 0xCF,0x52535400); // reset squirrel

		/*=============CLK_FREQ(Add = R: 0x31 - W: 0xB1): = 0x00B71B00====================
	    + CLK_FREQ(Frequence): 0x007A1200 - 8Mhz
	                           0x00B71b00 - 12Mhz
	                           0x00F42400 -16Mhz
	                           0x01036640 -17Mhz
		 */
		MotorController_SendData(MotorID, 0xB1,0x00B71B00);

		/*
		 * GENERAL_CONF(0x00)
		 * direct_acc_val_en = 1 => Acceleration values are set directly as steps per clock cycle
		 * pol_dir_out = 1 => Indicate negative direction
		 * diff_enc_in_disable = 1 => Differential encoder input is disabled
		 * stdby_clk_pin_assignment = 3 => Internal clock is forwarded to STDBY_CLK output pin
		 */
		MotorController_SendData(MotorID, 0x80,0x00007022);

		/*
		 * STEP_CONF (0x0A)
		 * MSTEP_PER_FS = 256 => 256 microsteps per full step
		 * FS_PER_REV = 200 => full steps per revolution
		 */
		MotorController_SendData(MotorID, 0x8A,0x00000C80);

		/*
		 * INPUT_FILT_CONF (0x03)
		 * SR_ENC_IN = 2 => Input sample rate = fclk/4
		 * FILT_L_ENC_IN = 2 => Filter length for Filter length for these pins: A_SCLK, ANEG_NSCLK,
		 * 						B_SDI, BNEG_NSDI, N, NNEG = 2
		 */
		MotorController_SendData(MotorID, 0x83,0x00000022);

		/*
		 * SPI_OUT_CONF (0x04)
		 * SPI_OUT_BLOCK_TIME = 8
		 * SPI_OUT_HIGH_TIME = 4
		 * SPI_OUT_LOW_TIME = 4
		 */
		MotorController_SendData(MotorID, 0x84,0x8440000A);

		/*
		 * Set the DRVCTRL register of TMC26x/389  (cover datagram): single edge steps, disable
		 * step interpolation, microstep resolution: 256
		 */
		MotorController_SendData(MotorID, 0xEC,0x00000000);
		delay_ms(100);
		/*
		 * Set the CHOPCONF register of TMC26x/389 (cover datagram): tbl=36, standard
		 * chopper, HDEC=16, HEND=11, HSTR=1, TOFF=5, RNDTF=off
		 */
		MotorController_SendData(MotorID, 0xEC,0x00090585);
		delay_ms(100);
		/*
		 * Disable the SMARTEN register of TMC26x/389 (cover datagram)
		 */
		MotorController_SendData(MotorID, 0xEC,0x000A0000);
		delay_ms(100);
		/*
		 * Set the SGCSCONF register of TMC26x/389 (cover datagram): SGT=0,CS=14
		 */
		MotorController_SendData(MotorID, 0xEC,0x000C000E);
		delay_ms(100);
		/*
		 * Set the DRVCONF register of TMC26x/389 (cover datagram): SLPH=3, SLPL=3,
		 * DISS2G=off, TS2G=0-3.2us, SDOFF=on, VSENSE=0
		 */
		MotorController_SendData(MotorID, 0xEC,0x000EF080);
		delay_ms(100);
		/*
		 * Encoder registers(0x54)
		 * ENC_IN_RES = 40000
		 */
		MotorController_SendData(MotorID, 0xD4,0x00009C40);

		/*
		 * ENC_IN_CONF (0x07)
		 * invert_enc_dir = 1 => Encoder direction is inverted internally
		 */
		//		MotorController_SendData(MotorID, 0x87,0x20000000);

		/*
		 * PID and Closed Loop registers(0x1C)
		 * CL_BETA=255
		 * CL_GAMMA=255
		 */
		MotorController_SendData(MotorID, 0x9C,0x00FF00FF);//0x00FF00FF //Nearest: 0x00FF00C8

		/*
		 * PID and Closed Loop registers(0x5C)
		 * CL_DELTA_P = 1.0
		 */
		MotorController_SendData(MotorID, 0xDC,0x02C000);// 0x10000 //Nearest: 0x018000

		/*
		 * PID and Closed Loop registers(0x5F)
		 * CL_TOLERANCE = 10
		 */
		MotorController_SendData(MotorID, 0xDF,0x0000000A);

		/*
		 * CURRENT_CONF (0x05)
		 * Disable current scaling
		 */
		MotorController_SendData(MotorID, 0x85,0x00000000);

		/*
		 * XACTUAL(0x21)
		 * XACTUAL = 0
		 */
		MotorController_SendData(MotorID, 0xA1,0x00000000);

		/*
		 * ENC_POS(0x50)
		 * ENC_POS = 0
		 */
		MotorController_SendData(MotorID, 0xD0,0x00000000);


		/*
		 * Ramp Generator Registers (0x20)
		 * Operation mode = 1 => Positioning Mode
		 * Motion profile = 1 => Trapezoidal ramp
		 */
		MotorController_SendData(MotorID, 0xA0,0x00000005);

		/*
		 * Ramp Generator Registers (0x24)
		 * VMAX = 51200 pps
		 */
		MotorController_SendData(MotorID, 0xA4,0x09000000);

		/*
		 * Ramp Generator Registers (0x28)
		 * AMAX = 16000 pps => 0x3E80
		 */
		MotorController_SendData(MotorID, 0xA8,25000);//16000

		/*
		 * Ramp Generator Registers (0x29)
		 * DMAX = 10000 pps => 0x2710
		 */
		MotorController_SendData(MotorID, 0xA9,20000);//10000
		/*
		 * Target and Compare Registers (0x37)
		 * XTARGET = 384
		 */
		MotorController_SendData(MotorID, 0xB7,-384);

		//Wait for target is reached
		delay_ms(1000);
		DEBUG_MOTORCONTROLLER("Calibrate Motor %d: %d\n",MotorID,MotorController_GetCurrentStepPosition(MotorID));

		/*
		 * ENC_IN_CONF (0x07)
		 * cl_calibration_en = 1 => Closed-loop calibration is active
		 * regulation_modus = 1 => Close-loop operation is enable
		 */
		MotorController_SendData(MotorID, 0x87,0x01400000);

		//Wait for 10ms
		delay_ms(10);
		/*
		 * ENC_IN_CONF (0x07)
		 * cl_calibration_en = 0 => Closed-loop calibration is disable
		 * regulation_modus = 1 => Close-loop operation is enable
		 */
		MotorController_SendData(MotorID, 0x87,0x00400000);

		/*
		 * PID and Closed-loop Registers (0x5A)
		 * CL_VMAX_CALC_P = 2000 => Proportional term for velocity limitation = 2000
		 */
		MotorController_SendData(MotorID, 0xDA,0x000007D0);

		/*
		 * PID and Closed-loop Registers (0x5B)
		 * CL_VMAX_CALC_I = 100 => Integral term for velocity limitation =100
		 */
		MotorController_SendData(MotorID, 0xDB,0x00000064);

		/*
		 * PID and Closed-loop Registers (0x5B)
		 * PID_DV_CLIP = 200000 => Clipping value for catch-up velocity = 200 kpps
		 */
		MotorController_SendData(MotorID, 0xDE,200000);

		/*
		 * PID and Closed-loop Registers (0x5D)
		 * PID_I_CLIP = 2000 => Clipping value for integral term =2000
		 */
		MotorController_SendData(MotorID, 0xDD,0x000007D0);

		/*
		 * PID and Closed-loop Registers (0x60)
		 * CL_VMIN_EMF = 50000 => emf_vmin=50 Kpps
		 */
		MotorController_SendData(MotorID, 0xE0,50000);

		/*
		 * PID and Closed-loop Registers (0x61)
		 * CL_VADD_EMF  = 50000 => emf_vadd=450Kpps -> emf_vmax=500Kpps
		 */
		MotorController_SendData(MotorID, 0xE1,450000);

		/*
		 * PID and Closed-loop Registers (0x63)
		 * ENC_VMEAN_WAIT=100
		 * ENC_VMEAN_FILTER=11
		 * ENC_VMEAN_INT=512
		 */
		MotorController_SendData(MotorID, 0xE3,0x2000b64);

		/*
		 * ENC_IN_CONF (0x07)
		 * regulation_modus = 1 => Close-loop operation is enable
		 * cl_vlimit_en = 1 => Catch-up velocity during closed-loop operation is limited by
		 * 					   internal PI regulator
		 */
		MotorController_SendData(MotorID, 0x87,0x08400000);

		/*
		 * SCALE_VALUES (0x06)
		 * CL_IMAX = 255 => Closed-loop maximum scaling value
		 * CL_IMIN = 100 => Closed-loop minimum scaling value
		 * CL_START_UP = 100 => |ENC_POS_DEV| value at which closed-loop scaling increase the current
		 *  					 value above CL_IMIN
		 */
		MotorController_SendData(MotorID, 0x86,0x000FFF7D);//0x0032F080 CL_IMAX = 240  CL_IMIN = 128 CL_START_UP = 50

		/*
		 * Various Scaling Configuration Registers (0x18)
		 * CL_UPSCALE_DELAY = 1000 clock cycles
		 */
		MotorController_SendData(MotorID, 0x98,0x3E8);

		/*
		 * Various Scaling Configuration Registers (0x19)
		 * CL_DNSCALE_DELAY = 100000 clock cycles
		 */
		MotorController_SendData(MotorID, 0x99,0x186a0);

		/*
		 * CURRENT_CONF (0x05)
		 * closed_loop_scale_en = 1 => Enable Closed-loop scaling
		 */
		MotorController_SendData(MotorID, 0x85,0x00000080);

		/*
		 * Encoder registers (0x52)
		 * CL_TR_TOLERANCE = 60 => Tolerated absolute tolerance between XACTUAL and ENC_POS
		 * 						   to trigger TARGET_REACHED
		 */
		MotorController_SendData(MotorID, 0xD2,0x0000003C);

		/*
		 * Ramp Generator Registers (0x20)
		 * Operation mode = 1 => Positioning Mode
		 * Motion profile = 1 => Trapezoidal ramp
		 */
		MotorController_SendData(MotorID, 0xA0,0x00000005);

		/*
		 * Ramp Generator Registers (0x24)
		 * VMAX = 51200 pps
		 */
		MotorController_SendData(MotorID, 0xA4,0x09000000);

		/*
		 * Ramp Generator Registers (0x28)
		 * AMAX = 16000 pps => 0x3E80
		 */
		MotorController_SendData(MotorID, 0xA8,25000);//16000

		/*
		 * Ramp Generator Registers (0x29)
		 * DMAX = 10000 pps => 0x2710
		 */
		MotorController_SendData(MotorID, 0xA9,20000);//10000
		DEBUG_MOTORCONTROLLER("Calibrate Motor %d: %d\n",MotorID,MotorController_GetCurrentStepPosition(MotorID));
	}
	else
	{
		//initialize motor
		MotorController_SendData(MotorID, 0xCF,0x52535400); // reset squirrel

		/*=============CLK_FREQ(Add = R: 0x31 - W: 0xB1): = 0x00B71B00====================
		    + CLK_FREQ(Frequence): 0x007A1200 - 8Mhz
		                           0x00B71b00 - 12Mhz
		                           0x00F42400 -16Mhz
		                           0x01036640 -17Mhz
		 */
		MotorController_SendData(MotorID, 0xB1,0x00B71B00);

		/*==============GENERAL_CONF(Add = R: 0x80 - W: 0x00):= 0x00006026=================
		    Direct-a, direct - bow
		 */
		MotorController_SendData(MotorID, 0x80,0x00007022);

		/*
		 * STEP_CONF (0x0A)
		 * MSTEP_PER_FS = 256 => 256 microsteps per full step
		 * FS_PER_REV = 200 => full steps per revolution
		 */
		MotorController_SendData(MotorID, 0x8A,0x00000C80);

		/*
		 * INPUT_FILT_CONF (0x03)
		 * SR_ENC_IN = 2 => Input sample rate = fclk/4
		 * FILT_L_ENC_IN = 2 => Filter length for Filter length for these pins: A_SCLK, ANEG_NSCLK,
		 * 						B_SDI, BNEG_NSDI, N, NNEG = 2
		 */
		MotorController_SendData(MotorID, 0x83,0x00000022);

		/*
		 * SPI_OUT_CONF (0x04)
		 * SPI_OUT_BLOCK_TIME = 8
		 * SPI_OUT_HIGH_TIME = 4
		 * SPI_OUT_LOW_TIME = 4
		 */
		MotorController_SendData(MotorID, 0x84,0x8440000A);

		/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x00000000================
		    DRV_STATUS response and COVER_LOW send

		    Using cover datagram to write to DRVCTRL of TMC26x  sendData   (0xEC,0x00000000)
		    0x00000000: single edge steps, disable step interpolation - 256 microstep
		 */
		MotorController_SendData(MotorID, 0xEC,0x00000000);

		/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x00091935================
		    DRV_STATUS response and COVER_LOW send

		    Using cover datagram to write to CHOPCONF of TMC26x        (0xEC,0x00090585)
		    tbl=36, standard chopper, HDEC=16, HEND=11, HSTR=1, TOFF=5, RNDTF=off
		 */
		MotorController_SendData(MotorID, 0xEC,0x00090585);

		/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000A0000================
		    DRV_STATUS response and COVER_LOW send

		    Using cover datagram to write to SMARTEN of TMC26x (0xEC,0x000A0000)
		    tbl=36, standard chopper, HDEC=16, HEND=11, HSTR=1, TOFF=5, RNDTF=off
		 */
		MotorController_SendData(MotorID, 0xEC,0x000A0000);

		/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000D05xx================
		    DRV_STATUS response and COVER_LOW send

		    Using cover datagram to write to SGCSCONF of TMC26x (0xEC,0x000D05xx)
		    Disable Smarten Register, SGT=0,CS=24
		 */
		MotorController_SendData(MotorID, 0xEC,0x000C001F);

		/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000EF040==============
		    DRV_STATUS response and COVER_LOW send

		    Using cover datagram to write to DRVCONF of TMC26x     (0xEC,0x000EF000)
		    SLPH = 3, SLPL = 3, DISS2G = off, TS2G = 0-3.2µs, SDOFF = on, VSENSE = 0
		 */
		MotorController_SendData(MotorID, 0xEC,0x000EF080); // Using cover datagram to write to DRVCONF of TMC26x                     (0xEC,0x000EF000)

		/****************************************************************************************/
		/*
		 * Encoder registers(0x54)
		 * ENC_IN_RES = 40000
		 */
		MotorController_SendData(MotorID, 0xD4,0x00009C40);

		/*
		 * ENC_IN_CONF (0x07)
		 * invert_enc_dir = 1 => Encoder direction is inverted internally
		 */
		MotorController_SendData(MotorID, 0x87,0x20000000);

		/*
		 * PID and Closed Loop registers(0x1C)
		 * CL_BETA=255
		 * CL_GAMMA=255
		 */
		MotorController_SendData(MotorID, 0x9C,0x00FF00FF);//0x00FF00FF //Nearest: 0x00FF00C8

		/*
		 * PID and Closed Loop registers(0x5C)
		 * CL_DELTA_P = 1.0
		 */
		MotorController_SendData(MotorID, 0xDC,0x00F000);// 0x10000 //Nearest: 0x018000

		/*
		 * PID and Closed Loop registers(0x5F)
		 * CL_TOLERANCE = 10
		 */
		MotorController_SendData(MotorID, 0xDF,0x000000032);

		/*
		 * CURRENT_CONF (0x05)
		 * Disable current scaling
		 */
		MotorController_SendData(MotorID, 0x85,0x00000000);

		/*
		 * XACTUAL(0x21)
		 * XACTUAL = 0
		 */
		MotorController_SendData(MotorID, 0xA1,0x00000000);

		/*
		 * ENC_POS(0x50)
		 * ENC_POS = 0
		 */
		MotorController_SendData(MotorID, 0xD0,0x00000000);

		/*
		 * Ramp Generator Registers (0x20)
		 * Operation mode = 1 => Positioning Mode
		 * Motion profile = 1 => Trapezoidal ramp
		 */
		MotorController_SendData(MotorID, 0xA0,0x00000005);

		/*
		 * Ramp Generator Registers (0x24)
		 * VMAX = 51200 pps
		 */
		MotorController_SendData(MotorID, 0xA4,0x09000000);

		/*
		 * Ramp Generator Registers (0x28)
		 * AMAX = 16000 pps => 0x3E80
		 */
		MotorController_SendData(MotorID, 0xA8,10000);//16000

		/*
		 * Ramp Generator Registers (0x29)
		 * DMAX = 10000 pps => 0x2710
		 */
		MotorController_SendData(MotorID, 0xA9,10000);//10000

		uint8_t sameValueCounter=0;
		int32_t prevStep=0;
		int32_t step=0;
		while(sameValueCounter<10)
		{
			/*
			 * Target and Compare Registers (0x37)
			 * XTARGET = step
			 */
			step+=5;
			MotorController_SendData(MotorID, 0xB7,step);
			int32_t currentStep = MotorController_GetCurrentStepPosition(MotorID);
			if(abs(currentStep-prevStep)<=5)
			{
				sameValueCounter++;
			}
			else
			{
				sameValueCounter=0;
				prevStep = currentStep;
			}
			delay_ms(4);
		}

		DEBUG_MOTORCONTROLLER("HFO Motor Go To Zero %d: %d\n",MotorID,MotorController_GetCurrentStepPosition(MotorID));
		/*==============VSTART(Add = R: 0x37 - W: 0xB7):= 0xB7,0x00000000================
				    Start velocity of ramp generator- v = 0
		 */
		MotorController_SendData(MotorID, 0xB7,0x00000000);
		/*
		 * Ramp Generator Registers (0x24)
		 * VMAX = 0 pps
		 */
		MotorController_SendData(MotorID, 0xA4,0);

		/*
		 * Ramp Generator Registers (0x21)
		 * XACTUAL = 0 pps
		 */
		MotorController_SendData(MotorID, 0xA1,0);

		/*
		 * Target and Compare Registers (0x37)
		 * XTARGET = 0
		 */

		MotorController_SendData(MotorID, 0xB7,0);

		/*
		 * Encoder Register (0x50)
		 * ENC_POS = 0 pps
		 */

		MotorController_SendData(MotorID, 0xD0,0);
		/*
		 * Ramp Generator Registers (0x20)
		 * Operation mode = 1 => Positioning Mode
		 * Motion profile = 1 => Trapezoidal ramp
		 */
		MotorController_SendData(MotorID, 0xA0,0x00000005);

		/*
		 * Ramp Generator Registers (0x24)
		 * VMAX = 51200 pps
		 */
		MotorController_SendData(MotorID, 0xA4,51200*4);

		/*
		 * Ramp Generator Registers (0x28)
		 * AMAX = 16000 pps => 0x3E80
		 */
		MotorController_SendData(MotorID, 0xA8,10000*3);//16000

		/*
		 * Ramp Generator Registers (0x29)
		 * DMAX = 10000 pps => 0x2710
		 */
		MotorController_SendData(MotorID, 0xA9,10000*3);//10000


		DEBUG_MOTORCONTROLLER("Start Calibrate Motor %d \n",MotorController_GetCurrentStepPosition(MotorID));
		/*
		 * Target and Compare Registers (0x37)
		 * XTARGET = 384
		 */

		MotorController_SendData(MotorID, 0xB7,-384);
		//Wait for target is reached
		delay_ms(1000);
		DEBUG_MOTORCONTROLLER("Calibrate Motor %d: %d\n",MotorID,MotorController_GetCurrentStepPosition(MotorID));
		/*
		 * ENC_IN_CONF (0x07)
		 * cl_calibration_en = 1 => Closed-loop calibration is active
		 * regulation_modus = 1 => Close-loop operation is enable
		 */
		MotorController_SendData(MotorID, 0x87,0x21400000);

		//Wait for 10ms
		delay_ms(10);
		/*
		 * ENC_IN_CONF (0x07)
		 * cl_calibration_en = 0 => Closed-loop calibration is disable
		 * regulation_modus = 1 => Close-loop operation is enable
		 */
		MotorController_SendData(MotorID, 0x87,0x20400000);

		/*
		 * PID and Closed-loop Registers (0x5A)
		 * CL_VMAX_CALC_P = 2000 => Proportional term for velocity limitation = 2000
		 */
		MotorController_SendData(MotorID, 0xDA,2000);

		/*
		 * PID and Closed-loop Registers (0x5B)
		 * CL_VMAX_CALC_I = 100 => Integral term for velocity limitation =100
		 */
		MotorController_SendData(MotorID, 0xDB,100);

		/*
		 * PID and Closed-loop Registers (0x5B)
		 * PID_DV_CLIP = 200000 => Clipping value for catch-up velocity = 200 kpps
		 */
		MotorController_SendData(MotorID, 0xDE,0xC350);

		/*
		 * PID and Closed-loop Registers (0x5D)
		 * PID_I_CLIP = 2000 => Clipping value for integral term =2000
		 */
		MotorController_SendData(MotorID, 0xDD,0x3E8);

		/*
		 * PID and Closed-loop Registers (0x60)
		 * CL_VMIN_EMF = 50000 => emf_vmin=50 Kpps
		 */
		MotorController_SendData(MotorID, 0xE0,50000);

		/*
		 * PID and Closed-loop Registers (0x61)
		 * CL_VADD_EMF  = 50000 => emf_vadd=450Kpps -> emf_vmax=500Kpps
		 */
		MotorController_SendData(MotorID, 0xE1,450000);

		/*
		 * PID and Closed-loop Registers (0x63)
		 * ENC_VMEAN_WAIT=100
		 * ENC_VMEAN_FILTER=11
		 * ENC_VMEAN_INT=512
		 */
		MotorController_SendData(MotorID, 0xE3,0x2000b64);

		/*
		 * ENC_IN_CONF (0x07)
		 * regulation_modus = 1 => Close-loop operation is enable
		 * cl_vlimit_en = 1 => Catch-up velocity during closed-loop operation is limited by
		 * 					   internal PI regulator
		 */
		MotorController_SendData(MotorID, 0x87,0x28400000);

		/*
		 * SCALE_VALUES (0x06)
		 * CL_IMAX = 255 => Closed-loop maximum scaling value
		 * CL_IMIN = 100 => Closed-loop minimum scaling value
		 * CL_START_UP = 100 => |ENC_POS_DEV| value at which closed-loop scaling increase the current
		 *  					 value above CL_IMIN
		 */
		MotorController_SendData(MotorID, 0x86,0xFFFFFF);//0x0032F080 CL_IMAX = 240  CL_IMIN = 128 CL_START_UP = 50

		/*
		 * Various Scaling Configuration Registers (0x18)
		 * CL_UPSCALE_DELAY = 1000 clock cycles
		 */
		MotorController_SendData(MotorID, 0x98,0);

		/*
		 * Various Scaling Configuration Registers (0x19)
		 * CL_DNSCALE_DELAY = 100000 clock cycles
		 */
		MotorController_SendData(MotorID, 0x99,0);

		/*
		 * CURRENT_CONF (0x05)
		 * closed_loop_scale_en = 1 => Enable Closed-loop scaling
		 */
		MotorController_SendData(MotorID, 0x85,0x00000080);

		/*
		 * Encoder registers (0x52)
		 * CL_TR_TOLERANCE = 60 => Tolerated absolute tolerance between XACTUAL and ENC_POS
		 * 						   to trigger TARGET_REACHED
		 */
		MotorController_SendData(MotorID, 0xD2,0x0000003C);

		/*
		 * Ramp Generator Registers (0x20)
		 * Operation mode = 1 => Positioning Mode
		 * Motion profile = 1 => Trapezoidal ramp
		 */
		MotorController_SendData(MotorID, 0xA0,0x00000005);

		/*
		 * Ramp Generator Registers (0x24)
		 * VMAX = 51200 pps
		 */
		MotorController_SendData(MotorID, 0xA4,0xF0000000);//0x0F000000

		/*
		 * Ramp Generator Registers (0x28)
		 * AMAX = 16000 pps => 0x3E80
		 */
		MotorController_SendData(MotorID, 0xA8,150000);//16000//35000//55000

		/*
		 * Ramp Generator Registers (0x29)
		 * DMAX = 10000 pps => 0x2710
		 */
		MotorController_SendData(MotorID, 0xA9,150000);//10000//30000//55000
		DEBUG_MOTORCONTROLLER("Calibrate Motor %d: %d\n",MotorID,MotorController_GetCurrentStepPosition(MotorID));

	}

}
//static void MotorController_InitOpenLoopControl(uint8_t MotorID)
//{
//	//initialize motor
//	MotorController_SendData(MotorID, 0xCF,0x52535400); // reset squirrel
//
//	/*=============SPI_OUT_CONF(Add = R: 0x04 - W: 0x84): = 0x4440006B=================
//    +SPI_OUT_LOW_TIME (SPI output transfer idle state duration)                   : 4
//    +SPI_OUT_HIGH_TIME (SPI output clock high level duration)                     : 4
//    +SPI_OUT_LOW_TIME (SPI output clock low level duration)                       : 4
//    +COVER_DATA_LENGTH (Datagram length. Set to 0 for TMCx drivers)               : 0
//    +POLL_BLOCK_MULT (Exponent for idle state time between two polling datagrams) : 0
//    +disable_polling(No transfer of polling datagrams) = A
//    +scale_vale_transfer_en(Transfer of scaling values in scaling values alter) = B
//             4 (AB = 00) 2(AB = 10) 4(AB = 01) 6(AB = 11)                         : 6
//    +spi_output_format(Selection of connected driver/DAC)                         : B
//             A: 26x-SPI, B: 26x-S/D
//	 */
//
//	MotorController_SendData(MotorID, 0x84,0x4440006B);
//
//	/*=============CLK_FREQ(Add = R: 0x31 - W: 0xB1): = 0x00B71B00====================
//    + CLK_FREQ(Frequence): 0x007A1200 - 8Mhz
//                           0x00B71b00 - 12Mhz
//                           0x00F42400 -16Mhz
//                           0x01036640 -17Mhz
//	 */
//	MotorController_SendData(MotorID, 0xB1,0x00B71B00);
//
//	/*==============GENERAL_CONF(Add = R: 0x80 - W: 0x00):= 0x00006026=================
//    Direct-a, direct - bow
//	 */
//	MotorController_SendData(MotorID, 0x80,0x00006026);
//
//	/*================REFERENCE_CONF(Add = R: 0x90 - W: 0x01): =0x00010001 ============
//    StepLength
//	 */
//	MotorController_SendData(MotorID, 0x90,0x00010001);
//
//	/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x00000000================
//    DRV_STATUS response and COVER_LOW send
//
//    Using cover datagram to write to DRVCTRL of TMC26x  sendData   (0xEC,0x00000000)
//    0x00000000: single edge steps, disable step interpolation - 256 microstep
//	 */
//	MotorController_SendData(MotorID, 0xEC,0x00000000);
//
//	/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x00091935================
//    DRV_STATUS response and COVER_LOW send
//
//    Using cover datagram to write to CHOPCONF of TMC26x        (0xEC,0x00090585)
//    tbl=36, standard chopper, HDEC=16, HEND=11, HSTR=1, TOFF=5, RNDTF=off
//	 */
//	MotorController_SendData(MotorID, 0xEC,0x00091935);
//
//	/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000A0000================
//    DRV_STATUS response and COVER_LOW send
//
//    Using cover datagram to write to SMARTEN of TMC26x (0xEC,0x000A0000)
//    tbl=36, standard chopper, HDEC=16, HEND=11, HSTR=1, TOFF=5, RNDTF=off
//	 */
//	MotorController_SendData(MotorID, 0xEC,0x000A0000);
//
//	/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000D05xx================
//    DRV_STATUS response and COVER_LOW send
//
//    Using cover datagram to write to SGCSCONF of TMC26x (0xEC,0x000D05xx)
//    Disable Smarten Register, SGT=0,CS=24
//	 */
//	if(MotorID == 1)
//	{
//		// Disable Smarten Register
//		MotorController_SendData(MotorID, 0xEC,0x000D0519); // Using cover datagram to write to SGCSCONF of TMC26x 06 for small motor                   (0xEC,0x000C0018)
//	}
//	else
//	{
//		// Disable Smarten Register
//		MotorController_SendData(MotorID, 0xEC,0x000D0506); // Using cover datagram to write to SGCSCONF of TMC26x 06 for small motor                   (0xEC,0x000C0018)
//	}
//
//	/*==============COVER_LOW(Add = R: 0xEC - W: 0x6C):= 0xEC,0x000EF040==============
//    DRV_STATUS response and COVER_LOW send
//
//    Using cover datagram to write to DRVCONF of TMC26x     (0xEC,0x000EF000)
//    SLPH = 3, SLPL = 3, DISS2G = off, TS2G = 0-3.2µs, SDOFF = on, VSENSE = 0
//	 */
//	MotorController_SendData(MotorID, 0xEC,0x000EF040); // Using cover datagram to write to DRVCONF of TMC26x                     (0xEC,0x000EF000)
//
//	/*
//	 * Ramp Generator Registers (0x20)
//	 * Operation mode = 1 => Positioning Mode
//	 * Motion profile = 1 => Trapezoidal ramp
//	 */
//	MotorController_SendData(MotorID, 0xA0,0x00000005);
//
//	/*
//	 * Ramp Generator Registers (0x24)
//	 * VMAX = 51200 pps
//	 */
//	MotorController_SendData(MotorID, 0xA4,0x09000000);
//
//	/*
//	 * Ramp Generator Registers (0x28)
//	 * AMAX = 16000 pps => 0x3E80
//	 */
//	MotorController_SendData(MotorID, 0xA8,10000);//16000
//
//	/*
//	 * Ramp Generator Registers (0x29)
//	 * DMAX = 10000 pps => 0x2710
//	 */
//	MotorController_SendData(MotorID, 0xA9,10000);//10000
//
//	uint8_t sameValueCounter=0;
//	int32_t prevStep=0;
//	int32_t step=0;
//	while(sameValueCounter<3)
//	{
//		/*
//		 * Target and Compare Registers (0x37)
//		 * XTARGET = step
//		 */
//		step-=10;
//		MotorController_SendData(MotorID, 0xB7,step);
//		int32_t currentStep = MotorController_GetCurrentStepPosition(MotorID);
//		if(abs(currentStep-prevStep)<=5)
//		{
//			sameValueCounter++;
//		}
//		else
//		{
//			sameValueCounter=0;
//			prevStep = currentStep;
//		}
//		delay_ms(6);
//	}
//
//	DEBUG_MOTORCONTROLLER("HFO Motor Go To Zero %d: %d\n",MotorID,MotorController_GetCurrentStepPosition(MotorID));
//	/*
//	 * Ramp Generator Registers (0x24)
//	 * VMAX = 0 pps
//	 */
//	MotorController_SendData(MotorID, 0xA4,0);
//
//	/*
//	 * Ramp Generator Registers (0x21)
//	 * XACTUAL = 0 pps
//	 */
//	MotorController_SendData(MotorID, 0xA1,0);
//	//	/*==============VMAX(Add = R: 0x24 - W: 0xA4):= 0xA4,0x00000000================
//	//    Actual ramp generator position- x = 0
//	//	 */
//	//	MotorController_SendData(MotorID, 0xA4,0x00000000);
//	//
//	//	/*==============XACTUAL(Add = R: 0x21 - W: 0xA1):= 0xA4,0x00000000================
//	//    Target ramp generator velocity- v = 0
//	//	 */
//	//	MotorController_SendData(MotorID, 0xA1,0x00000000);
//	//
//	//	/*==============VSTART(Add = R: 0x37 - W: 0xB7):= 0xB7,0x00000000================
//	//    Start velocity of ramp generator- v = 0
//	//	 */
//	//	MotorController_SendData(MotorID, 0xB7,0x00000000);
//
//	/*==============RAMPMODE(Add = R: 0x20 - W: 0xA0):= 0xA0,0x00000005================
//      bit 2: Ramp_mode
//              1 Positioning: XTARGET is superior objective for velocity ramp
//              0 Velocitiy mode: VMAX is superior objective for velocity ramp
//      bit 1 - 0: Ramp_type
//              00 Hold mode: |-|
//              01 Trapezoidal ramp /-\
//              10 S-shaped ramp /'-'\
//
//	 */
//	MotorController_SendData(MotorID, 0xA0,0x00000005);
//
//	/*==============VMAX(Add = R: 0x24 - W: 0xA4):= 0xA0, 0x03E80000================
//      03E80000 200K
//      0x01860000    100k
//	 */
//	MotorController_SendData(MotorID, 0xA4,0x01860000); // VMAX =  [pps] 03E80000 200K / 186a0=100k
//
//	/*==============AMAX(Add = R: 0x28 - W: 0xA8):= 0xA0, 0x00001000================
//	 */
//	MotorController_SendData(MotorID, 0xA8,0x00001000); // AMAX
//
//	/*==============DMAX(Add = R: 0x29 - W: 0xA9):= 0xA0, 0x00001000================
//	 */
//	MotorController_SendData(MotorID, 0xA9,0x00001000); // DMAX
//}

/**
 * Initialize Motor controller by config SPI communication, GPIO
 * and config closed loop control for all Motors
 */
void MotorController_Init()
{
	spi_HwInit();
	spi_RtosInit();
	gpio_pin_config_t cs_config_hfo = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	GPIO_PinInit(CS_HFO_PORT, CS_HFO_PIN, &cs_config_hfo);
	GPIO_PinWrite(CS_HFO_PORT, CS_HFO_PIN, 1U);

	gpio_pin_config_t cs_config_exh = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	GPIO_PinInit(CS_EXH_PORT, CS_EXH_PIN, &cs_config_exh);
	GPIO_PinWrite(CS_EXH_PORT, CS_EXH_PIN, 1U);

	gpio_pin_config_t cs_config_o2 = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	GPIO_PinInit(CS_O2_PORT, CS_O2_PIN, &cs_config_o2);
	GPIO_PinWrite(CS_O2_PORT, CS_O2_PIN, 1U);

	gpio_pin_config_t cs_config_air = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	GPIO_PinInit(CS_AIR_PORT, CS_AIR_PIN, &cs_config_air);
	GPIO_PinWrite(CS_AIR_PORT, CS_AIR_PIN, 1U);

	MotorController_InitClosedLoopControl(eHFOMotor);
	MotorController_InitClosedLoopControl(eExhMotor);
	MotorController_MoveToStepPosition(eExhMotor,0);
	MotorController_InitClosedLoopControl(eAirMotor);
	MotorController_MoveToStepPosition(eAirMotor,0);
	MotorController_InitClosedLoopControl(eO2Motor);
	MotorController_MoveToStepPosition(eO2Motor,0);
	MotorController_RampToHFOClosePosition();
}
/**
 * Move HFO motor to close position
 */
void MotorController_RampToHFOClosePosition()
{
	uint8_t sameValueCounter=0;
	int32_t prevStep=0;
	int32_t step=MotorController_GetCurrentStepPosition(eHFOMotor);
	while(sameValueCounter<10)
	{
		/*
		 * Target and Compare Registers (0x37)
		 * XTARGET = step
		 */
		step+=20;
		MotorController_MoveToStepPosition(eHFOMotor,step);
		int32_t currentStep = MotorController_GetCurrentStepPosition(eHFOMotor);
		if(abs(currentStep-prevStep)<=5)
		{
			sameValueCounter++;
		}
		else
		{
			sameValueCounter=0;
			prevStep = currentStep;
		}
		delay_ms(4);
	}
}
/**
 * Get current step position of specified Motor
 * @param MotorID Specify which Motor to get step position
 * @return int32_t step Step position (in scale 12800)
 */
int32_t MotorController_GetCurrentStepPosition(E_MOTORID MotorID)
{
	uint32_t datagram=0;
	MotorController_ReadData(MotorID, 0x50,&datagram);
	datagram=0;
	MotorController_ReadData(MotorID, 0x50,&datagram);
	datagram&=0xffffffff;
	int32_t step=0;
	switch(MotorID)
	{
	case eAirMotor:
		gs_CurrentAirStep=-datagram;
		step = -datagram;
		break;
	case eO2Motor:
		gs_CurrentO2Step=-datagram;
		step = -datagram;
		break;
	case eExhMotor:
		gs_CurrentExhStep=-datagram;
		step = -datagram;
		break;
	case eHFOMotor:
		gs_CurrentHFOStep=-datagram;
		step = -datagram;
		break;
	default:
		break;
	}
	return step;

}

/**
 * Get previously obtained reading step position of specified Motor
 * @param MotorID Specify which Motor to get step position
 * @return int32_t step Step position (in scale 12800)
 */
int32_t MotorController_GetLastStepPosition(E_MOTORID MotorID)
{
	int32_t step = 0;
	switch(MotorID)
	{
	case eAirMotor:
		step=gs_CurrentAirStep;
		break;
	case eO2Motor:
		step=gs_CurrentO2Step;
		break;
	case eExhMotor:
		step=gs_CurrentExhStep;
		break;
	case eHFOMotor:
		step=gs_CurrentHFOStep;
		break;
	default:
		break;
	}
	return step;
}
/**
 * Send command to Motor Controller Chip to move specified Motor
 * to desired step position (in scale 12800)
 * @param MotorID Specify which Motor to control
 * @param stepPosition Step position (in scale 12800)
 */
void MotorController_MoveToStepPosition(E_MOTORID MotorID,int32_t stepPosition)
{
	switch(MotorID)
	{
	case eAirMotor:
		gs_DesiredAirStep=-stepPosition;
		MotorController_SendData(MotorID, 0xB7,gs_DesiredAirStep);
		break;
	case eO2Motor:
		gs_DesiredO2Step=-stepPosition;
		MotorController_SendData(MotorID, 0xB7,gs_DesiredO2Step);
		break;
	case eExhMotor:
		gs_DesiredExhStep=-stepPosition;
		MotorController_SendData(MotorID, 0xB7,gs_DesiredExhStep);
		break;
	case eHFOMotor:
		gs_DesiredHFOStep=-stepPosition;
		MotorController_SendData(MotorID, 0xB7,gs_DesiredHFOStep);
		break;
	default:
		break;
	}
}
/**
 * Send command to Motor Controller Chip to move specified Motor
 * to desired step position (in scale 2200)
 * @param MotorID Specify which Motor to control
 * @param stepPosition Step position (in scale 2200)
 */
void MotorController_MoveToStepPositionOld(E_MOTORID MotorID,int32_t stepPosition)
{

	float percent = ((float)stepPosition/(float)MAX_STEP_POSITION_OLD);
	switch(MotorID)
	{
	case eAirMotor:
		gs_DesiredAirStep=-percent*(float)MAX_STEP_POSITION_CURRENT;
		MotorController_SendData(eAirMotor, 0xB7,gs_DesiredAirStep);
		break;
	case eO2Motor:
		gs_DesiredO2Step=-percent*(float)MAX_STEP_POSITION_CURRENT;
		MotorController_SendData(eO2Motor, 0xB7,gs_DesiredO2Step);
		break;
	case eExhMotor:
		gs_DesiredExhStep=-percent*(float)MAX_STEP_POSITION_CURRENT;
		MotorController_SendData(eExhMotor, 0xB7,gs_DesiredExhStep);
		break;
	case eHFOMotor:
		gs_DesiredHFOStep=-percent*(float)MAX_STEP_POSITION_CURRENT;
		MotorController_SendData(eHFOMotor, 0xB7,gs_DesiredHFOStep);
		break;
	default:
		break;
	}
}
/**
 * Get Lift Off value of specified Motor
 * @param MotorID Specify which Motor to get Lift Off value
 * @return LiftOff Lift Off value (in scale 2200)
 */
unsigned short MotorController_GetLiftOff(E_MOTORID MotorID)
{
	unsigned short LiftOff = 0;
	switch(MotorID)
	{
	case eAirMotor:
		LiftOff = gs_LiftOffAirStep;
		break;
	case eO2Motor:
		LiftOff = gs_LiftOffO2Step;
		break;
	case eExhMotor:
		LiftOff = gs_LiftOffExhStep;
		break;
	case eHFOMotor:
		LiftOff = gs_LiftOffHFOStep;
		break;
	default:
		break;
	}
	return LiftOff;
}

/**
 * Get max step position of specified Motor
 * @param MotorID Specify which Motor to get max step position
 * @return int32_t MaxStep Max step position (in scale 2200)
 */
int32_t MotorController_GetMaxStep(E_MOTORID MotorID)
{
	int32_t MaxStep = 0;
	switch(MotorID)
	{
	case eAirMotor:
		MaxStep = gs_MaxAirStep;
		break;
	case eO2Motor:
		MaxStep = gs_MaxO2Step;
		break;
	case eExhMotor:
		MaxStep = gs_MaxExhStep;
		break;
	case eHFOMotor:
		MaxStep = gs_MaxHFOStep;
		break;
	default:
		break;
	}
	return MaxStep;
}

/**
 * Set Lift Off value of specified Motor
 * @param MotorID Specify which Motor to set Lift Off value
 * @param step Lift Off value (in scale 2200)
 */
void MotorController_SetLiftOff(E_MOTORID MotorID,int32_t step)
{
	switch(MotorID)
	{
	case eAirMotor:
		gs_LiftOffAirStep=step;
		break;
	case eO2Motor:
		gs_LiftOffO2Step=step;
		break;
	case eExhMotor:
		gs_LiftOffExhStep=step;
		break;
	case eHFOMotor:
		gs_LiftOffHFOStep=step;
		break;
	default:
		break;
	}
}

/**
 * Set HFO frequency
 * @param value HFO frequency in Hz
 */
void MotorController_SetHFOFreq(int32_t value)
{
	gs_Freq = value;
}

/**
 * Set HFO Stroke volume
 * @param value HFO stroke volume
 */
void MotorController_SetHFOStrokeVolume(int32_t value)
{
	gs_StrokeVolume = value;
}

/**
 * Set HFO Offset step
 * @param value HFO offset step (in scale 2200)
 */
void MotorController_SetHFOOffSetPosition(int32_t value)
{
	gs_OffSetPosition = value;
}
/**
 * Handle HFO motor in hardware diagnostic mode
 */
void MotorController_RunHFOMode()
{
	int position = 0;
	if (gs_OffSetCurrentPosition < gs_OffSetPosition)
		gs_OffSetCurrentPosition++;
	else if (gs_OffSetCurrentPosition > gs_OffSetPosition)
		gs_OffSetCurrentPosition--;

	gs_T = 1.0/(double)gs_Freq*1000; //(ms)
	if(gs_t>=gs_T)
		gs_t=0;
	position =((double)(gs_StrokeVolume)/2.0)*sin((2.0*PI*gs_t)/gs_T) +
			(double)(gs_StrokeVolume)/2.0 + gs_OffSetCurrentPosition;

	MotorController_MoveToStepPositionOld(eHFOMotor,MAX_STEP_POSITION_OLD-position);
	gs_t=gs_t+2;

	return;
}




