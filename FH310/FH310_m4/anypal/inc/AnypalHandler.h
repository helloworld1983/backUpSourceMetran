#ifndef INC_ANYPALHANDLER_H_
#define INC_ANYPALHANDLER_H_

/********************************************************************/
/*                                                                  */
/* File Name    : AnypalHandler.h                        			*/
/*                                                                  */
/* General      : Anypal Handler									*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/* #0001       2016/11/01       Linh Nguyen	  	    new file        */
/*        															*/
/* #0002       2016/08/24       Linh Nguyen	  	    modify          */
/*  - Start gs_AnypalCycleTimer	timer after saving	all usage log	*/
/*  for anypal.          											*/
/*  - Add anypal reconnect err timer and start, stop it function	*/
/*        															*/
/* #0003       2016/09/20       Linh Nguyen	  	    modify          */
/*  Add anypal_SendGettingSpO2PulseDataStatus function				*/
/*  Add anypal_SetCalendarUpdate function							*/
/*  Add new define, modify some difine of bug						*/
/* #0004       2016/09/25       Linh Nguyen	  	    modify          */
/*  Add anypal_ResetAllAnypalStateWhenOVCR function					*/
/*  Change MAX_TRANSMIT_LOG to 40									*/
/********************************************************************/

#include "board.h"

#define ANYPAL_TASK_DELAY 50
#define ANYPAL_TASK_SIZE 4*1024

#define ANYPAL_RESPONE_TIMER 3000
#define ANYPAL_CYCLE_TIMER 20000
#define TURN_ON_VBUS_TIMER 300
#define ALARM_OVER_CURRENT_TURN_OFF_TIME 1500
#define ANYPAL_PUSH_OUT_TIME 200
#define ANYPAL_RECONNECT_ERR_TIMER 5000

#define ANYPAL_RESPONE_TIMER_ID 0
#define ANYPAL_CYCLE_TIMER_ID 1
#define TURN_ON_VBUS_TIMER_ID 2
#define OVERCURRENT_TIMER_ID 3
#define ANYPAL_PUSH_OUT_TIME_ID 4
#define ANYPAL_RECONNECT_ERR_TIMER_ID 5

#define RESET_VBUS 3
#define MAX_RESEND_PCKET 6

#define ANYPAL_CONNECT true
#define ANYPAL_DISCONNECT false

#define SEND_PACKET_INFOR_LENGTH 			10
#define SEND_PACKET_PATIENTID_LENGTH 		10
#define SEND_PACKET_GET_CALENDAR_LENGTH 	10
#define SEND_PACKET_SET_CALENDAR_LENGTH 	16
#define SEND_PACKET_QUANTITY_DATA_LENGTH 	10
#define SEND_PACKET_QUANTITY_DATA_FH 		10
#define SEND_PACKET_GETDATA_LENGTH 			15
#define MAX_SEND_PACKET_LENGTH 				16

#define RECEIVE_PACKET_GET_QUANTITY_DATA_LEN 		22
#define RECEIVE_PACKET_ID_NUMBER_LEN 				20

#define PACKET_ATP_INFO 							0x01
#define PACKET_GET_CALENDAR 						0x02
#define PACKET_GET_PATIENTID 						0x03
#define PACKET_SET_CALENDAR 						0x12
#define PACKET_QUANTITY_DATA 						0x04
#define PACKET_GET_LOG_DATA 						0x33
#define PACKET_QUANTITY_DATA_FH 					0x05
#define PACKET_STORAGE_OXYGEN_DATA 					0x21

#define PARAMETER_LENGTH_OFFSET 		9 + 1 + 1
#define TRANSMIT_LOG_DATA_LEN 			12

#define ATP_2 2
#define ATP_3 3

#define MAX_STORE_LOG 	40

#define MAX_TRANSMIT_LOG 	40
#define MAX_PARA_LEN		(9 + 1 + MAX_TRANSMIT_LOG*12 + 1)

#define DUMMY 0

#define ATP_PACKET_IN_LEN 450

#define ATP_VERSION_LEN 		7
#define ATP_COMDITY_CODE_LEN 	9
#define ATP_PATIENT_ID_LEN 		10


#define TIMER_WAITING 10
#define QUEUE_WAITING 500/portTICK_PERIOD_MS

#define YEAR_SCALE 100

#define F_BYTE 0x46
#define D_BYTE 0x44
#define H_BYTE 0x48
#define T_BYTE 0x54

#define MODEL_CODE 			0x01
#define MODEL_CODE_ATP_02 	0x01
#define MODEL_CODE_ATP_W 	0x02
#define MODEL_CODE_ATP_03 	0x03

#define DESTINATION 		0x01
#define NONE_DATA 			0x00

#define SET_CALENDAR_DATA_LEN 0x06
#define GET_LOG_DATA_DATA_LEN 0x05

#define MAJOR_NUMBER_MAX 0x99
#define MINOR_NUMBER_MAX 0x99
#define YEAR_NUMBER_MAX  99

