#include "gtest.h"

//define for UART3 - RS485
#define FS_UART_CHANEL              LPC_USART3
#define UART_SRB_SIZE               (128)             //Send
#define UART_RRB_SIZE               (32)              //Receive
#define BAUD_RATE                   (115200)          //Baud rate

#define EN_SEND_DATA_PORT           (0xF)             //PORT to control send data
#define EN_SEND_DATA_PIN            (6)               //PIN to control send data
#define EN_GPIO_SEND_DATA_PORT_NUM  (7)               //GPIO PORT to control send data
#define EN_GPIO_SEND_DATA_PIN_NUM   (20)              //GPIO PIN to control send data

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

class ProxyFlowSensorUnitTest : public ::testing::Test
{
};

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

void ConvertByteArrayToSend (uint8_t* pSourceByte,
                             uint32_t uSourceSize,
                             uint8_t* pDesByte,
                             uint64_t& uDesSizeByte)
{

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

TEST_F(ProxyFlowSensorUnitTest, ConvertByteArrayToSend)
{
    //Test with normal case
    //Test with out of range
    uint8_t TestData[8] = {0x7E, 0x7E, 0x7D, 0x11, 0x00, 0x13, 0xBF, 0x7E};
    uint8_t ExpectData[12] = {0x7e, 0x7d, 0x5e, 0x7d, 0x5d, 0x7d, 0x31, 0x00, 0x7d, 0x33, 0xbf, 0x7e};
    uint8_t OutPutTest[] = {};
    uint64_t Size;
    ConvertByteArrayToSend(TestData, 8, OutPutTest, Size);
    for(uint64_t i = 0; i < Size; ++i)
    {
        EXPECT_EQ(ExpectData[i], OutPutTest[i]);
    }
}

uint8_t CalcCRC(uint8_t* pBuf, uint32_t uSize)
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

TEST_F(ProxyFlowSensorUnitTest, CalcCRC)
{

}

void GenerateFrameData (ProxySensorSendFrameData* data,
                                         uint8_t* FrameData, uint32_t& SizeOfFrameData)
{
    const uint32_t THE_NUMBER_OF_START_BLOCK = 1;
    //check input and output data is valid

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

TEST_F(ProxyFlowSensorUnitTest, GenerateFrameData)
{
    ProxySensorSendFrameData dt;
        dt.Address = 0x7e;
        dt.CMD = 0x7d;
        dt.LengTH = 0x03;
        dt.Data[0] = 0x11;
        dt.Data[1] = 0x13;
        dt.Data[2] = 0xAA;
        uint8_t test[6 + dt.LengTH] = {0};
        uint8_t ExpectData[6 + dt.LengTH] = {0x7E, 0x7E, 0x7D, 0x3, 0x11, 0x13, 0xAA, 0x33, 0x7E};
        uint32_t sizeT;

        GenerateFrameData(&dt,test,sizeT);

        for(uint32_t i = 0; i < sizeT; ++i)
        {
            EXPECT_EQ(ExpectData[i], test[i]);
        }
}

bool ConvertReceiveDataToFrameData (uint8_t* pSourceByte,
                                    uint32_t uSourceSize,
                                    ProxySensorRecFrameData* data)
{
    //input and output data pointer must not be NULL

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

TEST_F(ProxyFlowSensorUnitTest, ConvertReceiveDataToFrameData)
{
    uint8_t Test[14] = {0x7e, 0x7d, 0x5e, 0x7d, 0x5d, 0x00, 0x3, 0x7d, 0x31, 0x7d, 0x33, 0xaa, 0x33, 0x7e};
    ProxySensorRecFrameData Recdata;

    EXPECT_TRUE(ConvertReceiveDataToFrameData(Test, 14, &Recdata));
    EXPECT_EQ(Recdata.Address, 0x7E);
    EXPECT_EQ(Recdata.CMD, 0x7D);
    EXPECT_EQ(Recdata.LengTH, 0x3);
    EXPECT_EQ(Recdata.State, 0);

    uint8_t ExpectData[3] = { 0x11, 0x13, 0xaa};

    for(uint32_t i = 0; i < Recdata.LengTH; ++i)
    {
        EXPECT_EQ(ExpectData[i], Recdata.Data[i]);
    }

}
