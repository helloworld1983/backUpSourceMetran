/********************************************************************/
/*                                                                  */
/* File Name    : PSATableMgr.c                       			    */
/*                                                                  */
/* General       : Write item psa parameter to EPPROM               */
/* 				   Read item psa parameter from EPPROM				*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/22      Viet Le			  	new file        */
/* #0001       2017/09/05      Viet Le              modify			*/
/*     remove #include "assert.h"                                   */
/* #0002       2017/09/12       Linh Nguyen	  	    modify          */
/*  Use new save log function                            			*/
/* #0003       2017/10/19      Viet Le	  	        modify          */
/*	Edit psa parameter (new PSA spec)								*/
/********************************************************************/

#include "stdint.h"
#include <EEPROM.h>
#include "PSATableMgr.h"
#include "setting.h"
#include "string.h"
#include "GuiTask.h"
#include "DualCoreCommon.h"
#include "GuiInterface.h"
#include "LogInterface.h"

//Normal O2 two-dimmentional array corresponding to 0.25 L
static uint8_t s_zeroAndTwoFineLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{60,0,0,0,0,0,0,0,0,0,0,1,1,40,19,5,30,100},
		{60,0,0,0,0,0,0,0,0,0,0,1,1,30,19,5,30,100},
		{60,0,0,0,0,0,0,0,0,0,0,1,1,30,19,5,30,100}//5<=>0.25L
};

//Low O2 two-dimmentional array corresponding to 0.25 L
static uint8_t s_zeroAndTwoFineLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{60,0,0,0,0,0,0,0,0,0,0,1,1,40,21,5,30,100},
		{60,0,0,0,0,0,0,0,0,0,0,1,1,30,21,5,30,100},
		{60,0,0,0,0,0,0,0,0,0,0,1,1,30,21,5,30,100}//5<=>0.25L
};

//Normal O2 two-dimmentional array corresponding to 0.5 L
static uint8_t s_zeroAndFineLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{65,1,1,1,1,1,1,1,1,0,0,2,2,40,20,10,30,100},
		{65,1,1,1,1,1,1,1,1,0,0,2,2,30,20,10,30,100},
		{65,1,1,1,1,1,1,1,1,0,0,2,2,30,22,10,30,100}//10<=>0.5L
};

//Low O2 two-dimmentional array corresponding to 0.5 L
static uint8_t s_zeroAndFineLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{65,1,1,1,1,1,1,1,1,0,0,2,2,40,22,10,30,100},
		{65,1,1,1,1,1,1,1,1,0,0,2,2,30,22,10,30,100},
		{65,1,1,1,1,1,1,1,1,0,0,2,2,30,24,10,30,100}//10<=>0.5L
};

//Normal O2 two-dimmentional array corresponding to 0.75 L
static uint8_t s_zeroAndSevenFineLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{70,1,1,1,1,1,1,1,1,0,0,2,2,40,22,15,30,100},
		{70,1,1,1,1,1,1,1,1,0,0,2,2,30,22,15,30,100},
		{70,1,1,1,1,1,1,1,1,0,0,2,2,30,24,15,30,100},//15<=>0.75L
};

//Low O2 two-dimmentional array corresponding to 0.75 L
static uint8_t s_zeroAndSevenFineLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{70,1,1,1,1,1,1,1,1,0,0,2,2,40,24,15,30,100},
		{70,1,1,1,1,1,1,1,1,0,0,2,2,30,24,15,30,100},
		{70,1,1,1,1,1,1,1,1,0,0,2,2,30,26,15,30,100},//15<=>0.75L
};

//Normal O2 two-dimmentional array corresponding to 1 L
static uint8_t s_oneLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{75,1,1,1,1,1,1,1,1,0,0,2,2,40,24,20,30,100},
		{75,1,1,1,1,1,1,1,1,0,0,2,2,30,24,20,30,100},
		{75,1,1,1,1,1,1,1,1,0,0,2,2,30,25,20,30,100}//20<=>1L
};

//Low O2 two-dimmentional array corresponding to 1 L
static uint8_t s_oneLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{75,1,1,1,1,1,1,1,1,0,0,2,2,40,26,20,30,100},
		{75,1,1,1,1,1,1,1,1,0,0,2,2,30,26,20,30,100},
		{75,1,1,1,1,1,1,1,1,0,0,2,2,30,27,20,30,100}//20<=>1L
};