#define FIRST_DAY_OF_MONTH 	1
#define LAST_DAY_OF_MONTH 	31

#define MAX_HOUR 	23
#define MAX_MIN 	59
#define MAX_SEC 	59

#define ID_NUMBER_DATA_MAX 				99
#define DATE_DATA_LEN 					5

#define ATP_03_DEV_INFO_BYTE 			0x03
#define ATP_03_COMDITY_CODE 			"651085390\0"
#define ATP_02_COMDITY_CODE 			"650214030\0"

#define ATP_BATTERY_STATUS_BIT 		0x01
#define ATP_RTC_STATUS_BIT			0x02
#define ATP_CHARGING_STATUS_BIT		0x04

#define YEAR_OFFSET 				2000
#define FIRST_LOG_NUMBER 			1
#define MAX_PACKET_NO 				0XFF

typedef enum
{
    StartOfATPId = 0,
    ATPIndentificationInfoId,  //1
    ATPDeviceInfoId,           //2
    ATPIdNumberId,             //3
    ATPGetCalendarId,          //4
    ATPSetCalendarId,          //5
    ATPLogQuantityId,          //6
    ATPLogDatanEndId,          //7
    ATPLogDatanTopId,          //8
    ATPLognCountId,            //9
    ATPLogForSearchAcqId,      //10
    ATPLogQuantityFH310Id,     //11
    ATPStorageOfOxygenId,      //12
    EndOfATPId
}E_ATPPacketId;

typedef enum
{
    ATP_Wait_SignatureF,
    ATP_Wait_SignatureD,
    ATP_Wait_SignatureH,
    ATP_Wait_ModelCode,
    ATP_Wait_SequenceNumber,
    ATP_Wait_PacketType,
    ATP_Wait_Destination,
    ATP_Wait_DataLengthLower,
    ATP_Wait_DataLengthHigh,
    ATP_Wait_Status,
    ATP_Wait_EquipmentCode,
    ATP_Wait_MajorNumber,
    ATP_Wait_MinorNumber,
    ATP_Wait_SerialNumber,
    ATP_Wait_YearUpper,
    ATP_Wait_YearLower,
    ATP_Wait_Month,
    ATP_Wait_Day,
    ATP_Wait_DeviceInforLower,
    ATP_Wait_DeviceInforHigh,
    ATP_Wait_Device_Status,
    ATP_Wait_IdNumberId,
    ATP_Wait_GetYearId,
    ATP_Wait_GetMonthId,
    ATP_Wait_GetDayId,
    ATP_Wait_GetHour,
    ATP_Wait_GetMinute,
    ATP_Wait_GetSecond,
    ATP_Wait_GetQuantityData,
    ATP_Wait_GetLogData,
    ATP_Wait_GetNumberOfData,
    ATP_Wait_GetIdNumberData,
    ATP_Wait_GetDateData,
    ATP_Wait_GetSpO2Data,
    ATP_Wait_GetPulseData,
    ATP_Wait_CheckSumData,
    ATP_Wait_GetQuantityFH310,
    ATP_Wait_CheckSum
}E_ATPCurrentState;

typedef struct
{
	bool isBatteryAbnomal; //1: abnormal
	bool isRTCAbnormal; // 1: abnormal
	bool isCommunication; //1: abnormal
	bool isChaging; //1: Charging
}ATP_STATUS;

typedef struct
{
	uint8_t type; //2:ATP-02 3:ATP-03
	uint8_t version[ATP_VERSION_LEN]; //anypal version
	uint8_t ComdityCode[ATP_COMDITY_CODE_LEN]; //anypal commodity code
	uint8_t patientID[ATP_PATIENT_ID_LEN]; //patient ID
}ATP_INFO;

typedef struct
{
	ATP_INFO info; // anypal info
	ATP_STATUS status; //anypal status
}Virtual_AnyPal;

void anypal_ResetAllAnypalStateWhenOVCR(void);

void anypal_ResetAllAnypalState(bool isDeleteAnypal);

void anypal_TransferPacket(E_ATPPacketId sendPacketId);

void anypal_ActiveAlarmCommunication(void);

void anypal_StartTurnONVBusTimer(void);

void anypal_StartAlarmOverCurrentOffTimer(void);

void anypal_StopAlarmOverCurrentOffTimer(void);

void anypal_RetransferPacket(E_ATPPacketId sendPacketId);

void anypal_CreateTimer(void);

void anypal_StartTimerAnypalCycleTimer(void);

void anypal_StopAllAnypalTimer(void);

void anypal_ReceivePacketFromATP(uint8_t receicePacket);

bool anypal_GetIsTurnOffVBus(void);

void anypal_SendRequestSwicthVbus(bool isTurnOn);

void anypal_SwitchPushOutAnypalTimer(bool isStartTimer);

void anypal_SwitchReconnectErrTimer(bool isStartTimer);

void anypal_SendGettingSpO2PulseDataStatus(bool isGetting);

void anypal_SetCalendarUpdate(void);
#endif /* INC_ANYPALHANDLER_H_ */
