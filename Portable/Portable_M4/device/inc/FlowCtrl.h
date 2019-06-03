#ifndef INC_FLOW_CONTROLLER_H_
	#define INC_FLOW_CONTROLLER_H_
/********************************************************************/
/*                                                                  */
/* File Name    : FlowCtrl.h		                                */
/*                                                                  */
/* General       : This module contain function for initialize and	*/
/* 				   communicate with flow controller                 */
/*                 	 						                        */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/08/23      Quyen Ngo(MV)  	new file        	*/
/*                                                                  */
/********************************************************************/

#include <DualCoreCommon.h>

#define FLOWCONTROLLER_PRESSURE_SENSOR_ERR_BIT 				1<<0
#define FLOWCONTROLLER_FLOW_SENSOR_ERR_BIT 					1<<1
#define FLOWCONTROLLER_TEMPERATURE_SENSOR_ERR_BIT 			1<<2
#define FLOWCONTROLLER_CMD_TIMEOUT_ERR_BIT					1<<3
#define FLOWCONTROLLER_COMMUNICATION_NO_RESPONSE_ERR_BIT	1<<7

#define FLOWCONTROLLER_CHECKSUM_COMMUNICATION_ERR_BIT 		1<<0
#define FLOWCONTROLLER_COMMAND_COMMUNICATION_ERR_BIT 		1<<1
#define FLOWCONTROLLER_PARAMETER_COMMUNICATION_ERR_BIT 		1<<2


typedef struct {
	uint16_t flowMsrVal; //Flow measure value
	uint16_t O2Val;		 //O2 % value
} FC_Data;

typedef enum
{
	FC_NORMAL_SEQ,
	FC_RESET_SEQ,
	FC_REBOOT_SEQ
}E_FlowCtrlSequence;

typedef enum
{
	FC_SEND_FLOW,
	FC_SEND_VERSION,
	FC_SEND_ACQ1,
	FC_SEND_ACQ2,
	FC_SEND_MODE,
	FC_SEND_OCSET,
	FC_SEND_CLEAR_ERR
}E_FlowCtrlRequestCmd;

typedef enum
{
	ContinuousMode,
	SynchronizedMode
}ModeSetting;