//Normal O2 two-dimmentional array corresponding to 1.25 L
static uint8_t s_oneAndTwoFineLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{75,1,1,1,1,1,1,1,1,0,0,2,2,40,25,25,30,100},
		{75,1,1,1,1,1,1,1,1,0,0,2,2,30,25,25,30,100},
		{80,1,1,1,1,1,1,1,1,0,0,2,2,30,28,25,30,100},//25<=>1.25L
};

//Low O2 two-dimmentional array corresponding to 1.25 L
static uint8_t s_oneAndTwoFineLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{75,1,1,1,1,1,1,1,1,0,0,2,2,40,27,25,30,100},
		{75,1,1,1,1,1,1,1,1,0,0,2,2,30,27,25,30,100},
		{80,1,1,1,1,1,1,1,1,0,0,2,2,30,30,25,30,100},//25<=>1.25L
};

//Normal O2 two-dimmentional array corresponding to 1.5 L
static uint8_t s_oneAndFineLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{80,2,2,2,2,2,2,2,2,0,0,4,4,40,25,30,30,100},
		{80,2,2,2,2,2,2,2,2,0,0,4,4,30,25,30,30,100},
		{90,4,4,4,4,4,4,4,4,0,0,8,8,30,33,30,30,100}//30<=>1.5L
};

//Low O2 two-dimmentional array corresponding to 1.5 L
static uint8_t s_oneAndFineLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{85,2,2,2,2,2,2,2,2,0,0,4,4,40,30,30,30,100},
		{85,2,2,2,2,2,2,2,2,0,0,4,4,30,30,30,30,100},
		{95,4,4,4,4,4,4,4,4,0,0,8,8,30,38,30,30,100}//30<=>1.5L
};

//Normal O2 two-dimmentional array corresponding to 1.75 L
static uint8_t s_oneAndSevenFineLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{85,4,4,4,4,4,4,4,4,0,0,8,8,40,28,35,30,100},
		{85,4,4,4,4,4,4,4,4,0,0,8,8,30,28,35,30,100},
		{90,4,4,4,4,4,4,4,4,0,0,8,8,30,35,35,30,100},//35<=>1.75L
};

//Low O2 two-dimmentional array corresponding to 1.75 L
static uint8_t s_oneAndSevenFineLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{90,4,4,4,4,4,4,4,4,0,0,8,8,40,33,35,30,100},
		{90,4,4,4,4,4,4,4,4,0,0,8,8,30,33,35,30,100},
		{95,4,4,4,4,4,4,4,4,0,0,8,8,30,40,35,30,100},//35<=>1.75L
};

//Normal O2 two-dimmentional array corresponding to 2 L
static uint8_t s_twoLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{90,6,6,6,6,6,6,6,6,0,0,12,12,55,32,40,30,100},
		{90,6,6,6,6,6,6,6,6,0,0,12,12,45,32,40,30,100},
		{105,6,6,6,6,6,6,6,6,0,0,12,12,45,42,40,30,100}//40<=>2L
};

//Low O2 two-dimmentional array corresponding to 2 L
static uint8_t s_twoLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{95,6,6,6,6,6,6,6,6,0,0,12,12,55,38,40,30,100},
		{95,6,6,6,6,6,6,6,6,0,0,12,12,45,38,40,30,100},
		{110,6,6,6,6,6,6,6,6,0,0,12,12,45,47,40,30,100}//40<=>2L
};

//Normal O2 two-dimmentional array corresponding to 2.5 L
static uint8_t s_twoAndFineLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{100,6,6,6,6,6,6,6,6,0,0,12,12,55,43,50,30,100},
		{100,7,7,7,7,7,7,7,7,0,0,14,14,45,43,50,30,100},
		{115,7,7,7,7,7,7,7,7,0,0,14,14,45,50,50,30,100}//50<=>2.5L
};

//Low O2 two-dimmentional array corresponding to 2.5 L
static uint8_t s_twoAndFineLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{105,6,6,6,6,6,6,6,6,0,0,12,12,55,50,50,30,100},
		{105,7,7,7,7,7,7,7,7,0,0,14,14,45,50,50,30,100},
		{120,7,7,7,7,7,7,7,7,0,0,14,14,45,57,50,30,100}//50<=>2.5L
};

