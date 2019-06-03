#ifndef INC_FILTERCLEANINGMODULE_H_
	#define INC_FILTERCLEANINGMODULE_H_

/******************************************************************************/
//
/********************************************************************/
/*                                                                  */
/* File Name     : FilterCleaningModule.h		                    */
/*                                                                  */
/* General       : This file contains the function for init and    	*/
/*				   handle operation  of filer cleaning module       */
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
/*             2016/11/26      Quyen Ngo  	new file       		    */
/*                                                                  */
/* #0001	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add void to non parameter function to follow coding rule	*/
/********************************************************************/
#include <lpc_types.h>
#include "TaskCommon.h"

/* constants, macro definition */

typedef enum
{
	FT_START,
	FT_NEW_CYCLE,
	FT_WAIT_T_FCM_WT,
	FT_WAIT_T_FCM_CYCLE,
	FT_STOP
} E_FilterSequence;

void filter_Init(void);

void filter_On(void);

void filter_Off(void);

bool filter_DetectErr(bool prevErrState, bool errState, E_FilterSequence filterSequence);

void filter_Handle(void);

void filter_UpdateCumulativeHour(uint32_t sec);

bool filter_getErrPinState(void);

bool filter_getErrStatus(void);

void filter_Stop(void);

void filter_Start(void);

#endif /* end of file */
