/*
 * RS485Sensoe.c
 *
 *  Created on: 11-10-2018
 *      Author: haxua
 */
#include <board/uart.h>
#include <ipcTask/IpcInterface.h>
#include <IpcCommonTypes.h>
#include "RS485Sensor.h"
#include "DebugTrace.h"
#include "Delay.h"
#include "board/board.h"
#include "fsl_gpio.h"
#define UART_RTS_PORT             GPIO1             //PORT to control send data
#define UART_RTS_PIN              (29)               //PIN to control send data
//define for UART4 - Proxymal flow sensor SFM300
#define PROXY_FS_UART              4
#define PROXY_BAUD_RATE                   (115200U)          //Baud rate

//define for SHDLC protocol( Protocol of Flow sensor)
#define START_BYTE                  (0x7E)          //Start byte of SHDLC Frame
#define STOP_BYTE                   (0x7E)          //Stop byte of SHDLC Frame
#define START_STOP_BYTE             (0x7E)          //Special byte of SHDLC Frame
#define XON_BYTE                    (0x11)          //Special byte of SHDLC Frame
#define XOFF_BYTE                   (0x13)          //Special byte of SHDLC Frame
#define ESCAPE_BYTE                 (0x7D)          //Special byte of SHDLC Frame

#define ESCAPE_ADD_BYTE             (0x5D)          //Special byte of SHDLC Frame
#define START_STOP_ADD_BYTE         (0x5E)          //Special byte of SHDLC Frame
#define XON_ADD_BYTE                (0x31)          //Special byte of SHDLC Frame
#define XOFF_ADD_BYTE               (0x33)          //Special byte of SHDLC Frame

#define BIT_WISE                    (0xFF)          //BIT_WISE to calculate CheckSume
#define BUFFER_SIZE					270
#define TIME_TO_SEND_ONE_BYTE 	    85 //80 uc

//User for SHDLC Protocol
//static uint8_t Rx_Buff[UART_RRB_SIZE], Tx_Buff[UART_SRB_SIZE];
typedef struct
{
	//$COMMON.ATTRIBUTE$
	//    Name: Address
	//    Description: Device address of slave which is send
	//    Range: 0 - 255
	uint8_t Address;

	//$COMMON.ATTRIBUTE$
	//    Name: CMD
	//    Description: Commend ID
	//    Range: None
	uint8_t CMD;

	//$COMMON.ATTRIBUTE$
	//    Name: LengTH
	//    Description: Length of data fields
	//    Range: None
	uint8_t LengTH;

	//$COMMON.ATTRIBUTE$
	//    Name: Data
	//    Description: Data
	//    Range: None
	uint8_t Data[255];

}ProxySensorSendFrameData;

typedef struct
{
	//$COMMON.ATTRIBUTE$
	//    Name: Address
	//    Description: Device address of slave which is receive
	//    Range: 0 - 255
	uint8_t Address;

	//$COMMON.ATTRIBUTE$
	//    Name: CMD
	//    Description: Commend ID
	//    Range: None
	uint8_t CMD;

	//$COMMON.ATTRIBUTE$
	//    Name: State
	//    Description: The slave sends a state byte to report execution errors
	//    Range: None
	uint8_t State;

	//$COMMON.ATTRIBUTE$
	//    Name: LengTH
	//    Description: Length of data fields
	//    Range: None
	uint8_t LengTH;

	//$COMMON.ATTRIBUTE$
	//    Name: Data
	//    Description: Data
	//    Range: None
	uint8_t Data[255];

}ProxySensorRecFrameData;

static ProxyStatus gs_Status = eNotCalibrated;
static ProxyActions gs_ProxyTitle = eGetProxyStatus;
static uint8_t gs_StatusFromSensor=0x00;
static uint8_t gs_DataLengthReceiveFromSensor=0x00;
static bool gs_IsGotVersionStr=false;
static uint8_t gs_VersionStr[7]={'\0'};
static bool gs_IsOkToShutdown = false;
static int32_t gs_CurrentReading = 0;
static bool gs_IsGetOffSetActive = false;
static uint8_t gs_GetOffSetCnt = 0;
static float gs_GetOffSetSum = 0;
static float gs_Offset = 0;