//Normal O2 two-dimmentional array corresponding to 3 L
static uint8_t s_threeLNormalO2 [eMaxTempType][eMaxItemOfParam] =
{
		{115,6,6,6,6,6,6,6,6,0,0,12,12,55,53,60,30,100},
		{120,7,7,7,7,7,7,7,7,0,0,14,14,45,53,60,30,100},
		{130,8,8,8,8,8,8,8,8,0,0,16,16,45,63,60,30,100}//60<=>3L
};

//Low O2 two-dimmentional array corresponding to 3 L
static uint8_t s_threeLLowO2 [eMaxTempType][eMaxItemOfParam] =
{
		{120,6,6,6,6,6,6,6,6,0,0,12,12,55,60,60,30,100},
		{125,7,7,7,7,7,7,7,7,0,0,14,14,45,60,60,30,100},
		{135,8,8,8,8,8,8,8,8,0,0,16,16,45,70,60,30,100}//60<=>3L
};

static uint8_t s_zeroAndTwoFineLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_zeroAndTwoFineLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_zeroAndFineLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_zeroAndFineLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_zeroAndSevenFineLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_zeroAndSevenFineLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_oneLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_oneLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_oneAndTwoFineLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_oneAndTwoFineLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_oneAndFineLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_oneAndFineLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_oneAndSevenFineLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_oneAndSevenFineLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_twoLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_twoLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_twoAndFineLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_twoAndFineLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

