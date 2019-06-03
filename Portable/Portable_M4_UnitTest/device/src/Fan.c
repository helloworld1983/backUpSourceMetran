/********************************************************************/
/*                                                                  */
/* File Name    : Fan.c                        				        */
/*                                                                  */
/* General      : Initiallize Fan									*/
/*                Control FAN voltage                                */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*       December 22, 2016     Thao Ha(MV)  		new file        */
/*                                                                  */
/********************************************************************/
#include <Gpio.h>
#include <PsaCtrl.h>
#include <TaskCommon.h>
#include "Fan.h"

#define HIGH_TEMPERATURE 60
#define LOW_TEMPERATURE 45
#define UPDATE_TEMPERATURE_LOOP_CNT 1000/DEVICE_TASK_DELAY_TIME
#define MAX_NUMBER_OF_SAMPLES 30
bool gs_isFanShutDown = false;
E_FanVoltage gs_fanVoltage = OFF;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_ShutDown						              				    */
/*                                                                                      */
/* Details:        Shutdown Fan															*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/* Local variables : None				                          						*/
/*                                                                                      */
/****************************************************************************************/
void fan_ShutDown(void)
{
	gs_isFanShutDown = true;
	fan_SetVoltage(OFF);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_SetVoltage						              				    */
/*                                                                                      */
/* Details:       Set Voltage supplied for Fan											*/
/*                                                                                      */
/* Arguments:	(I) E_FanVoltage voltage												*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void fan_SetVoltage(E_FanVoltage voltage)
{
	switch(voltage)
	{
	case OFF:
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,false);
		gs_fanVoltage = OFF;
		break;
	case OPERATE_5V:
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,true);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,false);
		gs_fanVoltage = OPERATE_5V;
		break;
	case OPERATE_9V:
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,true);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,false);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,true);
		gs_fanVoltage = OPERATE_9V;
		break;
	case OPERATE_12V:
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,true);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,true);
		Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,false);
		gs_fanVoltage = OPERATE_12V;
		break;
	default:
		break;
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_GetFanVoltage							              			*/
/*                                                                                      */
/* Details:       get current voltage of FAN											*/
/*                                                                                      */
/* Arguments:	NONE																	*/
/*                                                                                      */
/* ReturnValue : E_FanVoltage			                                                */
/*                                                                                      */
/****************************************************************************************/
E_FanVoltage fan_GetFanVoltage()
{
	return gs_fanVoltage;
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_Init							              				    */
/*                                                                                      */
/* Details:       Initiallize GPIO for control FAN										*/
/*                                                                                      */
/* Arguments:	NONE																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void fan_Init()
{
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,FAN_EN_PORT_NUM, FAN_EN_BIT_NUM);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,FAN_9V_PORT_NUM, FAN_9V_BIT_NUM);
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT,FAN_12V_PORT_NUM, FAN_12V_BIT_NUM);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_EN_PORT_NUM, FAN_EN_BIT_NUM,false);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_9V_PORT_NUM, FAN_9V_BIT_NUM,false);
	Chip_GPIO_WritePortBit(LPC_GPIO_PORT, FAN_12V_PORT_NUM, FAN_12V_BIT_NUM,false);
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_UpdateFanVoltage				              				    */
/*                                                                                      */
/* Details:        Update FAN voltage base on current compressor temperature			*/
/*                                                                                      */
/* Arguments:	(I)uint16_t avrTemperature												*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
void fan_UpdateFanVoltage(uint16_t avrTemperature)
{
    if(gs_isFanShutDown==false)
    {
        if(psa_isThreeSecondFinish()==false)
        {
            fan_SetVoltage(OPERATE_12V);
        }
        else
        {
            if(psa_isWarmUpFinish()==false)
            {
                if(avrTemperature<=HIGH_TEMPERATURE)
                {
                    fan_SetVoltage(OPERATE_9V);
                }
                else
                {
                    fan_SetVoltage(OPERATE_12V);
                }
            }
            else
            {
                if(avrTemperature<=LOW_TEMPERATURE)
                {
                    fan_SetVoltage(OPERATE_5V);
                }
                else
                {
                    if((avrTemperature>LOW_TEMPERATURE)&&(avrTemperature<=HIGH_TEMPERATURE))
                    {
                        fan_SetVoltage(OPERATE_9V);
                    }
                    else
                    {
                        fan_SetVoltage(OPERATE_12V);
                    }
                }
            }
        }
    }
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	fan_Handle								              				*/
/*                                                                                      */
/* Details:    	fan handle function is call for update Fan voltage base on current		*/
/* 				temperature every 1s													*/
/*                                                                                      */
/* Arguments:	(I) uint16_t temperature												*/
/*                                                                                      */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void fan_Handle(uint16_t temperature)
{
	static uint16_t s_tempList[30];
	static uint16_t s_cnt = 0;
	static uint16_t s_avrTemperature = 0;
	static uint8_t s_cntSec  = 0;
	static bool s_isFirst30SLast=false;
	if(s_cnt==UPDATE_TEMPERATURE_LOOP_CNT)
	{
		if(s_cntSec == MAX_NUMBER_OF_SAMPLES)
		{
			s_isFirst30SLast=true;
			s_cntSec = 0;
		}
		s_tempList[s_cntSec] = temperature;
		s_avrTemperature=0;
		double devideValue = 0;
		uint8_t numberOfSample = 0;
		if(s_isFirst30SLast==false)
			numberOfSample = s_cntSec;
		else
			numberOfSample = MAX_NUMBER_OF_SAMPLES;
		for(int i=0;i<numberOfSample;i++)
		{
			if(s_tempList[i]!=0)
			{
				s_avrTemperature+=s_tempList[i];
				devideValue++;
			}
		}
		if(devideValue == 0)
		{
			s_avrTemperature = 0;
		}
		else
		{
			s_avrTemperature = (double)s_avrTemperature/devideValue;
		}
		s_cntSec++;
		s_cnt=0;
	}
	fan_UpdateFanVoltage(s_avrTemperature);
	s_cnt++;
}
