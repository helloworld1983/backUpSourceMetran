#ifndef INC_PSA_CONTROL_H_
	#define INC_PSA_CONTROL_H_

/********************************************************************/
/*                                                                  */
/* File Name    : PsaControl.h		                                */
/*                                                                  */
/* General       : This module contain function for init and handle
 * 				   psa device based on psa parameter				*/
/*                                                                  */
/* Product Name  : FH310_m0                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            Jun 30, 2016      Quyen Ngo(MV)  		new file        */
/*                                                                  */
/*	#0001	  2017/09/01		Quyen Ngo(MV)		modify			*/
/*		-remove psa_GetPsaState	function							*/
/*  #0002	   2017/12/13	   Quyen Ngo(MV)		modify			*/
/*		Add void to non parameter function to follow coding rule	*/
/********************************************************************/

#include <IpcTask.h>
#include "TaskCommon.h"

#define P_COMP_STLOW 50 //50kpa
#define PSA_COMP_RESTART_30S_TIME 30000/DEVICE_TASK_DELAY_TIME

typedef enum {
	PHASE_A,
	PHASE_B
} E_PsaPhase;

// Create timer
void psa_CreateTimer(void);

void psa_SetMode(E_PsaMode mode);

E_PsaMode psa_GetMode(void);

//psa Handle
void psa_Handle(float productTankPress);

void psa_RequestEnterStartSequence(void);

// Enter to psa start sequence
void psa_EnterStartSequence(void);

// Enter to psa end sequence
void psa_EnterEndSequence(void);

// Update new psa parameter and time period
void psa_UpdateParameter(PSA_PARAM_PACKET_t stPsaParam);

// active psa parameter follow psa start up sequence
void psa_ActivePsaParameterAtStart(PSA_PARAM_PACKET_t stPsaParam);

// Active new psa parameter
void psa_ActiveNewPsaParameter(PSA_PARAM_PACKET_t stPsaParam);

// handle psa parameter table change setting by user
void psa_ChangeParamSetting(IPC_MSG_DATA_t msg);

//Get psa cycle time
uint16_t psa_GetCycleTime(void);

//Get psa current sequence
E_PsaSequence psa_GetPsaSeq(void);

void psa_StartCheckCompRestartSeq(void);

void psa_FinishCompressorRestart(void);

#endif /* INC_PSA_CONTROL_H_ */
