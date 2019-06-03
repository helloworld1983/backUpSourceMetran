/********************************************************************/
/*                                                                  */
/* File Name     : PsaSelectMgr.c		                            */
/*                                                                  */
/* General       : handle the psa select paramter based on 			*/
/* 				   temperature and O2 condition				     	*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/* 						                                            */
/* Company		 : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/10/04      Quyen Ngo  	new file       		    */
/*																	*/
/* #0001	   2017/09/01	   Quyen Ngo	modify		 	        */
/*		-remove s_current02, passing value direct to 				*/
/* 		psaSelect_CheckO2function(bug 1795)   						*/
/********************************************************************/
/* include system files */

/* include user files */

#include "PsaSelectMgr.h"
#include "TaskCommon.h"
#include "gpio.h"
#include "Compressor.h"

/* constants, macro definition */

#ifdef DEBUG
#define DEBUG_PSASLT_EN
#endif

#ifdef DEBUG_PSASLT_EN
#define DEBUG_PSASLT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PSASLT(...)
#endif

#define PSA_SELECT_20MIN_TIME 	20*60000/DEVICE_TASK_DELAY_TIME //(20min = 20*60000ms)
#define PSA_SELECT_3MIN_TIME 	3*60000/DEVICE_TASK_DELAY_TIME //(3min = 3*60000ms)
#define PSA_SELECT_15MIN_TIME 	15*60000/DEVICE_TASK_DELAY_TIME //(15min = 15*60000ms)
#define O2_THRESOLD 				9000 // 90% scale 100
#define HIGH_TEMP_THRESHOLD_MAX		60
#define HIGH_TEMP_THRESHOLD_MIN		25
#define LOW_TEMP_THRESHOLD_MAX		20
#define O2_THRESHOLD_MAX			95
#define O2_THRESHOLD_MIN			85

uint16_t s_psaHighTempThreShold = 43;
uint16_t s_psaLowTempThreShold = 15;
uint16_t s_O2ThreShold = 9000;
/* definitions of structures */

/* external decralation */

static E_PsaSelectName s_psaSelectType = NORM_TEMP_NORM_02;
static E_TempertureType s_tempType = NORM_TEMP;
static E_O2Type s_o2Type = NORM_O2;

static bool s_IsAllowChangeO2Type = false;
static bool s_IsProcessCnt20Min = false;

static bool s_isAllowCheckTemp = false;

static uint32_t s_20minCnt = 0;

static uint32_t s_timeLow15minCnt = 0;
static uint32_t s_timeHigh15minCnt = 0;
static uint32_t s_timeNormal15minCnt = 0;

static float s_curentTemp = 0.0;

