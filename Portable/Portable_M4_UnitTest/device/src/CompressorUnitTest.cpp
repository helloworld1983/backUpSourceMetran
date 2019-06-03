/********************************************************************/
/*                                                                  */
/* File Name     : CompressorUnitTest.cpp                           */
/*                                                                  */
/* General       : This module contain function for Unit Test       */
/*                                                                  */
/*                                                                  */
/* Product Name  : Portable                                         */
/*                                                                  */
/* Company       : Metran                                           */
/*                                                                  */
/* history                                                          */
/*==================================================================*/
/* [Number]  [Date]            [Editor]            [Explanation]    */
/* -----------+---------------+-------------------+-----------------*/
/*                                                                  */
/*                                                                  */
/********************************************************************/
#include "gtest.h"
using ::testing::TestWithParam;

typedef enum
{
    COMP_SEND_RPM,
    COMP_SEND_VERSION,
    COMP_SEND_TEMPERATURE,
    COMP_SEND_ACQ,
    COMP_SEND_CLR_ERR
}E_CompressorRequestCmd;

typedef enum
{
    eCompressorNoErr = 0,
    eCommunicationError,
    eDCOverVoltageError,
    eDCLowVoltageError,
    eSoftwareError,
    eHardwareError,
    eBoardTemperatureError,
    eMotorTemperatureError,
    eRPMError,
    eMotorStepOutError,
    eCompChecksumErr,
    eCompCommandErr,
    eCompParamErr,
    eCompNoResponseErr,
    eCompResponseLengthErr,
    eCompResponseCodeErr,
    eCompResponseChecksumErr,
    eCompResponseParamErr,
    eCompressorReset,
}E_CompressorStatus;

typedef enum
{
    COMP_NORMAL_SEQ,
    COMP_RESET_SEQ,
    COMP_RESET_FOR_RESTART_SEQ,
    COMP_REBOOT_SEQ,
    COMP_REBOOT_FOR_RESTART_SEQ,
    COMP_RESTART_SEQ
}E_CompressorSequence;
class CompressorUnitTest : public ::testing::Test
{
};

unsigned char comppressor_CreateCRC(unsigned char data[], char nbrOfBytes)
{
    unsigned char sum = 0;
    for(int i = 0; i < nbrOfBytes; i ++)
    {
        sum ^= data[i];
    }
    return sum;
}

TEST_F(CompressorUnitTest, comppressor_CreateCRC)
{
    unsigned char Data1[255];
    unsigned char Data2[255];
    for(int i = 0; i < 255; ++i)
    {
        Data1[i] = i;
        Data2[i] = 0xff;
    }

    EXPECT_EQ(0, comppressor_CreateCRC(Data1, 1));
    EXPECT_EQ(1, comppressor_CreateCRC(Data1, 2));
    EXPECT_EQ(1, comppressor_CreateCRC(Data1, 10));
    EXPECT_EQ(0, comppressor_CreateCRC(Data1, 255));
    EXPECT_EQ(0, comppressor_CreateCRC(Data2, 255));

}

E_CompressorRequestCmd compressor_RequestSendClearErrStatusCmd()
{
    E_CompressorRequestCmd s_compRequestSendCmd = COMP_SEND_CLR_ERR;
    return s_compRequestSendCmd;
}

TEST_F(CompressorUnitTest, compressor_RequestSendClearErrStatusCmd)
{
    EXPECT_EQ(COMP_SEND_CLR_ERR, compressor_RequestSendClearErrStatusCmd());
}

E_CompressorStatus compressor_HandleResetNoti()
{
    E_CompressorStatus s_compErrNoAlarmStatus = eCompressorReset;
    return s_compErrNoAlarmStatus;
}

TEST_F(CompressorUnitTest, compressor_HandleResetNoti)
{
    EXPECT_EQ(eCompressorReset, compressor_HandleResetNoti());
}