/**
 * Calculate CRC8 for provided buffer
 * @param pBuf Buffer that function will calculate CRC for
 * @param uSize Size of buffer
 * @return uint8_t InvertLeastSignificantByte
 */
static uint8_t RS485ProxySensor_CalcCRC(uint8_t* pBuf, uint32_t uSize)
{
	uint64_t SumAllByte = 0;
	uint8_t LeastSignificantByte = 0;
	uint8_t InvertLeastSignificantByte = 0;

	//sum all bytes in the frame content (from and including Adr to and including Data)
	for(unsigned int i = 0; i < uSize; ++i){
		SumAllByte = SumAllByte + pBuf[i];
	}

	//take the least significant byte of this sum
	LeastSignificantByte = (uint8_t)(SumAllByte & BIT_WISE);

	//invert the least significant byte
	InvertLeastSignificantByte = LeastSignificantByte ^ BIT_WISE;

	return InvertLeastSignificantByte;
}

/**
 * Generate Frame to send to Proxy sensor
 * @param data
 * @param FrameData
 * @param SizeOfFrameData
 */
static void RS485ProxySensor_GenerateFrameData(ProxySensorSendFrameData *data, uint8_t *FrameData, uint32_t* SizeOfFrameData)
{
	const uint32_t THE_NUMBER_OF_START_BLOCK = 1;
	//check input and output data is valid
	ASSERTION(NULL != data);
	ASSERTION(NULL != FrameData);
	uint16_t byte = 0;

	//initialize a temporary buffer to store Frame data
	uint8_t TempFrameData[BUFFER_SIZE];

	//The byte 0x7E marks the beginning of the frame
	TempFrameData[byte]=(START_BYTE);
	byte++;
	//Device Address of the slave
	TempFrameData[byte]=(data->Address);
	byte++;
	//Command ID of the command
	TempFrameData[byte]=(data->CMD);
	byte++;
	//Indicates the number of bytes sent in the Data block
	TempFrameData[byte]=(data->LengTH);
	byte++;
	//The data format depends on the command
	for(uint32_t i = 0; i < data->LengTH; ++i)
	{
		TempFrameData[byte]=(data->Data[i]);
		byte++;
	}

	//initialize a buffer to calculate Check sum
	//Size of buffer
	const uint32_t BufferSize = data->LengTH+3;
	uint8_t TempBuff[BufferSize];
	for(uint32_t i = 0; i < BufferSize; ++i)
	{
		TempBuff[i] = TempFrameData[THE_NUMBER_OF_START_BLOCK + i];
	}
	//Check sum over the frame content
	TempFrameData[byte]=RS485ProxySensor_CalcCRC(TempBuff,BufferSize);
	byte++;
	//The second byte 0x7E marks the end of the frame
	TempFrameData[byte]=STOP_BYTE;

	//Update size of Frame Data to output parameter
	*SizeOfFrameData = byte+1;

	//Copy Frame Data to output parameter
	for(uint32_t i = 0; i < *SizeOfFrameData; ++i)
	{
		FrameData[i] = TempFrameData[i];
	}
}

/**
 * Convert Byte Array to send to sensor
 * @param pSourceByte
 * @param uSourceSize
 * @param pDesByte
 * @param uDesSizeByte
 */