/* Body of functions */

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_Init
//
//    Detail:Init and config psa select port
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void psaSelect_Init(void)
{
	//Fan error GPIO config
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, PSA_SELECT_PORT_NUM, PSA_SELECT_BIT_NUM);		/* input */
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_SetType
//
//    Detail: Set psa selection type depend on temperature type and o2 tyep
//
//    Arguments: (I)E_TempertureType tempType, E_O2Type o2Type
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
static void psaSelect_SetType(E_TempertureType tempType,
		E_O2Type o2Type)
{
	switch (tempType) {
	case HIGH_TEMP:
		if (o2Type == NORM_O2)
		{
			s_psaSelectType = HIGH_TEMP_NORM_02;
			DEBUG_PSASLT("HIGH_TEMP_NORM_02\n");
		}
		else if (o2Type == LOW_O2)
		{
			s_psaSelectType = HIGH_TEMP_LOW_02;
			DEBUG_PSASLT("HIGH_TEMP_LOW_02\n");
		}
		break;
	case NORM_TEMP:
		if (o2Type == NORM_O2)
		{
			s_psaSelectType = NORM_TEMP_NORM_02;
			DEBUG_PSASLT("NORM_TEMP_NORM_02\n");
		}
		else if (o2Type == LOW_O2)
		{
			s_psaSelectType = NORM_TEMP_LOW_02;
			DEBUG_PSASLT("NORM_TEMP_LOW_02\n");
		}
		break;
	case LOW_TEMP:
		if (o2Type == NORM_O2)
		{
			s_psaSelectType = LOW_TEMP_NORM_02;
			DEBUG_PSASLT("LOW_TEMP_NORM_02\n");
		}
		else if (o2Type == LOW_O2)
		{
			s_psaSelectType = LOW_TEMP_LOW_02;
			DEBUG_PSASLT("LOW_TEMP_LOW_02\n");
		}
		break;
	default:
		break;
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_ChekTemp
//
//    Detail: Check temperature
//
//    Arguments: (I) float temperature
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
static void psaSelect_CheckTemp(float temperature)
{
	if (s_isAllowCheckTemp == true)
	{
		if(temperature < s_psaLowTempThreShold)
		{
			s_timeHigh15minCnt = 0;
			s_timeNormal15minCnt = 0;
			s_timeLow15minCnt++;
			if(s_timeLow15minCnt >= PSA_SELECT_15MIN_TIME)
			{
				DEBUG_PSASLT("LOW TEMP\n");
				s_timeLow15minCnt = 0;
				s_tempType = LOW_TEMP;
				psaSelect_SetType(s_tempType,s_o2Type);
				compressor_setTcomHspTime(s_tempType);
			}
		}
		else if (temperature > s_psaHighTempThreShold)
		{
			s_timeLow15minCnt = 0;
			s_timeNormal15minCnt = 0;
			s_timeHigh15minCnt++;
			if(s_timeHigh15minCnt >= PSA_SELECT_15MIN_TIME)
			{
				DEBUG_PSASLT("HIGH TEMP: %d\n",s_timeHigh15minCnt);
				s_timeHigh15minCnt = 0;
				s_tempType = HIGH_TEMP;
				psaSelect_SetType(s_tempType,s_o2Type);
				compressor_setTcomHspTime(s_tempType);
			}
		}
		else
		{
			s_timeLow15minCnt = 0;
			s_timeHigh15minCnt = 0;
			s_timeNormal15minCnt++;
			if(s_timeNormal15minCnt >= PSA_SELECT_15MIN_TIME)
			{
				DEBUG_PSASLT("NORM TEMP\n");
				s_timeNormal15minCnt = 0;
				s_tempType = NORM_TEMP;
				psaSelect_SetType(s_tempType,s_o2Type);
				compressor_setTcomHspTime(s_tempType);
			}
		}
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_CheckPort
//
//    Detail: Check Psa select port
//
//    Arguments: (I) float temperature
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
E_PsaPortSelect psaSelect_CheckPort(void)
{
	if(Chip_GPIO_GetPinState(LPC_GPIO_PORT, PSA_SELECT_PORT_NUM, PSA_SELECT_BIT_NUM) == true)
	{
		return DEFAULT_SELECT;
	}
	else
	{
		return EEPROM_SELECT;
	}
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_CheckFirstTemp
//
//    Detail: Check temperature
//
//    Arguments: (I) float temperature
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
static void psaSelect_CheckFirstTemp(float temperatture)
{
	s_isAllowCheckTemp = true;
	if(temperatture >= s_psaHighTempThreShold)
	{
		s_tempType = HIGH_TEMP;
	}
	else if(temperatture <= s_psaLowTempThreShold)
	{
		s_tempType = LOW_TEMP;
	}
	else
	{
		s_tempType = NORM_TEMP;
	}
	compressor_setTcomHspTime(s_tempType);
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_ChekO2
//
//    Detail: Check O2 %
//
//    Arguments: (I) uint16_t o2Percent
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
static void psaSelect_CheckO2(uint16_t  o2Percent)
{
	static uint32_t time3minCnt = 0;
	if(s_IsAllowChangeO2Type == true)
	{
		if(o2Percent < s_O2ThreShold)
		{
			time3minCnt++;
			if(time3minCnt >= PSA_SELECT_3MIN_TIME)
			{
				time3minCnt = 0;
				s_o2Type = LOW_O2;
				s_IsAllowChangeO2Type = false;
				psaSelect_SetType(s_tempType,s_o2Type);
			}
		}
		else
		{
			time3minCnt = 0;
		}
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_Count20MinTime
//
//    Detail: Count 20min when actually jump to psa start sequence or chane flow rate
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I) gs_IsProcessCnt20Min
//						  (O) gs_IsAllowChangeO2Type
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
static void psaSelect_Count20MinTime(void)
{
	if (s_IsProcessCnt20Min == true)
	{
		s_20minCnt++;
		if(s_20minCnt >= PSA_SELECT_20MIN_TIME)
		{
			s_20minCnt = 0;
			s_IsAllowChangeO2Type = true;
			s_IsProcessCnt20Min = false;
		}
	}
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_Count20MinTime
//
//    Detail: Count 20min when actually jump to psa start sequence or chane flow rate
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (O) s_IsProcessCnt20Min
//						  (O) s_20minCnt
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void psaSelect_StartCheckO2Condition(void)
{
	s_20minCnt = 0;
	s_IsProcessCnt20Min = true;
	s_IsAllowChangeO2Type = false;
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_Handling
//
//    Detail: Handling psa selection type
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void psaSelect_Handling(float temperature, uint16_t o2Percent)
{
	s_curentTemp = temperature;
	psaSelect_Count20MinTime();
	psaSelect_CheckO2(o2Percent);
	psaSelect_CheckTemp(s_curentTemp);
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_GetConditionAtStart
//
//    Detail: Start check condition
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
E_PsaSelectName psaSelect_GetConditionAtStart(void)
{
	s_o2Type = NORM_O2;
	s_timeLow15minCnt = 0;
	s_timeHigh15minCnt = 0;
	s_timeNormal15minCnt = 0;
	psaSelect_CheckFirstTemp(s_curentTemp);
	psaSelect_SetType(s_tempType,s_o2Type);
	psaSelect_StartCheckO2Condition();
	return s_psaSelectType;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_GetTypeName
//
//    Detail: Start check condition
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: PsaSelectName
//
//    Using Global Data : (I) s_psaSelectType
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
E_PsaSelectName psaSelect_GetConditionType(void)
{
	return s_psaSelectType;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_StopCheckCondition
//
//    Detail: Stop check condition
//
//    Arguments: (I)
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void psaSelect_StopCheckCondition(void)
{
	s_isAllowCheckTemp = false;
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_ChangeHighTempThreshold
//
//    Detail: Change high temp threshold
//
//    Arguments: (I) uint16_t temp
//				 Range: 25-60 degC
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void psaSelect_ChangeHighTempThreshold(uint16_t temp)
{
	if((temp > HIGH_TEMP_THRESHOLD_MAX) || (temp < HIGH_TEMP_THRESHOLD_MIN))
	{
		DEBUGOUT("High temp Out of range \n");
		return;
	}
	s_psaHighTempThreShold = temp;
	DEBUG_PSASLT("temp threshold %d \n",s_psaHighTempThreShold);
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_ChangeLowTempThreshold
//
//    Detail: Change low temp threshold
//
//    Arguments: (I) uint16_t temp
//				 Range: 0-20 degC
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void psaSelect_ChangeLowTempThreshold(uint16_t temp)
{
	if(temp > LOW_TEMP_THRESHOLD_MAX)
	{
		DEBUGOUT("Low temp Out of range \n");
		return;
	}
	s_psaLowTempThreShold = temp;
	DEBUG_PSASLT("temp threshold %d \n",s_psaLowTempThreShold);
	return;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Function Name: psaSelect_ChangeO2Threshold
//
//    Detail: Change O2 threshold
//
//    Arguments: (I) uint16_t O2
//				 Range: 8500-9500 Scale 100
//				 (O)
//
//    Return Values: None
//
//    Using Global Data : (I)
//						  (O)
//
//    Local variables: None
//
//    Note: None
//
/******************************************************************************/
void psaSelect_ChangeO2Threshold(uint16_t O2)
{
	if((O2 > O2_THRESHOLD_MAX) || (O2 < O2_THRESHOLD_MIN))	{
		DEBUGOUT("O2 threshold Out of range \n");
		return;
	}
	s_O2ThreShold = O2*100;//Scale 100
	DEBUG_PSASLT("O2 threshold %d \n",s_O2ThreShold);
	return;
}