void compressor_ProcessSendCmdRequest(E_CompressorRequestCmd s_compRequestSendCmdInPut,
                                      E_CompressorRequestCmd &s_compRequestSendCmd, E_CompressorRequestCmd &s_preCompRequestSendCmd)
{

    switch (s_compRequestSendCmdInPut) {
        case COMP_SEND_VERSION:
            s_compRequestSendCmd = COMP_SEND_RPM;   //COMP_SEND_CLR_ERR
            s_preCompRequestSendCmd = COMP_SEND_VERSION;
            break;
        case COMP_SEND_CLR_ERR:
            s_compRequestSendCmd = COMP_SEND_VERSION; //COMP_SEND_RPM
            s_preCompRequestSendCmd = COMP_SEND_CLR_ERR;
            break;
        case COMP_SEND_RPM:
            s_compRequestSendCmd = COMP_SEND_ACQ;
            s_preCompRequestSendCmd = COMP_SEND_RPM;
            break;
        case COMP_SEND_ACQ:
            s_compRequestSendCmd = COMP_SEND_TEMPERATURE;
            s_preCompRequestSendCmd = COMP_SEND_ACQ;
            break;
        case COMP_SEND_TEMPERATURE:
            s_compRequestSendCmd = COMP_SEND_ACQ;
            s_preCompRequestSendCmd = COMP_SEND_ACQ;
        default:
            break;
    }

}
TEST_F(CompressorUnitTest, compressor_ProcessSendCmdRequest_COMP_SEND_VERSION)
{
    E_CompressorRequestCmd s_compRequestSendCmdOutPut;
    E_CompressorRequestCmd s_preCompRequestSendCmd;
    compressor_ProcessSendCmdRequest(COMP_SEND_VERSION, s_compRequestSendCmdOutPut, s_preCompRequestSendCmd);
    EXPECT_EQ(COMP_SEND_RPM, s_compRequestSendCmdOutPut);
    EXPECT_EQ(COMP_SEND_VERSION, s_preCompRequestSendCmd);
}
TEST_F(CompressorUnitTest, compressor_ProcessSendCmdRequest_COMP_SEND_CLR_ERR)
{
    E_CompressorRequestCmd s_compRequestSendCmdOutPut;
    E_CompressorRequestCmd s_preCompRequestSendCmd;
    compressor_ProcessSendCmdRequest(COMP_SEND_CLR_ERR, s_compRequestSendCmdOutPut, s_preCompRequestSendCmd);
    EXPECT_EQ(COMP_SEND_VERSION, s_compRequestSendCmdOutPut);
    EXPECT_EQ(COMP_SEND_CLR_ERR, s_preCompRequestSendCmd);
}
TEST_F(CompressorUnitTest, compressor_ProcessSendCmdRequest_COMP_SEND_RPM)
{
    E_CompressorRequestCmd s_compRequestSendCmdOutPut;
    E_CompressorRequestCmd s_preCompRequestSendCmd;
    compressor_ProcessSendCmdRequest(COMP_SEND_RPM, s_compRequestSendCmdOutPut, s_preCompRequestSendCmd);
    EXPECT_EQ(COMP_SEND_ACQ, s_compRequestSendCmdOutPut);
    EXPECT_EQ(COMP_SEND_RPM, s_preCompRequestSendCmd);
}
TEST_F(CompressorUnitTest, compressor_ProcessSendCmdRequest_COMP_SEND_ACQ)
{
    E_CompressorRequestCmd s_compRequestSendCmdOutPut;
    E_CompressorRequestCmd s_preCompRequestSendCmd;
    compressor_ProcessSendCmdRequest(COMP_SEND_ACQ, s_compRequestSendCmdOutPut, s_preCompRequestSendCmd);
    EXPECT_EQ(COMP_SEND_TEMPERATURE, s_compRequestSendCmdOutPut);
    EXPECT_EQ(COMP_SEND_ACQ, s_preCompRequestSendCmd);
}
TEST_F(CompressorUnitTest, compressor_ProcessSendCmdRequest_COMP_SEND_TEMPERATURE)
{
    E_CompressorRequestCmd s_compRequestSendCmdOutPut;
    E_CompressorRequestCmd s_preCompRequestSendCmd;
    compressor_ProcessSendCmdRequest(COMP_SEND_TEMPERATURE, s_compRequestSendCmdOutPut, s_preCompRequestSendCmd);
    EXPECT_EQ(COMP_SEND_ACQ, s_compRequestSendCmdOutPut);
    EXPECT_EQ(COMP_SEND_ACQ, s_preCompRequestSendCmd);
}

E_CompressorSequence compressor_Handle(E_CompressorSequence s_compressorSeq)
{
    switch (s_compressorSeq) {
        case COMP_NORMAL_SEQ:
            return COMP_NORMAL_SEQ;

            break;
        case COMP_REBOOT_SEQ:
            return COMP_REBOOT_SEQ;

            break;
        case COMP_RESET_SEQ:
            return COMP_RESET_SEQ;

            break;
        case COMP_REBOOT_FOR_RESTART_SEQ:
            return COMP_REBOOT_FOR_RESTART_SEQ;

            break;
        case COMP_RESET_FOR_RESTART_SEQ:
            return COMP_RESET_FOR_RESTART_SEQ;

            break;
        case COMP_RESTART_SEQ:
            return COMP_RESTART_SEQ;

            break;
        default:
            break;
    }
}
TEST_F(CompressorUnitTest, compressor_Handle_COMP_NORMAL_SEQ)
{
    EXPECT_EQ(COMP_NORMAL_SEQ, compressor_Handle(COMP_NORMAL_SEQ));
}
TEST_F(CompressorUnitTest, compressor_Handle_COMP_REBOOT_SEQ)
{
    EXPECT_EQ(COMP_REBOOT_SEQ, compressor_Handle(COMP_REBOOT_SEQ));
}
TEST_F(CompressorUnitTest, compressor_Handle_COMP_RESET_SEQ)
{
    EXPECT_EQ(COMP_RESET_SEQ, compressor_Handle(COMP_RESET_SEQ));
}
TEST_F(CompressorUnitTest, compressor_Handle_COMP_REBOOT_FOR_RESTART_SEQ)
{
    EXPECT_EQ(COMP_REBOOT_FOR_RESTART_SEQ, compressor_Handle(COMP_REBOOT_FOR_RESTART_SEQ));
}
TEST_F(CompressorUnitTest, compressor_Handle_COMP_RESET_FOR_RESTART_SEQ)
{
    EXPECT_EQ(COMP_RESET_FOR_RESTART_SEQ, compressor_Handle(COMP_RESET_FOR_RESTART_SEQ));
}
TEST_F(CompressorUnitTest, compressor_Handle_COMP_RESTART_SEQ)
{
    EXPECT_EQ(COMP_RESTART_SEQ, compressor_Handle(COMP_RESTART_SEQ));
}