static uint8_t s_threeLNormalO2PSAParam[eMaxTempType][eMaxItemOfParam];
static uint8_t s_threeLLowO2PSAParam[eMaxTempType][eMaxItemOfParam];

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSATableMgr_InitTablePSA	                                        		*/
/*                                                                                      */
/* Details  -Initialize psa parameters table		        							*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) bool erasePSATable : - erase psa table						  		*/
/*                    - range : 0-1                                                     */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_zeroAndTwoFineLNormalO2                                    */
/*                     (O) s_zeroAndTwoFineLLowO2						                */
/*                     (O) s_zeroAndFineLNormalO2                                       */
/*                     (O) s_zeroAndFineLLowO2						                    */
/*                     (O) s_zeroAndSevenFineLNormalO2                                  */
/*                     (O) s_zeroAndSevenFineLLowO2						                */
/*                     (O) s_oneLNormalO2                                           	*/
/*                     (O) s_oneLLowO2						                            */
/*                     (O) s_oneAndTwoFineLNormalO2                                     */
/*                     (O) s_oneAndTwoFineLLowO2						                */
/*                     (O) s_oneAndFineLNormalO2                                        */
/*                     (O) s_oneAndFineLLowO2						                    */
/*                     (O) s_oneAndSevenFineLNormalO2                                   */
/*                     (O) s_oneAndSevenFineLLowO2						                */
/*                     (O) s_twoLNormalO2                                           	*/
/*                     (O) s_twoLLowO2						                            */
/*                     (O) s_twoAndFineLNormalO2                                        */
/*                     (O) s_twoAndFineLLowO2						                    */
/*                     (O) s_threeLNormalO2                                           	*/
/*                     (O) s_threeLLowO2						                        */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSATableMgr_InitTablePSA(bool erasePSATable)
{
	if(erasePSATable == true)
	{
		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ZERO_TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_zeroAndTwoFineLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ZERO_TWO_FINE_LOW_O2_PAGE,(uint32_t*)&s_zeroAndTwoFineLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ZERO_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_zeroAndFineLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ZERO_FINE_LOW_O2_PAGE,(uint32_t*)&s_zeroAndFineLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ZERO_SEVEN_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_zeroAndSevenFineLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ZERO_SEVEN_FINE_LOW_O2_PAGE,(uint32_t*)&s_zeroAndSevenFineLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_NORMAL_O2_PAGE,(uint32_t*)&s_oneLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_LOW_O2_PAGE,(uint32_t*)&s_oneLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_oneAndTwoFineLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_TWO_FINE_LOW_O2_PAGE,(uint32_t*)&s_oneAndTwoFineLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_oneAndFineLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_FINE_LOW_O2_PAGE,(uint32_t*)&s_oneAndFineLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_SEVEN_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_oneAndSevenFineLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_SEVEN_FINE_LOW_O2_PAGE,(uint32_t*)&s_oneAndSevenFineLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, TWO_NORMAL_O2_PAGE,(uint32_t*)&s_twoLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, TWO_LOW_O2_PAGE,(uint32_t*)&s_twoLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_twoAndFineLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, TWO_FINE_LOW_O2_PAGE,(uint32_t*)&s_twoAndFineLLowO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, THREE_NORMAL_O2_PAGE,(uint32_t*)&s_threeLNormalO2[0][0],SIZE_BUFFER_WRITE);

		guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, THREE_LOW_O2_PAGE,(uint32_t*)&s_threeLLowO2[0][0],SIZE_BUFFER_WRITE);

		logInterface_WriteMacineLog(guiInterface_GetSettingValue(eFlowRateSettingId), eUseDefaultParameterLogId);
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSATableMgr_SetPSAParam	                                        		*/
/*                                                                                      */
/* Details  -Set psa parameters table with flow rate	        						*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flowRate :  - flow rate setting								*/
/*                      - range : 5-60                                                  */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/* 				   E_O2ConcentratorType o2Type : - Condition O2 concentrator			*/
/*                       - range : 0-2                                                  */
/*                       - unit : NONE                                                  */
/*                                                                                      */
/* 				   E_TemperatureType tempType : - Condition temperature					*/
/*                       - range : 0-3                                                  */
/*                       - unit : NONE                                                  */
/*                                                                                      */
/* 				   E_ItemOfParam psaId : - psa item										*/
/*                       - range : 0-18                                                 */
/*                       - unit : NONE                                                  */
/*                                                                                      */
/* 				   uint8_t value : value item							  		    	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_zeroAndTwoFineLNormalO2PSAParam                            */
/*                     (O) s_zeroAndTwoFineLLowO2PSAParam						        */
/*                     (O) s_zeroAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_zeroAndFineLLowO2PSAParam						        	*/
/*                     (O) s_zeroAndSevenFineLNormalO2PSAParam                          */
/*                     (O) s_zeroAndSevenFineLLowO2PSAParam						        */
/*                     (O) s_oneLNormalO2PSAParam                            			*/
/*                     (O) s_oneLLowO2PSAParam						        			*/
/*                     (O) s_oneAndTwoFineLLowO2PSAParam                            	*/
/*                     (O) s_oneAndTwoFineLLowO2PSAParam						        */
/*                     (O) s_oneAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_oneAndFineLLowO2PSAParam						        	*/
/*                     (O) s_oneAndSevenFineLNormalO2PSAParam                           */
/*                     (O) s_oneAndSevenFineLLowO2PSAParam						        */
/*                     (O) s_twoLNormalO2PSAParam                            			*/
/*                     (O) s_twoLLowO2PSAParam						        			*/
/*                     (O) s_twoAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_twoAndFineLLowO2PSAParam						        	*/
/*                     (O) s_threeLNormalO2PSAParam                            			*/
/*                     (O) s_threeLLowO2PSAParam						        		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSATableMgr_SetPSAParam(uint8_t flowRate,E_O2ConcentratorType o2Type, E_TemperatureType tempType,E_ItemOfParam psaId, uint8_t value)
{
	switch(flowRate)
	{
	case SETTING_FR_025L:
		if(o2Type == eNormalO2Type)
		{
			s_zeroAndTwoFineLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_zeroAndTwoFineLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_050L:
		if(o2Type == eNormalO2Type)
		{
			s_zeroAndFineLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_zeroAndFineLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_075L:
		if(o2Type == eNormalO2Type)
		{
			s_zeroAndSevenFineLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_zeroAndSevenFineLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_100L:
		if(o2Type == eNormalO2Type)
		{
			s_oneLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_oneLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_125L:
		if(o2Type == eNormalO2Type)
		{
			s_oneAndTwoFineLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_oneAndTwoFineLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_150L:
		if(o2Type == eNormalO2Type)
		{
			s_oneAndFineLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_oneAndFineLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_175L:
		if(o2Type == eNormalO2Type)
		{
			s_oneAndSevenFineLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_oneAndSevenFineLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_200L:
		if(o2Type == eNormalO2Type)
		{
			s_twoLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_twoLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_250L:
		if(o2Type == eNormalO2Type)
		{
			s_twoAndFineLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_twoAndFineLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	case SETTING_FR_300L:
		if(o2Type == eNormalO2Type)
		{
			s_threeLNormalO2PSAParam[tempType][psaId] = value;
		}
		else//Low O2
		{
			s_threeLLowO2PSAParam[tempType][psaId] = value;
		}
		break;
	default:
		break;
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSATableMgr_GetPSAItem                                        				*/
/*                                                                                      */
/* Details  -Get psa parameters table on EEPROM or default with flow rate		        */
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flowRate :  - flow rate setting								*/
/*                      - range : 5-60                                                  */
/*                      - unit : NONE                                                   */
/*                                                                                      */
/* 				   E_O2ConcentratorType o2Type : - Condition O2 concentrator			*/
/*                       - range : 0-2                                                  */
/*                       - unit : NONE                                                  */
/*                                                                                      */
/* 				   E_TemperatureType tempType : - Condition temperature					*/
/*                       - range : 0-3                                                  */
/*                       - unit : NONE                                                  */
/*                                                                                      */
/* 				   E_ItemOfParam item : - psa item										*/
/*                       - range : 0-18                                                 */
/*                       - unit : NONE                                                  */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : uint8_t : psa item value		                                        */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_zeroAndTwoFineLNormalO2PSAParam							*/
/*                     (I) s_zeroAndTwoFineLLowO2PSAParam						        */
/*                     (I) s_zeroAndFineLNormalO2PSAParam                            	*/
/*                     (I) s_zeroAndFineLLowO2PSAParam						        	*/
/*                     (I) s_zeroAndSevenFineLNormalO2PSAParam                          */
/*                     (I) s_zeroAndSevenFineLLowO2PSAParam						        */
/*                     (I) s_oneLNormalO2PSAParam                            			*/
/*                     (I) s_oneLLowO2PSAParam						        			*/
/*                     (I) s_oneAndTwoFineLLowO2PSAParam                            	*/
/*                     (I) s_oneAndTwoFineLLowO2PSAParam						        */
/*                     (I) s_oneAndFineLNormalO2PSAParam                            	*/
/*                     (I) s_oneAndFineLLowO2PSAParam						        	*/
/*                     (I) s_oneAndSevenFineLNormalO2PSAParam                           */
/*                     (I) s_oneAndSevenFineLLowO2PSAParam						        */
/*                     (I) s_twoLNormalO2PSAParam                            			*/
/*                     (I) s_twoLLowO2PSAParam						        			*/
/*                     (I) s_twoAndFineLNormalO2PSAParam                            	*/
/*                     (I) s_twoAndFineLLowO2PSAParam						        	*/
/*                     (I) s_threeLNormalO2PSAParam                            			*/
/*                     (I) s_threeLLowO2PSAParam						        		*/
/*                     (O) NONE                                                         */
/*                     (O) NONE                                                         */
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
uint8_t PSATableMgr_GetPSAItem(uint8_t flowRate,E_O2ConcentratorType o2Type, E_TemperatureType tempType,E_ItemOfParam item)
{
	uint8_t retPara[eMaxTempType][eMaxItemOfParam];
	switch(flowRate)
	{
	case SETTING_FR_025L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_zeroAndTwoFineLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_zeroAndTwoFineLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_050L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_zeroAndFineLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_zeroAndFineLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_075L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_zeroAndSevenFineLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_zeroAndSevenFineLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_100L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_oneLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_oneLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_125L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_oneAndTwoFineLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_oneAndTwoFineLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_150L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_oneAndFineLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_oneAndFineLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_175L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_oneAndSevenFineLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_oneAndSevenFineLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_200L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_twoLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_twoLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_250L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_twoAndFineLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_twoAndFineLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	case SETTING_FR_300L:
		if(o2Type == eNormalO2Type)
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_threeLNormalO2PSAParam[i][j];
				}
			}
		}
		else//Low O2
		{
			for(int i = 0; i < eMaxTempType; i++)
			{
				for(int j = 0; j < eMaxItemOfParam; j++)
				{
					retPara[i][j] = s_threeLLowO2PSAParam[i][j];
				}
			}
		}
		break;
	default:
		break;
	}

	return retPara[tempType][item];
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSATableMgr_WritePSAParam	                                        		*/
/*                                                                                      */
/* Details  -Write PSA parameter to EEPROM		        								*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) uint8_t flowRate :  - flow rate setting								*/
/*                    - range : 5-60                                                    */
/*                    - unit : LPM                                                      */
/*                                                                                      */
/* 				E_O2ConcentratorType o2Type : condition Oxygenconcentration			    */
/*                    - range : 0-1                                                     */
/*                    - unit : NONE                                                     */
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) s_zeroAndTwoFineLNormalO2PSAParam							*/
/*                     (I) s_zeroAndTwoFineLLowO2PSAParam						        */
/*                     (I) s_zeroAndFineLNormalO2PSAParam                            	*/
/*                     (I) s_zeroAndFineLLowO2PSAParam						        	*/
/*                     (I) s_zeroAndSevenFineLNormalO2PSAParam                          */
/*                     (I) s_zeroAndSevenFineLLowO2PSAParam						        */
/*                     (I) s_oneLNormalO2PSAParam                            			*/
/*                     (I) s_oneLLowO2PSAParam						        			*/
/*                     (I) s_oneAndTwoFineLLowO2PSAParam                            	*/
/*                     (I) s_oneAndTwoFineLLowO2PSAParam						        */
/*                     (I) s_oneAndFineLNormalO2PSAParam                            	*/
/*                     (I) s_oneAndFineLLowO2PSAParam						        	*/
/*                     (I) s_oneAndSevenFineLNormalO2PSAParam                           */
/*                     (I) s_oneAndSevenFineLLowO2PSAParam						        */
/*                     (I) s_twoLNormalO2PSAParam                            			*/
/*                     (I) s_twoLLowO2PSAParam						        			*/
/*                     (I) s_twoAndFineLNormalO2PSAParam                            	*/
/*                     (I) s_twoAndFineLLowO2PSAParam						        	*/
/*                     (I) s_threeLNormalO2PSAParam                            			*/
/*                     (I) s_threeLLowO2PSAParam						        		*/
/*                     (O) NONE                                           			   	*/
/*                     (O) NONE						                                   	*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSATableMgr_WritePSAParam(uint8_t flowRate,E_O2ConcentratorType o2Type)
{
	switch(flowRate)
	{
	case SETTING_FR_025L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ZERO_TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_zeroAndTwoFineLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ZERO_TWO_FINE_LOW_O2_PAGE,(uint32_t*)&s_zeroAndTwoFineLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_050L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ZERO_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_zeroAndFineLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ZERO_FINE_LOW_O2_PAGE,(uint32_t*)&s_zeroAndFineLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_075L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ZERO_SEVEN_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_zeroAndSevenFineLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ZERO_SEVEN_FINE_LOW_O2_PAGE,(uint32_t*)&s_zeroAndSevenFineLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_100L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_NORMAL_O2_PAGE,(uint32_t*)&s_oneLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET, ONE_LOW_O2_PAGE,(uint32_t*)&s_oneLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_125L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ONE_TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_oneAndTwoFineLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ONE_TWO_FINE_LOW_O2_PAGE,(uint32_t*)&s_oneAndTwoFineLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_150L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ONE_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_oneAndFineLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ONE_FINE_LOW_O2_PAGE,(uint32_t*)&s_oneAndFineLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_175L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ONE_SEVEN_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_oneAndSevenFineLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,ONE_SEVEN_FINE_LOW_O2_PAGE,(uint32_t*)&s_oneAndSevenFineLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_200L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,TWO_NORMAL_O2_PAGE,(uint32_t*)&s_twoLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,TWO_LOW_O2_PAGE,(uint32_t*)&s_twoLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_250L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&s_twoAndFineLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,TWO_FINE_LOW_O2_PAGE,(uint32_t*)&s_twoAndFineLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	case SETTING_FR_300L:
		if(o2Type == eNormalO2Type)
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,THREE_NORMAL_O2_PAGE,(uint32_t*)&s_threeLNormalO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		else//Low O2
		{
			guiInterface_EEPROMWrite(EEPROM_PSA_TABLE_OFFSET,THREE_LOW_O2_PAGE,(uint32_t*)&s_threeLLowO2PSAParam[0][0],SIZE_BUFFER_WRITE);
		}
		break;
	default:
		break;
	}
	setting_RecordChecksum();

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSATableMgr_SetDefaultSelectPort	        	                            */
/*                                                                                      */
/* Details  -Set PSA parameter when psa select port is default		        			*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_zeroAndTwoFineLNormalO2PSAParam                            */
/*                     (O) s_zeroAndTwoFineLLowO2PSAParam						        */
/*                     (O) s_zeroAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_zeroAndFineLLowO2PSAParam						        	*/
/*                     (O) s_zeroAndSevenFineLNormalO2PSAParam                          */
/*                     (O) s_zeroAndSevenFineLLowO2PSAParam						        */
/*                     (O) s_oneLNormalO2PSAParam                            			*/
/*                     (O) s_oneLLowO2PSAParam						        			*/
/*                     (O) s_oneAndTwoFineLLowO2PSAParam                            	*/
/*                     (O) s_oneAndTwoFineLLowO2PSAParam						        */
/*                     (O) s_oneAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_oneAndFineLLowO2PSAParam						        	*/
/*                     (O) s_oneAndSevenFineLNormalO2PSAParam                           */
/*                     (O) s_oneAndSevenFineLLowO2PSAParam						        */
/*                     (O) s_twoLNormalO2PSAParam                            			*/
/*                     (O) s_twoLLowO2PSAParam						        			*/
/*                     (O) s_twoAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_twoAndFineLLowO2PSAParam						        	*/
/*                     (O) s_threeLNormalO2PSAParam                            			*/
/*                     (O) s_threeLLowO2PSAParam						        		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSATableMgr_SetDefaultSelectPort(void)
{
	//Get 0.25L normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndTwoFineLNormalO2PSAParam[i][j] = s_zeroAndTwoFineLNormalO2[i][j];
		}
	}

	//Get 0.25L low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndTwoFineLLowO2PSAParam[i][j] = s_zeroAndTwoFineLLowO2[i][j];
		}
	}

	//Get 0.5L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndFineLNormalO2PSAParam[i][j] = s_zeroAndFineLNormalO2[i][j];
		}
	}

	//Get 0.5L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndFineLLowO2PSAParam[i][j] = s_zeroAndFineLLowO2[i][j];
		}
	}

	//Get 0.75L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndSevenFineLNormalO2PSAParam[i][j] = s_zeroAndSevenFineLNormalO2[i][j];
		}
	}

	//Get 0.75L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndSevenFineLLowO2PSAParam[i][j] = s_zeroAndSevenFineLLowO2[i][j];
		}
	}

	//get 1L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneLNormalO2PSAParam[i][j] = s_oneLNormalO2[i][j];
		}
	}

	//get 1L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneLLowO2PSAParam[i][j] = s_oneLLowO2[i][j];
		}
	}

	//get 1.25L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndTwoFineLNormalO2PSAParam[i][j] = s_oneAndTwoFineLNormalO2[i][j];
		}
	}

	//get 1.25L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndTwoFineLLowO2PSAParam[i][j] = s_oneAndTwoFineLLowO2[i][j];
		}
	}

	//get 1.5L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndFineLNormalO2PSAParam[i][j] = s_oneAndFineLNormalO2[i][j];
		}
	}

	//get 1.5L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndFineLLowO2PSAParam[i][j] = s_oneAndFineLLowO2[i][j];
		}
	}

	//get 1.75L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndSevenFineLNormalO2PSAParam[i][j] = s_oneAndSevenFineLNormalO2[i][j];
		}
	}

	//get 1.75L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndSevenFineLLowO2PSAParam[i][j] = s_oneAndSevenFineLLowO2[i][j];
		}
	}

	//get 2L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_twoLNormalO2PSAParam[i][j] = s_twoLNormalO2[i][j];
		}
	}

	//get 2L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_twoLLowO2PSAParam[i][j] = s_twoLLowO2[i][j];
		}
	}

	//get 2.5L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_twoAndFineLNormalO2PSAParam[i][j] = s_twoAndFineLNormalO2[i][j];
		}
	}

	//get 2.5L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_twoAndFineLLowO2PSAParam[i][j] = s_twoAndFineLLowO2[i][j];
		}
	}

	//get 3L Normal O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_threeLNormalO2PSAParam[i][j] = s_threeLNormalO2[i][j];
		}
	}

	//get 3L Low O2
	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_threeLLowO2PSAParam[i][j] = s_threeLLowO2[i][j];
		}
	}

	return;
}

/****************************************************************************************/
/*                                                                                      */
/* General 	-PSATableMgr_SetEEPROMSelectPort	        	                            */
/*                                                                                      */
/* Details  -Set PSA parameter if is eeprom select port		        					*/
/*        																				*/
/*																						*/
/*                                                                                      */
/* Arguments : (I) NONE																	*/
/*                                                                                      */
/*             (O) NONE																	*/
/*                                                                                      */
/* ReturnValue : void		                                                            */
/*        		 			                                                            */
/*                                                                                      */
/* Using Global Data : (I) NONE							                              	*/
/*                     (O) s_zeroAndTwoFineLNormalO2PSAParam                            */
/*                     (O) s_zeroAndTwoFineLLowO2PSAParam						        */
/*                     (O) s_zeroAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_zeroAndFineLLowO2PSAParam						        	*/
/*                     (O) s_zeroAndSevenFineLNormalO2PSAParam                          */
/*                     (O) s_zeroAndSevenFineLLowO2PSAParam						        */
/*                     (O) s_oneLNormalO2PSAParam                            			*/
/*                     (O) s_oneLLowO2PSAParam						        			*/
/*                     (O) s_oneAndTwoFineLLowO2PSAParam                            	*/
/*                     (O) s_oneAndTwoFineLLowO2PSAParam						        */
/*                     (O) s_oneAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_oneAndFineLLowO2PSAParam						        	*/
/*                     (O) s_oneAndSevenFineLNormalO2PSAParam                           */
/*                     (O) s_oneAndSevenFineLLowO2PSAParam						        */
/*                     (O) s_twoLNormalO2PSAParam                            			*/
/*                     (O) s_twoLLowO2PSAParam						        			*/
/*                     (O) s_twoAndFineLNormalO2PSAParam                            	*/
/*                     (O) s_twoAndFineLLowO2PSAParam						        	*/
/*                     (O) s_threeLNormalO2PSAParam                            			*/
/*                     (O) s_threeLLowO2PSAParam						        		*/
/*【Note】                                                                              												*/
/*                                                                                      */
/****************************************************************************************/
void PSATableMgr_SetEEPROMSelectPort(void)
{
	uint8_t buffer[eMaxTempType][eMaxItemOfParam];
	uint8_t tempBuffer[SIZE_BUFFER_WRITE];//temp buffer

	memset(tempBuffer,0,SIZE_BUFFER_WRITE);

	//Get 0.25L normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndTwoFineLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 0.25L normal O2

	//Get 0.25L low O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_TWO_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndTwoFineLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 0.25L low O2

	//Get 0.5L Normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndFineLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 0.5L Normal O2

	//Get 0.5L Low O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndFineLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 0.5L Low O2

	//Get 0.75L Normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_SEVEN_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndSevenFineLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 0.75L Normal O2

	//Get 0.75L Low O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ZERO_SEVEN_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_zeroAndSevenFineLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 0.75L Low O2

	//Get 1L Normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET,  ONE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 1L Normal O2

	//Get 1L Low O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 1L Low O2

	//Get 1.25L Normal O2
	//read from EEPROM
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndTwoFineLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 1.25L Normal O2

	//Get 1.25L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_TWO_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndTwoFineLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 1.25L Low O2

	//Get 1.5L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndFineLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 1.5L Normal O2

	//Get 1.5L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndFineLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 1.5L Low O2

	//Get 1.75L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_SEVEN_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndSevenFineLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 1.75L Normal O2

	//Get 1.75L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, ONE_SEVEN_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_oneAndSevenFineLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 1.75L Low O2

	//Get 2L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, TWO_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_twoLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 2L Normal O2

	//Get 2L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, TWO_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_twoLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 2L Low O2

	//Get 2.5L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, TWO_FINE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_twoAndFineLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 2.5L Normal O2

	//Get 2.5L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, TWO_FINE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_twoAndFineLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 2.5L Low O2

	//Get 3L Normal O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, THREE_NORMAL_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_threeLNormalO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 3L Normal O2

	//Get 3L Low O2
	guiInterface_ReadEEPROM(EEPROM_PSA_TABLE_OFFSET, THREE_LOW_O2_PAGE,(uint32_t*)&tempBuffer[0],SIZE_BUFFER_WRITE);
	//copy from tempBuffer to buffer
	memcpy(&buffer[0][0], &tempBuffer[0], SIZE_BUFFER_READ);

	for(int i = 0; i < eMaxTempType; i++)
	{
		for(int j = 0; j < eMaxItemOfParam; j++)
		{
			s_threeLLowO2PSAParam[i][j] = buffer[i][j];
		}
	}
	//End Get 3L Low O2

	return;
}

/* end of files */

