
#ifndef AUDIO_INC_AUDIOTASK_H_
#define AUDIO_INC_AUDIOTASK_H_
/********************************************************************/
/*                                                                  */
/* File Name    : AudioTask.h		                                */
/*                                                                  */
/* General       : This module contain function for Create RTOS task*/
/* to control Audio													*/
/*                                                                  */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable	                                        */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*            April 18, 2016    Thao Ha(MV)  		new file    	*/
/*                                                                  */
/********************************************************************/
#include <AudioMgr.h>
#include <TaskCommon.h>
#include "DualCoreCommon.h"

#define IS_SPEAKER_CONNECTED 0

/****************************************************************************************/
/*                                                                                      */
/* Function name:   audioTask_Init                                                      */
/*                                                                                      */
/* Details:        Init I2C1 task by creating audioQueue, setting audio chip			*/
/*                 							                                            */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void audioTask_Init(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   audioTask_CreateTask                                                */
/*                                                                                      */
/* Details:        Create FREERTOS task name audio task                                 */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void audioTask_CreateTask(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   audioTask_SendToAudioQueue                                          */
/*                                                                                      */
/* Details:        Send data to Audio Queue                                             */
/*                                                                                      */
/* Arguments:   AUDIO_T AudioQueueData                                                  */
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*               false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool audioTask_SendToAudioQueue(AUDIO_T AudioQueueData,TickType_t xTicksToWait);


#endif /* AUDIO_INC_AUDIOTASK_H_ */