enum
{
	eFlowRateZeroOneZero,
	eFlowRateZeroTwoFive,
	eFlowRateZeroFiveZero,
	eFlowRateZeroSevenFive,
	eFlowRateOneZeroZero,
	eFlowRateOneTwoFive,
	eFlowRateOneFiveZero,
	eFlowRateOneSevenFive,
	eFlowRateTwoZeroZero,
	eFlowRateTwoTowFive,
	eFlowRateTwoFiveZero,
	eFlowRateTwoSevenFive,
	eFlowRateThreeZeroZero,
	eFlowRateZeroZeroZero,
	eFlowRateLast
};

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_ReadFromDevice                                             */
/*                                                                                      */
/* Details:        read uart data return from flow controller                           */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_ReadFromDevice();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_InitCommunication                                          */
/*                                                                                      */
/* Details:        init uart communication                                              */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_InitCommunication(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendCmd                                                    */
/*                                                                                      */
/* Details:        send command via UART                                                */
/*                                                                                      */
/* Arguments:   unsigned char code                                                      */
/*              const unsigned char parameter[]                                         */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendCmd(unsigned char code,const unsigned char parameter[]);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendComunicationConfirmCmd                                 */
/*                                                                                      */
/* Details:        send confirm cmd                                                     */
/*                                                                                      */
/* Arguments:   None                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendComunicationConfirmCmd();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendTestFlowCmd                                            */
/*                                                                                      */
/* Details:        send test flow cmd                                                   */
/*                                                                                      */
/* Arguments:   uint16_t flowVal                                                        */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendTestFlowCmd(uint16_t flowVal);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendTestO2Cmd                                              */
/*                                                                                      */
/* Details:        send test 02 cmd                                                     */
/*                                                                                      */
/* Arguments:   uint16_t o2Val                                                          */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendTestO2Cmd(uint16_t o2Val);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendStatusAcquisition1Cmd                                  */
/*                                                                                      */
/* Details:        send acquistion1 cmd                                                 */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendStatusAcquisition1Cmd();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendStatusAcquisition2Cmd                                  */
/*                                                                                      */
/* Details:        send acquistion2 cmd                                                 */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendStatusAcquisition2Cmd();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendVersionAcquisitionCmd                                  */
/*                                                                                      */
/* Details:        send version cmd                                                     */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendVersionAcquisitionCmd();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendErrorStatusResetCmd                                    */
/*                                                                                      */
/* Details:        send err reset cmd                                                   */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendErrorStatusResetCmd();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendFlowRateCmd                                            */
/*                                                                                      */
/* Details:        send flow rate cmd                                                   */
/*                                                                                      */
/* Arguments:   uint16_t flowVal                                                        */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendFlowRateCmd(uint16_t flowVal);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_RequestSetFlow                                             */
/*                                                                                      */
/* Details:        Request send flow rate cmd                                           */
/*                                                                                      */
/* Arguments:   uint16_t flow                                                           */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_RequestSetFlow(uint16_t flow);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_Handle                                                     */
/*                                                                                      */
/* Details:        handle flow controller                                               */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_Handle();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_Reset                                                      */
/*                                                                                      */
/* Details:        reset flow controller                                                */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_Reset();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_GetFlowVal                                                 */
/*                                                                                      */
/* Details:        get flow value                                                       */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
uint16_t flowCtrl_GetFlowVal(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_GetO2Val                                                   */
/*                                                                                      */
/* Details:        get O2 value                                                         */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
uint16_t flowCtrl_GetO2Val(void);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_GetTemperature                                             */
/*                                                                                      */
/* Details: Get current Temperature                                                     */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : uint16_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint16_t flowCtrl_GetTemperature(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_GetPressure                                                */
/*                                                                                      */
/* Details: Get current Pressure                                                        */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : uint16_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint16_t flowCtrl_GetPressure(void);


/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_getErrStatus                                               */
/*                                                                                      */
/* Details: Get current compressor error status                                         */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : uint16_t                                                               */
/*                                                                                      */
/****************************************************************************************/
uint8_t flowCtrl_getErrStatus();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_getCommunicateErrStatus                                    */
/*                                                                                      */
/* Details: Get current flow controller communication error status                      */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : uint8_t                                                                */
/*                                                                                      */
/****************************************************************************************/
uint8_t flowCtrl_getCommunicateErrStatus();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_getResponseErrStatus                                       */
/*                                                                                      */
/* Details: Get current flow controller error response status                           */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : E_FlowControllerNoAlarmStatus                                          */
/*                                                                                      */
/****************************************************************************************/
E_FlowControllerNoAlarmStatus flowCtrl_getResponseErrStatus();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_GetResetStatus                                             */
/*                                                                                      */
/* Details:        flow Ctrl get reset status                                           */
/*                                                                                      */
/* Arguments:   void                                                                    */
/*                                                                                      */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/****************************************************************************************/
bool flowCtrl_GetResetStatus();

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_GetVersion                                                 */
/*                                                                                      */
/* Details:        flow ctrl get version                                                */
/*                                                                                      */
/* Arguments:   char *version                                                           */
/*                                                                                      */
/* ReturnValue : unsigned char                                                          */
/*                                                                                      */
/****************************************************************************************/
unsigned char* flowCtrl_GetVersion(char*);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendModeSettingCmd                                         */
/*                                                                                      */
/* Details:        Send set Mode cmd                                                    */
/*                                                                                      */
/* Arguments:   ModeSetting                                                             */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendModeSettingCmd(ModeSetting);

//Send Open and Close setting
/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_SendOpenAndCloseSettingCmd                                 */
/*                                                                                      */
/* Details:        Send Open and Close setting                                          */
/*                                                                                      */
/* Arguments:   uint16_t* msecond                                                       */
/*                                                                                      */
/* ReturnValue : void                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_SendOpenAndCloseSettingCmd(uint16_t* msecond);

/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_Stop                                                       */
/*                                                                                      */
/* Details: Stop flow controller                                                        */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : None                                                                   */
/*                                                                                      */
/****************************************************************************************/
void flowCtrl_Stop();


/****************************************************************************************/
/*                                                                                      */
/* Function name:   flowCtrl_isGetVersionDone                                           */
/*                                                                                      */
/* Details:        Check whether read version process is finished                       */
/*                                                                                      */
/* Arguments:   (I) None                                                                */
/*              (O) None                                                                */
/* ReturnValue : bool                                                                   */
/*                                                                                      */
/*                                                                                      */
/****************************************************************************************/
bool flowCtrl_isGetVersionDone();

#endif /* INC_FLOW_CONTROLLER_H_ */
