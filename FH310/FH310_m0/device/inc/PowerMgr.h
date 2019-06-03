#ifndef INC_POWERMGR_H_
	#define INC_POWERMGR_H_
/********************************************************************/
/*                                                                  */
/* File Name     : PowerMgr.h			                            */
/*                                                                  */
/* General       : This file contain function for handle the power	*/
/*  			   signal of FH310									*/
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
/*             2016/09/28      Quyen Ngo  			new file  	    */
/*	#0002.	   2017/09/08	   Quyen Ngo			modify			*/
/*		When detect power lost signal, just send the msg to M4, does*/
/*      not judge the current mode                                  */
/*	#0003	   2017/11/02	   Quyen Ngo			modify			*/
/*		-Chang the return value of power_SendPowerLostMsgToM4 func	*/
/*      to bool for handle the IPC queue full case (Bug 2007)       */
/********************************************************************/

#include <lpc_types.h>

void power_Init(void);

void power_Handle(void);

bool power_CheckPowerWhenHandShake(void);

bool power_SendPowerLostMsgToM4(void);

#endif /* INC_POWERMGR_H_ */
/* end of file */
