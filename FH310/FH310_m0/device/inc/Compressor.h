#ifndef INC_COMPRESSOR_H_
	#define INC_COMPRESSOR_H_

/********************************************************************/
/*                                                                  */
/* File Name     : Compressor.h		                                */
/*                                                                  */
/* General       : This file contains the functions for init and    */
/*				  handle compressor communication                   */
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
/*             2016/10/07      Quyen Ngo  	new file       		    */
/*																	*/
/*	#0001	   2017/12/07	   Quyen Ngo	modify					*/
/* 	Move COMP_MAX_RPM macro from .c to .h file(Bug 2054)			*/
/*                                                                  */
/********************************************************************/

#include "DualCoreCommon.h"
#include "TaskCommon.h"
#include "PsaSelectMgr.h"


#define COMP_ABNORMAL_DC_VOLT_BIT			0x01
#define COMP_ABNORMAL_SW_OVERCURRENT_BIT	0x02
#define COMP_ABNORMAL_TEMP_ERR_BIT 			0x04
#define COMP_ROTAION_SPEED_ERR_BIT 			0x08
#define COMP_ABNORMAL_HW_OVERCURRENT_BIT	0x10
#define COMP_MAX_RPM						100

void compressor_InitCommunication(void);

void compressor_ReadFromDevice(void);

void compressor_Handle(void);

void compressor_Reset(void);

void compressor_ResetWhenAlarm(void);

void compressor_Restart(void);

void compressor_Start(void);

uint16_t compressor_getErrStatus(void);

E_CompressorSequence compressor_GetCurentSeq(void);

uint16_t compressor_getRpm(void);

void compressor_ReqestSetRpm(uint16_t rpm);

void compressor_Stop(void);

void compressor_ClearAlarmStatus(void);

uint8_t* compressor_GetVersion(void);

void compressor_UpdatePsaParamRpm(uint16_t psaParamRpm);

void compressor_setTcomHspTime(E_TempertureType type);

#endif /* INC_COMPRESSOR_H_ */
/* end of file */
