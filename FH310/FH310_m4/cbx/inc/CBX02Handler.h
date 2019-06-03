#ifndef INC_CBX02HANDLER_H_
#define INC_CBX02HANDLER_H_
/********************************************************************/
/*                                                                  */
/* File Name    : CBX02Handler.h                        			*/
/*                                                                  */
/* General      : CBX02 Handler										*/
/*                 	 						                        */
/*                                                                  */
/* Product Name  : FH310                                            */
/*                                                                  */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*             2016/11/22      Linh Nguyen	  	    new file        */
/* #0001       2017/08/30      Linh Nguyen	  	    modify	        */
/*      update ATP-03 information	                        		*/
/* #0002       2016/09/05     Linh Nguyen	  	    modify          */
/*  Improve bluetooth transfer mode:								*/ 
/*	- change CBX_TurnOnTransferMode function						*/
/*	- Add cbx_CopyLogToEXTRam function								*/
/* #0003	   2017/09/15      Linh Nguyen		  	modify		    */
/*  Modify and add some define 										*/
/* #0004       2017/09/20      Linh Nguyen	  	    modify          */
/*  Remove unuse code												*/
/*  Add some define													*/
/* #0004       2017/09/28       Linh Nguyen	  	    modify          */
/*  Change function name for coding rule					     	*/
/* 		       2017/10/07       Linh Nguyen	  	    modify          */
/*  #0005 Add MEM_DATA_PARAMETER_LEN Macro					     	*/
/*  #0006 Add CALENDAR_PARAMETER_LEN Macro					     	*/
/* 		       2017/10/12       Linh Nguyen	  	    modify          */
/*  #0007 Add BUFFER_FULL Macro								     	*/
/*  #0008 Add CBX_ERR_PACKET_LENGTH Macro					     	*/
/*  #0009 Add MAIN_ERR_PACKET_LENGTH Macro					     	*/
/*  #0010 Add cbx_SendErrPacketToCbx function				     	*/
/********************************************************************/
#include <RealTimeClock.h>
#include "board.h"
#include "util.h"
#include "ModeMgr.h"
#include "LogInterface.h"

#define pdMS_TO_TICKS( xTimeInMs ) ( ( TickType_t ) ( ( ( TickType_t ) ( xTimeInMs ) * ( TickType_t ) configTICK_RATE_HZ ) / ( TickType_t ) 1000 ) )
#define ADD_INFO_MAX_LEN 524288

#define CRC_LENGTH 4
#define CRC_OFFSET_POSITION 4
#define ETX_OFFSET_POSITION 3

#define SEND_PACKET_OFFSET_LEN 8

#define RESPONSE_DELAY 	0x21
#define ERR_CRC 		0x30
#define ERR_CMD 		0x31
#define ERR_PARAM_NUM 	0x32
#define ERR_PARAM_LENTH 0x33
#define ERR_PARAM_DATA 	0x34
#define ERR_PARAM_RANGE 0x35
#define ERR_IMPOSSIBLE_EXC 					0x40
#define CBX_MAX_PACKET_LENGTH 				115
#define MAX_AWL_LOG_QUANTITY_SEND_TO_SERVER 5

#define PACKETIN_LENGTH 	130
#define PARAMETER_LENGTH 	120

#define STX_BYTE 0X02
#define ETX_BYTE 0X03
#define REGISTRATION_TYPE 0x24
#define DLE_BYTE 0x10
#define CTRL_CODE_WAIT_C2 0x59

#define C1_REGISTRATION 0x24  // $ registration type
#define C1_CALENDAR 								0X4B
#define C1_LOG_EVEN 								0x4A
#define C1_DEVICE_IDENTIFICATION_INFORMATION 		0X3F
#define C1_VERSION_INFORMATION 						0X41
#define C1_ID_NUMBER 								0X47
#define C1_CUMULATIVE_OPERATION_HOURS				0X44
#define C1_REAL_DATA								0X42
#define C1_PASS_COMMAND								0x4F
#define C1_OPERATIONAL_STATUS						0x49
#define C1_MEMORY_DATA								0x59
#define C1_LOG_QUANTITY_DEVICENO1					0x59

#define C2_VERSION_INFORMATION 						0X30
#define C2_ID_NUMBER 								0X52
#define C2_CUMULATIVE_OPERATION_HOURS				0X52
#define C2_OPERATIONAL_STATUS						0X52
#define C2_CALENDAR									0X57
#define C2_LOG_DATA 								0X44
#define C2_LOG_QUANTITY								0X4E
#define C2_DEVICE_QUANTITY							0X4E
#define C2_UPDATING_LOG								0X55
#define C2_UPDATING_CHECK_IDENTIFICATION_INFO		0X55
#define C2_UPDATING_LOG_DEVICE_NO1					0x31
#define C2_REAL_DATA								0X20
#define C2_PASS_COMMAND								0x31

#define SP_BYTE										0x20

#define COMMUDITY_CODE_LENG 	16
#define DEVICE_NAME_LENG		16