static void RS485ProxySensor_ConvertByteArrayToSend (uint8_t* pSourceByte,
		uint32_t uSourceSize,
		uint8_t* pDesByte,
		uint64_t* uDesSizeByte)
{

	ASSERTION(NULL != pSourceByte);
	ASSERTION(NULL != pDesByte);

	const uint8_t NUMBER_OF_START_BYTE = 1;
	const uint8_t NUMBER_OF_STOP_BYTE = 1;
	uint16_t byte = 0;
	uint8_t TempBuf[uSourceSize*2];
	//Ignore START_BYTE(0x7E);
	TempBuf[byte]=(START_BYTE);
	byte++;
	//Convert data if it has special bytes
	for(uint32_t i = (0 + NUMBER_OF_START_BYTE); i < (uSourceSize - NUMBER_OF_STOP_BYTE); ++i)
	{
		switch (pSourceByte[byte])
		{
		//Replace  0x7E   =   0x7D, 0x5E
		case START_STOP_BYTE:
			TempBuf[byte]=(ESCAPE_BYTE);
			byte++;
			TempBuf[byte]=(START_STOP_ADD_BYTE);
			byte++;
			break;
			//Replace 0x7D  =  0x7D, 0x5D
		case ESCAPE_BYTE:
			TempBuf[byte]=(ESCAPE_BYTE);
			byte++;
			TempBuf[byte]=(ESCAPE_ADD_BYTE);
			byte++;
			break;
			//Replace 0x11  =  0x7D, 0x31
		case XON_BYTE:
			TempBuf[byte]=(ESCAPE_BYTE);
			byte++;
			TempBuf[byte]=(XON_ADD_BYTE);
			byte++;
			break;
			//Replace 0x13  =  0x7D, 0x33
		case XOFF_BYTE:
			TempBuf[byte]=(ESCAPE_BYTE);
			byte++;
			TempBuf[byte]=(XOFF_ADD_BYTE);
			byte++;
			break;
			//IF it is not a special byte
		default:
			TempBuf[byte]=(pSourceByte[i]);
			byte++;
			break;
		}
	}

	//Ignore STOP_BYTE(0x7E)
	TempBuf[byte]=(STOP_BYTE);

	//Update size of pDesByte
	*uDesSizeByte = byte+1;

	//Copy data to pDesByte
	for(uint32_t i = 0; i < *uDesSizeByte; ++i)
	{
		pDesByte[i] = TempBuf[i];
	}

}

/**
 * Send data to Proxy sensor
 * @param data Pointer to data buffer
 * @param bytes Number of Byte to be sent
 */
static void RS485ProxySensor_SendDataToUART (void* data, int bytes)
{
	GPIO_PinWrite(UART_RTS_PORT, UART_RTS_PIN,1U);
	uart_Write(PROXY_FS_UART, data, bytes);
	GPIO_PinWrite(UART_RTS_PORT, UART_RTS_PIN,0U);
}

/**
 * Read data from Proxy sensor
 * @param data Pointer to data buffer
 * @param bytes Number of Byte to be read
 */
static int32_t RS485ProxySensor_ReadDataFromUART (void* pData, int iBytes)
{
	// Returns -1 if there is
	// error.
	return uart_Read(PROXY_FS_UART,pData, iBytes);
}

/**
 * Send command to put Proxy sensor in Continuous measurement mode
 */
