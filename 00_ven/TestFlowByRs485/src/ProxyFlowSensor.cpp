//******************************************************************************//
//$COMMON.CPP$
//   File Name:  ProxyFlowSensor.CPP
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file contains the ProxyFlowSensor class which
//          represents a TSI Flow Sensor.
//
//   Interfaces:
//          Sensor base class
//          BinarySearch - utility routine
//          interpolate  - utility routine
//          ipow         - utility routine
//
//   Restrictions:
//          Flow sensor input signals are assumed to have range of 0-5 Volts.
//          A2D output range for the input signals is assumed to be 0 - 4095.
//******************************************************************************
#include "ProxyFlowSensor.h"

#include <vector>

#include "Standard.h"
#include "board.h"

//define for UART3 - RS485
#define FS_UART_CHANEL              LPC_USART3
#define UART_SRB_SIZE               (128)             //Send
#define UART_RRB_SIZE               (32)              //Receive
#define BAUD_RATE                   (115200)          //Baud rate

#define EN_SEND_DATA_PORT           (0x9)             //PORT to control send data
#define EN_SEND_DATA_PIN            (2)               //PIN to control send data
#define EN_GPIO_SEND_DATA_PORT_NUM  (4)               //GPIO PORT to control send data
#define EN_GPIO_SEND_DATA_PIN_NUM   (14)              //GPIO PIN to control send data

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

//Use for Ring Buffer
static RINGBUFF_T gs_txringFc, gs_rxringFc;
static uint8_t gs_rxbuff[UART_RRB_SIZE], gs_txbuff[UART_SRB_SIZE];

//User for SHDLC Protocol
static uint8_t Rx_Buff[UART_RRB_SIZE], Tx_Buff[UART_SRB_SIZE];

#define ASSERTION(...)  //delete later

