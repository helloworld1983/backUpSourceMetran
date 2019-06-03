/*
 * RS485Sensoe.c
 *
 *  Created on: 11-10-2018
 *      Author: haxua
 */
#include "RS485Sensor.h"
#include "DebugTrace.h"
#include "Delay.h"
#include "uart.h"
#include "board/board.h"
#include "ipc/IpcInterface.h"
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
static uint8_t RS485Sensor_CalcCRC(uint8_t* pBuf, uint32_t uSize)
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
static void RS485Sensor_GenerateFrameData(ProxySensorSendFrameData *data, uint8_t *FrameData, uint32_t* SizeOfFrameData)
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
	TempFrameData[byte]=RS485Sensor_CalcCRC(TempBuff,BufferSize);
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
static void RS485Sensor_ConvertByteArrayToSend (uint8_t* pSourceByte,
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
static void RS485Sensor_SendDataToUART (void* data, int bytes)
{
	uart_Write(PROXY_FS_UART, data, bytes);
}
static int32_t RS485Sensor_ReadDataFromUART (void* pData, int iBytes)
{
	// Returns -1 if there is
	// error.
	return uart_Read(PROXY_FS_UART,pData, iBytes);
}
static void RS485Sensor_StartContinuousMeasurement()
{
	int32_t stat = 0;
	uint8_t recData[32] = {0};

	ProxySensorSendFrameData StartContMeas;

	StartContMeas.Address = 0x00;
	StartContMeas.CMD = 0x33;
	StartContMeas.LengTH = 0x02;

	StartContMeas.Data[0] = 0x00;
	StartContMeas.Data[1] = 0x02;


	uint8_t StartContinuousMeasurement[6 + StartContMeas.LengTH];
	uint32_t SizeofStartContinuousMeasurement = 0;
	const uint16_t TempDataBufferSize=600;
	uint8_t TempData[TempDataBufferSize];
	uint64_t TempSize = 0;

	RS485Sensor_GenerateFrameData(&StartContMeas, StartContinuousMeasurement, &SizeofStartContinuousMeasurement);
	RS485Sensor_ConvertByteArrayToSend(StartContinuousMeasurement, SizeofStartContinuousMeasurement,TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	//vTaskSuspendAll();
	RS485Sensor_SendDataToUART(TempData, TempSize);
	//xTaskResumeAll();

	stat = RS485Sensor_ReadDataFromUART(recData, 32);
	DEBUG_RS485SENSOR("REC = ");
	for(int i  = 0; i  < 32; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", recData[i]);
	}
}

void RS485Sensor_Init(void)
{
	uart_Init(PROXY_FS_UART,PROXY_BAUD_RATE);
	RS485Sensor_StartContinuousMeasurement();

}
float RS485Sensor_GetProxyFlow(void)
{
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

	RS485Sensor_GenerateFrameData(&GetLastMeasure, GetLastArr, &SizeofGetLastArr);
	RS485Sensor_ConvertByteArrayToSend(GetLastArr, SizeofGetLastArr, TempData,&TempSize);
	DEBUG_RS485SENSOR("SEND = ");
	for(uint64_t i  = 0; i  < TempSize; ++i)
	{
		DEBUG_RS485SENSOR("%x \n", TempData[i]);
	}
	num=RS485Sensor_ReadDataFromUART(recData, 32);
	RS485Sensor_SendDataToUART(TempData, TempSize);

	DEBUG_RS485SENSOR("REC = %d Bytes   ",num);
	for(int i  = 0; i  < num; ++i)
	{
		DEBUG_RS485SENSOR("%x ", recData[i]);
	}
	DEBUG_RS485SENSOR("\n                   %d\n", (recData[5] << 8) + recData[6]);
	uint16_t ProxyFlowValue = ((uint16_t)recData[5] << 8) + (uint16_t)recData[6];
	// flow = (measure - offset) / scale
	// offset = 32 000
	// scale (air, N2) = 140
	// scale (02) = 142.8
	float realFlow=0;
	if(ProxyFlowValue!=0)
		realFlow = (ProxyFlowValue - 32768.0)/ 120.0;

	return realFlow;
}