static void RS485ProxySensor_StartContinuousMeasurement()
{
	int32_t stat = 0;
	uint8_t recData[32] = {0};

	ProxySensorSendFrameData StartContMeas;

	StartContMeas.Address = 0x00;
	StartContMeas.CMD = 0x33;
	StartContMeas.LengTH = 0x02;

	StartContMeas.Data[0] = 0x00;
	StartContMeas.Data[1] = 0x00;


	uint8_t StartContinuousMeasurement[6 + StartContMeas.LengTH];
	uint32_t SizeofStartContinuousMeasurement = 0;
	const uint16_t TempDataBufferSize=600;
	uint8_t TempData[TempDataBufferSize];
	uint64_t TempSize = 0;

	RS485ProxySensor_GenerateFrameData(&StartContMeas, StartContinuousMeasurement, &SizeofStartContinuousMeasurement);
	RS485ProxySensor_ConvertByteArrayToSend(StartContinuousMeasurement, SizeofStartContinuousMeasurement,TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	RS485ProxySensor_SendDataToUART(TempData, TempSize);

	stat = RS485ProxySensor_ReadDataFromUART(recData, 32);
	DEBUG_RS485SENSOR("REC = ");
	for(int i  = 0; i  < 32; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", recData[i]);
	}
}

/**
 * Get flow from Proxy sensor
 * @return float s_FlowValue Flow value in Lpm
 */
static float RS485ProxySensor_GetProxyFlow(void)
{
	static float s_FlowValue = 0;
	int num = 0;
	uint8_t recData[32] = {0};

	ProxySensorSendFrameData GetLastMeasure;

	GetLastMeasure.Address = 0x00;
	GetLastMeasure.CMD = 0x35;
	GetLastMeasure.LengTH = 0x00;

	uint8_t GetLastArr[6 + GetLastMeasure.LengTH];
	uint32_t SizeofGetLastArr = 0;
	const uint16_t TempDataBufferSize=600;
	uint8_t TempData[TempDataBufferSize];
	uint64_t TempSize = 0;

	RS485ProxySensor_GenerateFrameData(&GetLastMeasure, GetLastArr, &SizeofGetLastArr);
	RS485ProxySensor_ConvertByteArrayToSend(GetLastArr, SizeofGetLastArr, TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	num=RS485ProxySensor_ReadDataFromUART(recData, 32);
	RS485ProxySensor_SendDataToUART(TempData, TempSize);

	DEBUG_RS485SENSOR("REC = %d Bytes   ",num);
	for(int i  = 0; i  < num; ++i)
	{
		DEBUG_RS485SENSOR("%x ", recData[i]);
	}
	DEBUG_RS485SENSOR("\n                   %d\n", (recData[5] << 8) + recData[6]);
	uint16_t ProxyFlowValue = ((uint16_t)recData[5] << 8) + (uint16_t)recData[6];

	float realFlow=0;
	if(recData[2]==0x35 && recData[3]==0 && recData[4]==2)
	{
		realFlow = (ProxyFlowValue - 32768.0)/ 120.0;
		s_FlowValue=realFlow;
	}
	gs_StatusFromSensor = recData[3];
	gs_DataLengthReceiveFromSensor = recData[4];
	return s_FlowValue;
}

/**
 * Send command to factory reset Proxy sensor
 */
static void RS485ProxySensor_FactoryResetDevice()
{
	int num = 0;
	uint8_t recData[32] = {0};

	ProxySensorSendFrameData FactoryResetDevice;

	FactoryResetDevice.Address = 0x00;
	FactoryResetDevice.CMD = 0x92;
	FactoryResetDevice.LengTH = 0x00;

	uint8_t FactoryResetDeviceBuff[6 + FactoryResetDevice.LengTH];
	uint32_t SizeofFactoryResetDevice = 0;
	const uint16_t TempDataBufferSize=600;
	uint8_t TempData[TempDataBufferSize];
	uint64_t TempSize = 0;

	RS485ProxySensor_GenerateFrameData(&FactoryResetDevice, FactoryResetDeviceBuff, &SizeofFactoryResetDevice);
	RS485ProxySensor_ConvertByteArrayToSend(FactoryResetDeviceBuff, SizeofFactoryResetDevice,TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	RS485ProxySensor_SendDataToUART(TempData, TempSize);
	//	delay_ms(1000);
	//	num = RS485ProxySensor_ReadDataFromUART(recData, 32);
	//	DEBUG_RS485SENSOR("REC = %d\n",num);
	//	for(int i  = 0; i  < num; ++i)
	//	{
	//		DEBUG_RS485SENSOR("%x \n", recData[i]);
	//	}
	delay_ms(5000);
}

/**
 * Send command to set type of sensor to SF05
 */
static void RS485ProxySensor_SetSensorTypeSF05()
{
	int num = 0;
	uint8_t recData[32] = {0};

	ProxySensorSendFrameData SetSensorType;

	SetSensorType.Address = 0x00;
	SetSensorType.CMD = 0x24;
	SetSensorType.LengTH = 0x01;
	SetSensorType.Data[0] = 2;

	uint8_t SetSensorTypeBuff[6 + SetSensorType.LengTH];
	uint32_t SizeofSetSensorType = 0;
	const uint16_t TempDataBufferSize=600;
	uint8_t TempData[TempDataBufferSize];
	uint64_t TempSize = 0;

	RS485ProxySensor_GenerateFrameData(&SetSensorType, SetSensorTypeBuff, &SizeofSetSensorType);
	RS485ProxySensor_ConvertByteArrayToSend(SetSensorTypeBuff, SizeofSetSensorType,TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	RS485ProxySensor_SendDataToUART(TempData, TempSize);
	delay_ms(1000);
	num = RS485ProxySensor_ReadDataFromUART(recData, 32);
	DEBUG_RS485SENSOR("REC = %d\n",num);
	for(int i  = 0; i  < num; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", recData[i]);
	}
}

/**
 * Send command to set sensor address to 0x40
 */
static void RS485ProxySensor_SetSensorAddress()
{
	int num = 0;
	uint8_t recData[32] = {0};

	ProxySensorSendFrameData SetSensorAdress;

	SetSensorAdress.Address = 0x00;
	SetSensorAdress.CMD = 0x25;
	SetSensorAdress.LengTH = 0x01;
	SetSensorAdress.Data[0] = 0x40;

	uint8_t SetSensorAdressBuff[6 + SetSensorAdress.LengTH];
	uint32_t SizeofSetSensorAdress = 0;
	const uint16_t TempDataBufferSize=600;
	uint8_t TempData[TempDataBufferSize];
	uint64_t TempSize = 0;

	RS485ProxySensor_GenerateFrameData(&SetSensorAdress, SetSensorAdressBuff, &SizeofSetSensorAdress);
	RS485ProxySensor_ConvertByteArrayToSend(SetSensorAdressBuff, SizeofSetSensorAdress,TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	RS485ProxySensor_SendDataToUART(TempData, TempSize);
	delay_ms(1000);
	num = RS485ProxySensor_ReadDataFromUART(recData, 32);
	DEBUG_RS485SENSOR("REC = %d\n",num);
	for(int i  = 0; i  < num; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", recData[i]);
	}
}

/**
 * Send command to set sensor voltage to 5V
 */
static void RS485ProxySensor_SetSensorVoltageTo5V()
{
	int num = 0;
	uint8_t recData[32] = {0};

	ProxySensorSendFrameData SetSensorVoltage;

	SetSensorVoltage.Address = 0x00;
	SetSensorVoltage.CMD = 0x23;
	SetSensorVoltage.LengTH = 0x01;
	SetSensorVoltage.Data[0] = 0x01;

	uint8_t SetSensorVoltageBuff[6 + SetSensorVoltage.LengTH];
	uint32_t SizeofSetSensorVoltage = 0;
	const uint16_t TempDataBufferSize=600;
	uint8_t TempData[TempDataBufferSize];
	uint64_t TempSize = 0;

	RS485ProxySensor_GenerateFrameData(&SetSensorVoltage, SetSensorVoltageBuff, &SizeofSetSensorVoltage);
	RS485ProxySensor_ConvertByteArrayToSend(SetSensorVoltageBuff, SizeofSetSensorVoltage,TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	RS485ProxySensor_SendDataToUART(TempData, TempSize);
	delay_ms(1000);
	num = RS485ProxySensor_ReadDataFromUART(recData, 32);
	DEBUG_RS485SENSOR("REC = %d\n",num);
	for(int i  = 0; i  < num; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", recData[i]);
	}
}

/**
 * Initialize Proxy sensor by sending several setting command to sensor
 */
void RS485ProxySensor_Init(void)
{
	uart_Init(PROXY_FS_UART,PROXY_BAUD_RATE);

	gpio_pin_config_t uart_rts_config_hfo = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
	GPIO_PinInit(UART_RTS_PORT, UART_RTS_PIN, &uart_rts_config_hfo);
	GPIO_PinWrite(UART_RTS_PORT, UART_RTS_PIN,0U);


//	RS485ProxySensor_FactoryResetDevice();
//	RS485ProxySensor_SetSensorTypeSF05();
//	RS485ProxySensor_SetSensorAddress();
//	RS485ProxySensor_SetSensorVoltageTo5V();
//	RS485ProxySensor_DoGetVersionStr();
//	RS485ProxySensor_StartContinuousMeasurement();
}

/**
 * Handle getting status of Proxy sensor procedure
 * @return ProxyStatus gs_Status Current status of Proxy sensor
 */
ProxyStatus RS485ProxySensor_GetCurrentStatus()
{
	static int s_ErrorStaticCollection = 0;
	if(gs_StatusFromSensor == 0x00 && gs_DataLengthReceiveFromSensor ==2)
	{
		gs_Status = eCalibrated;
		s_ErrorStaticCollection=0;
	}
	else
	{
		s_ErrorStaticCollection++;
		if(s_ErrorStaticCollection>10)
		{
			gs_Status = eSensorError;
		}
	}
	return gs_Status;
}

/**
 * Return status of Proxy sensor
 * @return ProxyStatus gs_Status Current status of Proxy sensor
 */
ProxyStatus RS485ProxySensor_GetLastStatus()
{
	return gs_Status;
}

/**
 * Handle calibration procedure
 */
void RS485ProxySensor_DoCalibration(void)
{
	if(gs_ProxyTitle!=eDoProxyCalibration)
		return;
	gs_ProxyTitle = eGetProxyStatus;
}

/**
 *     Processing: depend on current job, do the correspoding thing:
 *                 1. get version of proxy board
 *                 2. calibrate proxy board
 *                 3. clear calibration effect
 *                 4. confirm proxy board reset
 *                 5. request status of proxy board
 */
void RS485ProxySensor_CommunicateProxySensor()
{
	switch(gs_ProxyTitle){
	case eGetProxyVersion:
		RS485ProxySensor_DoGetVersionStr();
		break;
	case eDoProxyCalibration:
		RS485ProxySensor_DoCalibration();
		break;
	case eClearProxyCalibration:
		RS485ProxySensor_ClearCalibration();
		break;
	case eComfirmProxyReset:
		RS485ProxySensor_ConfirmProxyReset();
		break;
	default:
		RS485ProxySensor_GetCurrentStatus();
		break;
	}
}

/**
 * Set current action procedure of Proxy sensor
 * @param title Desired action
 */
void RS485ProxySensor_SetCurrentAction(ProxyActions title)
{
	gs_ProxyTitle = title;
}

/**
 * Get current flow value of Proxy sensor
 * @return int32_t gs_CurrentReading Flow value in Lpm*100
 */
int32_t RS485ProxySensor_GetCurrentReading()
{
	float currentValue=RS485ProxySensor_GetProxyFlow()+gs_Offset;
	if(gs_IsGetOffSetActive)
	{
		if(gs_GetOffSetCnt<10)
		{
			gs_GetOffSetSum+=currentValue;
			gs_GetOffSetCnt++;
		}
		else
		{
			gs_Offset = gs_Offset/10;
			gs_GetOffSetCnt = 0;
			gs_IsGetOffSetActive =false;
		}
	}
	gs_CurrentReading = currentValue*100;
	return gs_CurrentReading;
}

/**
 * Get previously obtained reading proxy flow value
 * @return int32_t gs_CurrentReading Flow value in Lpm*100
 */
int32_t RS485ProxySensor_GetLastReading()
{
	return gs_CurrentReading;
}

/**
 * Check whether Proxy sensor is OK to shutdown
 * @return bool gs_IsOkToShutdown Return True if it is OK to shutdown
 * 								  otherwise return False
 */
bool RS485ProxySensor_OkToShutdown()
{
	return gs_IsOkToShutdown;
}

/**
 * Handle clear calibration procedure
 */
void RS485ProxySensor_ClearCalibration()
{
	if(gs_ProxyTitle != eClearProxyCalibration)
		return;
	gs_ProxyTitle = eGetProxyStatus;
	gs_IsOkToShutdown = true;
}

/**
 * Handle confirm proxy reset procedure
 */
void RS485ProxySensor_ConfirmProxyReset()
{
	if(gs_ProxyTitle != eComfirmProxyReset)
		return;
	gs_ProxyTitle = eGetProxyStatus;

}

/**
 * Handle Get Offset procedure
 */
void RS485ProxySensor_DoGetOffset()
{
	gs_IsGetOffSetActive=true;
	gs_GetOffSetCnt=0;
	gs_GetOffSetSum=0;
}

/**
 * Send command to Proxy sensor to get version and send version to A53 Application
 */
void RS485ProxySensor_DoGetVersionStr()
{
	int num = 0;
	uint8_t recData[32] = {0};

	ProxySensorSendFrameData GetVersionStr;

	GetVersionStr.Address = 0x00;
	GetVersionStr.CMD = 0xD1;
	GetVersionStr.LengTH = 0x00;

	uint8_t GetVersionStrBuff[6 + GetVersionStr.LengTH];
	uint32_t SizeofGetVersionStr = 0;
	const uint16_t TempDataBufferSize=600;
	uint8_t TempData[TempDataBufferSize];
	uint64_t TempSize = 0;

	RS485ProxySensor_GenerateFrameData(&GetVersionStr, GetVersionStrBuff, &SizeofGetVersionStr);
	RS485ProxySensor_ConvertByteArrayToSend(GetVersionStrBuff, SizeofGetVersionStr,TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	RS485ProxySensor_SendDataToUART(TempData, TempSize);
	delay_ms(2);
	num = RS485ProxySensor_ReadDataFromUART(recData, 32);
	DEBUG_RS485SENSOR("REC = %d\n",num);
	for(int i  = 0; i  < num; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", recData[i]);
	}
	/************************************************************/
	/*gs_VersionStr[0] = Firmware Major Version Number [0-255]	*/
	/*gs_VersionStr[1] = Firmware Major Version Number [0-99]	*/
	/*gs_VersionStr[2] = Firmware in Debug State [bool]			*/
	/*gs_VersionStr[3] = Hardware Major [0-255]					*/
	/*gs_VersionStr[4] = Hardware Minor [0-99]					*/
	/*gs_VersionStr[5] = SHDLC Protocol Version Major [0-255]	*/
	/*gs_VersionStr[6] = SHDLC Protocol Version Minor [0-255]	*/
	/************************************************************/
	if(recData[2]==0xD1 && recData[3]==0 && recData[4]==7)
	{
		gs_IsGotVersionStr = true;
		Command sendMsg;
		sendMsg.id = eProxyVersionGUIEvent;
		for(int i = 0;i<7;i++)
		{
			gs_VersionStr[i]=recData[i+5];
			sendMsg.data.proxyVersion.version[i]=recData[i+5];
		}
		sendMsg.data.proxyVersion.version[7]='\0';
		ipcIf_sendCommandMsg(&sendMsg);
		gs_ProxyTitle = eGetProxyStatus;
	}
}

/**
 * Check whether it is needed to get version
 * @return bool gs_IsGotVersionStr Return false if it isn't needed
 * 									otherwise return true
 */
bool RS485ProxySensor_IsNeedToGetVersionStr()
{
	return !gs_IsGotVersionStr;
}