ProxyFlowSensor *ProxyFlowSensor::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing:
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants
//      access to the object, it invokes S_GetInstance(),
//      which returns a pointer to the object.
//
//      If the object has not already been created, this method instantiates
//      it and returns a pointer to the object.  If the object has already been
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      * - pointer to the object instantiated from this class
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
ProxyFlowSensor* ProxyFlowSensor::S_GetInstance (void)
{
    //if objects do not exit, create it
    if(NULL == S_Instance)
    {
        S_Instance = new ProxyFlowSensor();
    }

    //check object is null or not
    ASSERTION(S_Instance != NULL);

    return (S_Instance);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProxyFlowSensor()
//
//    Processing:
//      This operation is the Console Task constructor.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:
//
/******************************************************************************/
ProxyFlowSensor::ProxyFlowSensor(void)
{

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Init
//
//    Processing: initialize for proxyflowsensor - RS485 Sensor uses following settings
//          115’200 baud (May be configured to baudrates between 1200 and 115’200)
//          Half Duplex
//          8 Data bits, Least-significant bit (LSb) first
//          No Parity
//          1 Stop bit
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
void ProxyFlowSensor::Init (void)
{
    //Size of each buffer item size
    const uint8_t BUFFER_ITEM_SIZE = 1;

    //the priority of an interrupt FS_UART_CHANEL(UART3)
    const uint8_t PRIORITY = 2;

    //Initialize for uart3 using for RS485
    Board_UART_Init(FS_UART_CHANEL);

    /* Setup UART for 115.2K8N1 */
    Chip_UART_DeInit(FS_UART_CHANEL);
    Chip_UART_Init(FS_UART_CHANEL);
    Chip_UART_SetBaud(FS_UART_CHANEL, BAUD_RATE);
    Chip_UART_ConfigData(FS_UART_CHANEL, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT);
    Chip_UART_SetupFIFOS(FS_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
    Chip_UART_TXEnable(FS_UART_CHANEL);

    //Before using the ring buffers, initialize them using the ring buffer init function
    RingBuffer_Init(&gs_rxringFc, gs_rxbuff, BUFFER_ITEM_SIZE, UART_RRB_SIZE);
    RingBuffer_Init(&gs_txringFc, gs_txbuff, BUFFER_ITEM_SIZE, UART_SRB_SIZE);

    //Reset and enable FIFOs, FIFO trigger level 3 (14 chars)
    Chip_UART_SetupFIFOS(FS_UART_CHANEL, (UART_FCR_FIFO_EN | UART_FCR_RX_RS |
            UART_FCR_TX_RS | UART_FCR_TRG_LEV3));

    //Enable receive data and line status interrupt
    Chip_UART_IntEnable(FS_UART_CHANEL, (UART_IER_RBRINT | UART_IER_RLSINT));

    //preemption = 1, sub-priority = 3
    NVIC_SetPriority(USART3_IRQn, PRIORITY);
    NVIC_EnableIRQ(USART3_IRQn);

    //Initialize for Control PIN's RS485
    //By default, it's on LOW(0) to receive all data from another RS485 device
    //when You want to send data, set this PIN HIGH(1), after finish sending data, set it LOW

    //Set pin PF_6 using function 4:GPIO7[20] General purpose digital input/output pin.
    Chip_SCU_PinMuxSet(EN_SEND_DATA_PORT, EN_SEND_DATA_PIN, (SCU_MODE_FUNC0));
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, EN_GPIO_SEND_DATA_PORT_NUM, EN_GPIO_SEND_DATA_PIN_NUM);

    //Set pin LOW
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,EN_GPIO_SEND_DATA_PORT_NUM, EN_GPIO_SEND_DATA_PIN_NUM,false);

    //TEst ConvertByteSruffing
    //    int8_t *test = ConvertByteSruffing(0x7e);
    //    Board_UARTDebug("%x %x \n", test[0], test[1]);
    //    test = ConvertByteSruffing(0x7D);
    //    Board_UARTDebug("%x %x \n", test[0], test[1]);
    //    test = ConvertByteSruffing(0x11);
    //    Board_UARTDebug("%x %x \n", test[0], test[1]);
    //    test = ConvertByteSruffing(0x13);
    //    Board_UARTDebug("%x %x \n", test[0], test[1]);
    //    test = ConvertByteSruffing(0x55);
    //    Board_UARTDebug("%x %x \n", test[0], test[1]);

    //test COnverByteArrayToSendData
    //    uint8_t test[8] = {0x7E, 0x11, 0x33, 0x02, 0x00, 0xFA, 0xBF, 0x7E};
    //    uint8_t test[8] = {0x7E, 0x11, 0x33, 0x02, 0x00, 0xFA, 0xBF, 0x7E};
    //    uint8_t test[7] = {0x7E, 0x00, 0xD0, 0x01, 0x01, 0x2D, 0x7E};
    //
    //    uint8_t data[10] = {0};
    //    uint32_t sizeData;
    //    uint8_t nhan[50];
    //    uint32_t sizeNhan = 50;

    //
    //    if(ConvertByteArrayToSend(test, 7, data, sizeData))
    //    {
    //        for(uint32_t i = 0; i < sizeData; ++i)
    //        {
    //            Board_UARTDebug("%x ", data[i]);
    //        }
    //        Board_UARTDebug("\n\n");
    //    }
    //
    //    SendDataToUART(data, sizeData);

    ProxySensorSendFrameData dt;
    dt.Address = 0x7e;
    dt.CMD = 0x7d;
    dt.LengTH = 0x03;
    dt.Data[0] = 0x11;
    dt.Data[1] = 0x13;
    dt.Data[2] = 0xAA;
    uint8_t test[6 + dt.LengTH] = {0};
    uint32_t sizeT;

    GenerateFrameData(&dt,test,sizeT);

    for(uint32_t i = 0; i < sizeT; ++i)
    {
        Board_UARTDebug("%x ", test[i]);
    }
    Board_UARTDebug("\n");

    uint8_t Final[100] = {0};
    uint32_t SizeFinal;

    ConvertByteArrayToSend(test, sizeT, Final, SizeFinal);
    for(uint32_t i = 0; i < SizeFinal; ++i)
    {
        Board_UARTDebug("%x ", Final[i]);
    }
    Board_UARTDebug("\n");

    ProxySensorRecFrameData Recdata;



    uint8_t Test[14] = {0x7e, 0x7d, 0x5e, 0x7d, 0x5d, 0x00, 0x3, 0x7d, 0x31, 0x7d, 0x33, 0xaa, 0x33, 0x7e};

    if(ConvertReceiveDataToFrameData(Test, 14, &Recdata))
    {
        Board_UARTDebug("%x %x %x %x\n", Recdata.Address, Recdata.CMD, Recdata.LengTH, Recdata.State);
        for(uint32_t i = 0; i < Recdata.LengTH; ++i)
        {
            Board_UARTDebug("%x ", Recdata.Data[i]);
        }
        Board_UARTDebug(" END DATA\n");
    }
    else
    {
        Board_UARTDebug("FAIL\n");
    }





}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: UART3_IRQHandler
//
//    Processing: handle interrupt for uart3
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: Need to extern keyword for linking function from C language
//
/******************************************************************************/
//extern "C" void UART3_IRQHandler(void)
//{
//    /* Want to handle any errors? Do it here. */
//    /* Use default ring buffer handler. Override this with your own
//       code if you need more capability. */
//    Chip_UART_IRQRBHandler(FS_UART_CHANEL, &gs_rxringFc, &gs_txringFc);
//}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SendDataToUART
//
//    Processing: After, we generate Frame data from Command ID of Flow sensor protocol
//                  Send frame data to UART
//             We have a SEND PIN, if you want to send data, you must set it HIGH,
//             By default, SEND PIN is always LOW(Always ready to receive data from another devices)
//             In process to send data, we need a time delay to finish transferring data,
//             But After finishing send data, SEND PIN must be LOW instantly, to make sure
//             don't miss receiving data
//
//
//    Input Parameters: void *data - data to send
//                      int byte = number of byte to send
//
//    Output Parameters: None
//
//    Return Values: NumberOfByteToSend
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
uint32_t ProxyFlowSensor::SendDataToUART (void* data, int bytes)
{
    //initialize number of bytes that UART sends
    uint32_t NumberOfByteToSend = 0;

    //initialize time delay, it is calculated by DELAY = 1/baud rate
    //DELAY = Time delay need to transfer 1 byte.
    const uint8_t DELAY = 90;   //Us

    //Enable PIN to send Data
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,EN_GPIO_SEND_DATA_PORT_NUM, EN_GPIO_SEND_DATA_PIN_NUM,true);

    //Send data
    NumberOfByteToSend = Chip_UART_SendRB(FS_UART_CHANEL, &gs_txringFc, data, bytes);

    //Delay time to send data
    Board_DelayUs(DELAY * bytes);

    //Disable SEND PIN
    Chip_GPIO_WritePortBit(LPC_GPIO_PORT,EN_GPIO_SEND_DATA_PORT_NUM, EN_GPIO_SEND_DATA_PIN_NUM,false);

    //If the number of bytes returned is less than the
    // number of bytes to send, the ring buffer is considered full.
    return NumberOfByteToSend;

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ReadDataFromUART
//
//    Processing: handle interrupt for uart3
//
//    Input Parameters: void *data - data to receive
//                      int byte = number of byte to send
//
//    Output Parameters: None
//
//    Return Values: Returns 0 if there is no data in buffer
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: Need to extern keyword for linking function from C language
//
/******************************************************************************/
int ProxyFlowSensor::ReadDataFromUART (void* pData, int iBytes)
{
    // Returns 0 if there is
    // no data in the ring buffer.
    return Chip_UART_ReadRB(FS_UART_CHANEL, &gs_rxringFc, pData, iBytes);
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CalcCRC
//
//    Processing: Calculate CRC of frame data
//                The checksum is calculated over the frame content in the following way:
//          + Sum all bytes in the frame content (from and including Adr to and including Data)
//          + Take the least significant byte of this sum
//          + Invert the least significant byte
//
//  frame content: [0x00, 0x33, 0x02, 0x00, 0xFA]
//  sum all bytes: 0x00 + 0x33 + 0x02 + 0x00 + 0xFA = 0 + 51 + 2 + 0 + 250 = 303 = 0x12F
//  least significant byte: 0x12F & 0xFF = 0x2F
//  invert: 0x2F ^ 0xFF = 0xD0
//  CheckSume: 0xD0
//
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
uint8_t ProxyFlowSensor::CalcCRC(uint8_t* pBuf, uint32_t uSize)
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

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SendDataToUART
//
//    Processing: Because there is no hardware handshaking
//      the frame start and stop are signaled by a unique data content
//          + START = STOP = 0x7E
//      If this special start/stop byte (0x7E) occurs anywhere else in the frame,
//      it needs to be replaced. THe same is true for 3 more special byte:
//          Escape (0x7D), XON (0x11) and XOFF (0x13)
//      Original Byte               Transferred Bytes
//          0x7E                        0x7D, 0x5E
//          0x7D                        0x7D, 0x5D
//          0x11                        0x7D, 0x31
//          0x13                        0x7D, 0x33
//
//    Input Parameters: uByte - byte needed to convert
//
//    Output Parameters: None
//
//    Return Values: 2 byte convert
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None
//
/******************************************************************************/
int8_t* ProxyFlowSensor::ConvertByteSruffing (int8_t iByte)
{
    const uint8_t SIZE_OF_TRANSFER_BYTE = 2;
    const uint8_t INIT_BYTE = 0x00;

    static int8_t TempArray[SIZE_OF_TRANSFER_BYTE] = {0};

    switch (iByte) {
        //Replace  0x7E   =   0x7D, 0x5E
        case START_STOP_BYTE:
            TempArray[0] = ESCAPE_BYTE;
            TempArray[1] = START_STOP_ADD_BYTE;
            break;
            //Replace 0x7D  =  0x7D, 0x5D
        case ESCAPE_BYTE:
            TempArray[0] = ESCAPE_BYTE;
            TempArray[1] = ESCAPE_ADD_BYTE;
            break;
            //Replace 0x11  =  0x7D, 0x31
        case XON_BYTE:
            TempArray[0] = ESCAPE_BYTE;
            TempArray[1] = XON_ADD_BYTE;
            break;
            //Replace 0x13  =  0x7D, 0x33
        case XOFF_BYTE:
            TempArray[0] = ESCAPE_BYTE;
            TempArray[1] = XOFF_ADD_BYTE;
            break;
            //IF it is not a special byte
        default:
            TempArray[0] = iByte;
            TempArray[1] = INIT_BYTE;
            break;
    }

    return TempArray;
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GenerateFrameData
//
//    Processing: Generate frame data from Comment ID
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: Pointer to data and  FrameData must not be NULL
//
/******************************************************************************/
void ProxyFlowSensor::GenerateFrameData (ProxySensorSendFrameData* data,
                                         uint8_t* FrameData, uint32_t& SizeOfFrameData)
{
    const uint32_t THE_NUMBER_OF_START_BLOCK = 1;
    //check input and output data is valid
    ASSERTION(NULL != data);
    ASSERTION(NULL != FrameData);

    //initialize a temporary buffer to store Frame data
    std::vector<uint8_t> TempFrameData;

    //The byte 0x7E marks the beginning of the frame
    TempFrameData.push_back(START_BYTE);

    //Device Address of the slave
    TempFrameData.push_back(data->Address);

    //Command ID of the command
    TempFrameData.push_back(data->CMD);

    //Indicates the number of bytes sent in the Data block
    TempFrameData.push_back(data->LengTH);

    //The data format depends on the command
    for(uint32_t i = 0; i < data->LengTH; ++i)
    {
        TempFrameData.push_back(data->Data[i]);
    }

    //initialize a buffer to calculate Check sum
    //Size of buffer
    const uint32_t BufferSize = TempFrameData.size() - THE_NUMBER_OF_START_BLOCK;
    uint8_t TempBuff[BufferSize];
    for(uint32_t i = 0; i < BufferSize; ++i)
    {
        TempBuff[i] = TempFrameData[THE_NUMBER_OF_START_BLOCK + i];
    }
    //Check sum over the frame content
    TempFrameData.push_back(CalcCRC(TempBuff, BufferSize));

    //The second byte 0x7E marks the end of the frame
    TempFrameData.push_back(STOP_BYTE);

    //Update size of Frame Data to output parameter
    SizeOfFrameData = TempFrameData.size();

    //Copy Frame Data to output parameter
    for(uint32_t i = 0; i < SizeOfFrameData; ++i)
    {
        FrameData[i] = TempFrameData[i];
    }
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SendDataToUART
//
//    Processing:we generate Frame data to final byte array - sending
//      Because there is no hardware handshaking
//      the frame start and stop are signaled by a unique data content
//          + START = STOP = 0x7E
//      If this special start/stop byte (0x7E) occurs anywhere else in the frame or in CHK,
//      it needs to be replaced. THe same is true for 3 more special byte:
//          Escape (0x7D), XON (0x11) and XOFF (0x13)
//      Original Byte               Transferred Bytes
//          0x7E                        0x7D, 0x5E
//          0x7D                        0x7D, 0x5D
//          0x11                        0x7D, 0x31
//          0x13                        0x7D, 0x33
//
//
//    Input Parameters: pSourceByte - source byte
//                      uSourceSize - size of source byte
//
//    Output Parameters: pDesByte - destination byte
//                       uDesSizeByte - size of destination byte
//
//    Return Values: status of process
//              if it converts successfully, Return TRUE
//              if it fails to convert, Return FALSE
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: pointer to pSourceByte and pDesByte must not be NULL
//
/******************************************************************************/
void ProxyFlowSensor::ConvertByteArrayToSend (uint8_t* pSourceByte,
                                              uint32_t uSourceSize,
                                              uint8_t* pDesByte,
                                              uint32_t& uDesSizeByte)
{

    ASSERTION(NULL != pSourceByte);
    ASSERTION(NULL != pDesByte);

    const uint8_t NUMBER_OF_START_BYTE = 1;
    const uint8_t NUMBER_OF_STOP_BYTE = 1;

    std::vector<uint8_t> TempBuf;

    //Ignore START_BYTE(0x7E);
    TempBuf.push_back(START_BYTE);

    //Convert data if it has special bytes
    for(uint32_t i = (0 + NUMBER_OF_START_BYTE); i < (uSourceSize - NUMBER_OF_STOP_BYTE); ++i)
    {
        switch (pSourceByte[i])
        {
                //Replace  0x7E   =   0x7D, 0x5E
            case START_STOP_BYTE:
                TempBuf.push_back(ESCAPE_BYTE);
                TempBuf.push_back(START_STOP_ADD_BYTE);
                break;
                //Replace 0x7D  =  0x7D, 0x5D
            case ESCAPE_BYTE:
                TempBuf.push_back(ESCAPE_BYTE);
                TempBuf.push_back(ESCAPE_ADD_BYTE);
                break;
                //Replace 0x11  =  0x7D, 0x31
            case XON_BYTE:
                TempBuf.push_back(ESCAPE_BYTE);
                TempBuf.push_back(XON_ADD_BYTE);
                break;
                //Replace 0x13  =  0x7D, 0x33
            case XOFF_BYTE:
                TempBuf.push_back(ESCAPE_BYTE);
                TempBuf.push_back(XOFF_ADD_BYTE);
                break;
                //IF it is not a special byte
            default:
                TempBuf.push_back(pSourceByte[i]);
                break;
        }

    }

    //Ignore STOP_BYTE(0x7E)
    TempBuf.push_back(STOP_BYTE);

    //Update size of pDesByte
    uDesSizeByte = TempBuf.size();

    //Copy data to pDesByte
    for(uint32_t i = 0; i < uDesSizeByte; ++i)
    {
        pDesByte[i] = TempBuf[i];
    }

}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ConvertReceiveDataToFrameData
//
//    Processing: After receiving data from UART, removing special byte
//                      getting data from it
//
//
//    Input Parameters: uint8_t* pSourceByte - pointer to input data
//                      uint32_t uSourceSize - size of input data
//
//    Output Parameters: ProxySensorFrameData* data
//
//    Return Values: True - if data is right with Frame format
//                      and vice versa
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements: input and output data pointer must not be NULL
//
/******************************************************************************/
bool ProxyFlowSensor::ConvertReceiveDataToFrameData (uint8_t* pSourceByte,
                                                     uint32_t uSourceSize,
                                                     ProxySensorRecFrameData* data)
{
    //input and output data pointer must not be NULL
    ASSERTION(NULL != pSourceByte);
    ASSERTION(NULL != data);

    bool Status = true;

    //numerical order of each byte in HDLC frame
    const uint8_t THE_START_BYTE = 0;
    const uint8_t THE_STOP_BYTE = uSourceSize - 1;
    const uint8_t THE_ADD_BYTE = 1;
    const uint8_t THE_CMD_BYTE = 2;
    const uint8_t THE_STATE_BYTE = 3;
    const uint8_t THE_LEN_BYTE = 4;
    const uint8_t THE_DATA_BYTE = 5;

    //number of byte in each block
    const uint8_t NUMBER_OF_START_BYTE = 1;
    const uint8_t NUMBER_OF_STOP_BYTE = 1;
    const uint8_t NUMBER_OF_CHK_BYTE = 1;

    const uint8_t NO_ERROR = 0x00;

    //Check start_byte and stop_byte in frame data
    if((START_BYTE != pSourceByte[THE_START_BYTE])
            &&(STOP_BYTE != pSourceByte[THE_STOP_BYTE]))
    {
        //START BYTE and STOP BYTE is not right
        Status = false;
    }
    else
    {
        std::vector <uint8_t> TempBuf;

        //add start byte
        TempBuf.push_back(START_BYTE);

        //Remove special byte
        for(uint32_t i = (0 + NUMBER_OF_START_BYTE); i < (uSourceSize - NUMBER_OF_STOP_BYTE); ++i)
        {
            if(ESCAPE_BYTE == pSourceByte[i])
            {
               switch (pSourceByte[i+1]) {
                  case START_STOP_ADD_BYTE:
                     TempBuf.push_back(START_STOP_BYTE);
                     ++i;
                     break;
                  case ESCAPE_ADD_BYTE:
                     TempBuf.push_back(ESCAPE_BYTE);
                     ++i;
                     break;
                  case XON_ADD_BYTE:
                     TempBuf.push_back(XON_BYTE);
                     ++i;
                     break;
                  case XOFF_ADD_BYTE:
                     TempBuf.push_back(XOFF_BYTE);
                     ++i;
                     break;
                  default:
                      TempBuf.push_back(pSourceByte[i]);
                     break;
               }

            }
            else
            {
               TempBuf.push_back(pSourceByte[i]);
            }
        }

        //add stop byte
        TempBuf.push_back(STOP_BYTE);


        //check the CheckSume of data
        const uint32_t sizeOfTempArrayToCalcCHK = TempBuf.size() - NUMBER_OF_START_BYTE - NUMBER_OF_STOP_BYTE - NUMBER_OF_CHK_BYTE;
        uint8_t tempArrayToCalcCHK[sizeOfTempArrayToCalcCHK]= {0};

        //Copy data to temporary array for checking checksum
        for(uint32_t i = 0; i < sizeOfTempArrayToCalcCHK; ++i)
        {
            tempArrayToCalcCHK[i] = TempBuf[i + NUMBER_OF_START_BYTE];
        }
        uint8_t calcCHK = CalcCRC(tempArrayToCalcCHK, sizeOfTempArrayToCalcCHK);

        //Wrong CHK
        if(calcCHK != TempBuf[TempBuf.size() - NUMBER_OF_STOP_BYTE - 1])
        {
            Status = false;
        }
        else
        {
            //Copy data to output pointer
            data->Address = TempBuf[THE_ADD_BYTE];
            data->CMD = TempBuf[THE_CMD_BYTE];
            data->State = TempBuf[THE_STATE_BYTE];
            data->LengTH = TempBuf[THE_LEN_BYTE];

            for(uint32_t i = 0; i < data->LengTH; ++i)
            {
                data->Data[i] = TempBuf[THE_DATA_BYTE + i];
            }

            //Check the State byte
            //The slave sends a state byte to report execution errors or communication errors to the master
            //The value NO_ERROR(0x00) corresponds to 'no error'.
            if(NO_ERROR == data->State)
            {
                Status = true;
            }
            else
            {
                Status = false;
            }
        }
    }
    return Status;
}
