/********************************************************************/
/*                                                                  */
/* File Name    : AudioTask.c		                                */
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
#include <AudioTask.h>
#include <DevTask.h>

#define SPEAKER_STATUS_UPDATE_TIME 5000/AUDIO_TASK_DELAY_TIME

QueueHandle_t audioQueue;

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	audioTask_Loop						              				    */
/*                                                                                      */
/* Details:        Audio task loop function is call function for handle audio every 10ms*/
/*                 											                            */
/* Arguments:	void *pvParameters														*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
static void audioTask_Loop(void *pvParameters)
{
	audioMgr_InitAudio();
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	AUDIO_T eventId;
	uint32_t s_cnt = 0;
	static bool s_AudioState = true;
	DEV_EVENT_STRUCT deviceQueue;
	deviceQueue.id = eFromAudioTask;
#ifdef CHECK_REMAINING_STACK_SIZE
	UBaseType_t uxHighWaterMark;
	uint32_t cnt = 0;
#endif
	while(1)
	{
		s_cnt++;
		if(xQueueReceive(audioQueue, &eventId, 0) == pdTRUE)		//wait max 10 ticks
		{
			if(s_AudioState==true)
			{
				if(eventId.ID != eNoneAudio)
				{
					audioMgr_PlayAudio(eventId.ID,eventId.volume);
					if (eventId.ID == eStartUpAudio)
						vTaskDelay(DELAY_TICK_3S); // Delay for start up audio
					else if (eventId.ID == eTerminateAudio)
						s_AudioState = false;	//Terminate audio when shutdown
				}
			}
		}
		else
			if(s_cnt>=SPEAKER_STATUS_UPDATE_TIME)
			{
				deviceQueue.msgData.audioMsg.isSpeakerConnected = audioMgr_IsSpeakerConnected();
				if(!devTask_SendToDeviceQueue(deviceQueue,10)){
					DEBUG_TASK("Failed to send Item to deviceQueue from batteryTask_Loop");
				}
				s_cnt = 0;
			}
#ifdef CHECK_REMAINING_STACK_SIZE
		uxHighWaterMark = uxTaskGetStackHighWaterMark( NULL );
		if(cnt>=1000/AUDIO_TASK_DELAY_TIME)
		{
			DEBUGOUT("I2C1 Task %d\n",uxHighWaterMark);
			cnt = 0;
		}
		else
		{
			cnt++;
		}
#endif
		vTaskDelayUntil( &xLastWakeTime, AUDIO_TASK_DELAY_TIME );
	}
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	audioTask_Init						              				    */
/*                                                                                      */
/* Details:        Init audio task by creating audioQueue, setting audio chip			*/
/*                 								                                        */
/* Arguments:	(I) None																*/
/*              (O) None                                                                */
/* ReturnValue : None			                                                        */
/*                                                                                      */
/****************************************************************************************/
void audioTask_Init(void)
{
	audioQueue = xQueueCreate(AUDIO_QUEUE_LENGTH, sizeof(AUDIO_T));
}
/****************************************************************************************/
/*                                                                                      */
/* Function name: 	audioTask_CreateTask					              				*/
/*                                                                                      */
/* Details:        Create FREERTOS task name audio task									*/
/*                                                                                      */
/* Arguments:	None																	*/
/*                                                                                      */
/* ReturnValue : void			                                                        */
/*                                                                                      */
/****************************************************************************************/
void audioTask_CreateTask(void)
{
	xTaskCreate(audioTask_Loop, "audio task",
			AUDIO_TASK_SIZE, (void *)1, TASK_PRIO_AUDIO_TASK,
			( TaskHandle_t * ) NULL );
}

/****************************************************************************************/
/*                                                                                      */
/* Function name: 	audioTask_SendToAudioQueue   		              				    */
/*                                                                                      */
/* Details:        Send data to Audio Queue												*/
/*                                                                                      */
/* Arguments:	AUDIO_T AudioQueueData											 		*/
/*              TickType_t xTicksToWait                                                 */
/*                                                                                      */
/* ReturnValue : true if it sent successfully                                           */
/*  			 false if it fails to sent                                              */
/*                                                                                      */
/****************************************************************************************/
bool audioTask_SendToAudioQueue(AUDIO_T AudioQueueData,TickType_t xTicksToWait)
{
	bool status = false;
	AUDIO_T audioQueueDataSend = AudioQueueData;
	//send Data to Audio Queue
	if(!xQueueSend(audioQueue, &audioQueueDataSend, xTicksToWait)){
		//return false if it fails to sent
		status = false;
	}else{
		//return true if it sent successfully
		status = true;
	}
	return status;
}