#define LOG_DATA_PARAMETER_LENG 12
#define LOG_INDEX_POSITION		4


#define BT_ADDESS_LEN			12
#define PIN_CODE 				"80505691        "
#define FH310_COMMUDITY_CODE 	"251178080       "
#define FH310_DEVICE_NAME 		"FH-310          "

#define ATP03_COMMUDITY_CODE 	"651085390       "
#define ATP03_DEVICE_NAME 		"ATP-03          "

#define ATP02_COMMUDITY_CODE 	"650214030       "
#define ATP02_DEVICE_NAME 		"ATP-02          "

#define LOG_QUANTITY_DEVICENO1 	"00000000"

#define QUEUE_WAIT_500MS 	500
#define QUEUE_WAIT_1000MS 	1000

#define FLOW_RATE_SETTING_SCALE 5
#define SEND_MESSAGE_FAIL -1
#define BUFFER_FULL 0

#define COMMUNICATION_ERROR_TIMER 			((5*60 + 30)*1000) //5.5min
#define COMMUNICATION_ERROR_TIMER_ID 		0

#define CALENDAR_LEN 		18
#define MONITOR_DATA_LEN 	5
#define PRESSURE_SCALE_VALUE 	100.00
#define TEMP_SCALE_VALUE 		100.00
#define YEAR_SCALE			 	100

#define UPDATING_LOG_DEVICE_NO1_ADDED_INFOR_LEN 26
#define ID_NUMBER_ADDED_INFOR_LEN 10

#define FH310_PATIENT_HOUR_LEN 	13
#define MAX_NUMBER_OF_LOG_DATA 	32
#define LOG_DATA_LEN 			25
#define CURRENT_LOG_INDEX_LEN 	8
#define PASS_PARAMETER_LEN 		20
#define MEM_DATA_PARAMETER_LEN 	16
#define CALENDAR_PARAMETER_LEN 	12

#define CRC_SIZE 				4
#define MAX_LOG_REQUEST 20000

#define IDENTIFICATION "01"
#define PATIENTHOUR "01"
#define UNITHOUR "02"
#define CUMULATIVE_OPERATION "03"
#define LOG_DEVICE_NO1 "0001"
#define FH310_USAGE_LOG "03"
#define ANYPAL_PULSE_LOG "04"
#define VERSION_INFO "01"

#define CBX_ERR_PACKET_LENGTH 8
#define MAIN_ERR_PACKET_LENGTH 3

//CBX SENDPACKET RESULT struct
typedef struct
{
	int16_t result; //CBX SENDPACKET RESULT
	uint8_t mC1; //C1
}CBX_SENDPACKET_RESULT_t;

typedef enum {
    B_Wait_STX=0,
    B_Wait_CtrlCode,
    B_Wait_C1,
    B_Wait_C2,
    B_Wait_Parameter,
    B_Wait_ETX,
    B_Wait_CRC
}E_CBX02CurrentState;

typedef enum {
	CBX_WAIT_STX,
	CBX_WAIT_ETX,
	CBX_WAIT_CRC
} E_CBX02ReceivePacketState;

typedef enum {
    StartOfBtID = 0, //0
	BTInfoID,
    IdentificationInfoID,
    VersionInfoID,
    IdNumberID,
    CumulativeOperationID,
    Fh310PatientHourID,
    Fh310UnitHourID,
    CumulativeHourOfDeviceNo1ID,
    UpdatingCheckID,
    DeviceQuantityID,
    CalendarSetupID,
    Fh310LogQuantityID,
    DeviceQuantityUpdatingLogID,
    Fh310LogDataID,
    UpdatingLogDeviceNo1ID,
    DeviceQuantityDeviceNo1ID,
    IdentificationInfoDeviceNo1ID,
    IdNumberDeviceNo1ID,
    VersionInfoDeviceNo1ID,
    UnitHourDeviceNo1ID,
    PatientHourDeviceNo1ID,
    LogQuantityDeviceNo1ID,
    LogQuantityFh310UsageLogID,
    LogQuantityAnypalPulseLogID,
    LogDataFh310UsageLogID,
    LogDataAnypalPulseLogID,
    RealDataID,
    Fh310OperationStatusID,
    EndOfBtID
}E_BtReturnPacketId;

int32_t cbx_SendErrPacketToCbx(uint8_t command, uint8_t ErrStatus);

void cbx_CopyLogToEXTRam(uint16_t dest, FH310_LOG_T *source, uint16_t NoLog, LogType logType);

void cbx_TurnOnTransferMode(NUM_OF_LOG_t NoLog);

void cbx_TurnOffTransferMode(void);

void cbx_isPacketFormRS232(bool isPacketFormRS232);

void cbx_CreateTimer(void);

void cbx_ReceivePacketFromCbx(uint8_t* ch, uint8_t packketLen);

void cbx_SetGettingSpO2Data(bool value);

#endif /* INC_CBX02HANDLER_H_ */
