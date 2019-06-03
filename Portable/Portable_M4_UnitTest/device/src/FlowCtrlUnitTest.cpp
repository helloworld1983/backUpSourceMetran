/********************************************************************/
/*                                                                  */
/* File Name     : FlowCtrUnitTest.cpp                              */
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

#define MAX_RESEND_CNT                  5


typedef enum
{
    FC_SEND_FLOW,
    FC_SEND_VERSION,
    FC_SEND_ACQ1,
    FC_SEND_ACQ2,
    FC_SEND_MODE,
    FC_SEND_OCSET,
    FC_SEND_CLEAR_ERR,
    FC_DONT_CARE
}E_FlowCtrlRequestCmd;

typedef enum
{
    FC_NORMAL_SEQ,
    FC_RESET_SEQ,
    FC_REBOOT_SEQ
}E_FlowCtrlSequence;

typedef enum
{
    eFlowControllerNoErr,
    eChecksumErr,
    eCommandErr,
    eParamErr,
    eNoResponseErr,
    eResponseLengthErr,
    eResponseCodeErr,
    eResponseCheckSumErr,
    eResponseParamErr,
    eFlowReset,
    eFlowControllerNoAlarm
}E_FlowControllerNoAlarmStatus;

unsigned char flowCtrl_CreateCRC(unsigned char data[], char nbrOfBytes)
{
    unsigned char sum = 0;
    for(int i = 0; i < nbrOfBytes; i ++)
    {
        sum ^= data[i];
    }
    return sum;
}

class FlowCtrUnitTest : public ::testing::Test
{
};
TEST_F(FlowCtrUnitTest, flowCtrl_CreateCRC)
{
    unsigned char Data1[255];
    unsigned char Data2[255];
    for(int i = 0; i < 255; ++i)
    {
        Data1[i] = i;
        Data2[i] = 0xff;
    }

    EXPECT_EQ(0, flowCtrl_CreateCRC(Data1, 1));
    EXPECT_EQ(1, flowCtrl_CreateCRC(Data1, 2));
    EXPECT_EQ(1, flowCtrl_CreateCRC(Data1, 10));
    EXPECT_EQ(0, flowCtrl_CreateCRC(Data1, 255));
    EXPECT_EQ(0, flowCtrl_CreateCRC(Data2, 255));
}

void flowCtrl_ProcessSendCmdRequest( E_FlowCtrlRequestCmd RequestSendCmd, E_FlowCtrlRequestCmd &s_RequestSendCmd,
                                     E_FlowCtrlRequestCmd &s_preRequestSendCmd)
{
    s_RequestSendCmd = FC_SEND_VERSION;
    s_preRequestSendCmd = FC_SEND_VERSION;

    switch (RequestSendCmd) {
        case FC_SEND_VERSION:
            s_RequestSendCmd = FC_SEND_CLEAR_ERR;
            s_preRequestSendCmd = FC_SEND_VERSION;
            break;
        case FC_SEND_CLEAR_ERR:
            s_RequestSendCmd = FC_SEND_FLOW;
            s_preRequestSendCmd = FC_SEND_CLEAR_ERR;
            break;
        case FC_SEND_FLOW:
            s_RequestSendCmd = FC_SEND_ACQ1;
            s_preRequestSendCmd = FC_SEND_FLOW;
            break;
        case FC_SEND_ACQ1:
            s_RequestSendCmd = FC_SEND_ACQ2;
            s_preRequestSendCmd = FC_SEND_ACQ1;
            break;
        case FC_SEND_ACQ2:
            s_RequestSendCmd = FC_SEND_ACQ1;
            s_preRequestSendCmd = FC_SEND_ACQ2;
            break;
        case FC_SEND_MODE:
            s_RequestSendCmd = FC_SEND_ACQ1;
            s_preRequestSendCmd = FC_SEND_MODE;
        default:
            break;
    }
};
struct ProcessSendCmdRequest
{
        E_FlowCtrlRequestCmd RequestSendCmd;
        E_FlowCtrlRequestCmd s_RequestSendCmd;
        E_FlowCtrlRequestCmd s_preRequestSendCmd;
};
struct ProcessSendCmdRequest ProcessSendCmdRequestTestCase[] =
{
        {FC_SEND_VERSION, FC_SEND_CLEAR_ERR, FC_SEND_VERSION},
        {FC_SEND_CLEAR_ERR, FC_SEND_FLOW, FC_SEND_CLEAR_ERR},
        {FC_SEND_FLOW, FC_SEND_ACQ1, FC_SEND_FLOW},
        {FC_SEND_ACQ1, FC_SEND_ACQ2, FC_SEND_ACQ1},
        {FC_SEND_ACQ2, FC_SEND_ACQ1, FC_SEND_ACQ2},
        {FC_SEND_MODE, FC_SEND_ACQ1, FC_SEND_MODE},
        {FC_DONT_CARE, FC_SEND_VERSION, FC_SEND_VERSION}
};
class TestProcessSendCmdRequest : public::testing::TestWithParam<ProcessSendCmdRequest>
{
};
TEST_P(TestProcessSendCmdRequest, TestProcessSendCmdRequestList)
{
    E_FlowCtrlRequestCmd RequestSendCmd = GetParam().RequestSendCmd;
    E_FlowCtrlRequestCmd In_RequestSendCmd;
    E_FlowCtrlRequestCmd s_RequestSendCmd = GetParam().s_RequestSendCmd;
    E_FlowCtrlRequestCmd In_preRequestSendCmd;
    E_FlowCtrlRequestCmd s_preRequestSendCmd = GetParam().s_preRequestSendCmd;

    flowCtrl_ProcessSendCmdRequest(RequestSendCmd, In_RequestSendCmd, In_preRequestSendCmd);
    EXPECT_EQ(s_RequestSendCmd, In_RequestSendCmd);
    EXPECT_EQ(s_preRequestSendCmd, In_preRequestSendCmd);
}
INSTANTIATE_TEST_CASE_P(TestProcessSendCmdRequest,
                        TestProcessSendCmdRequest,
                        testing::ValuesIn(ProcessSendCmdRequestTestCase)
);

E_FlowControllerNoAlarmStatus flowCtrl_HandleResendCmd(uint8_t gs_fcReSendCnt, E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus)
{
    s_fcErrNoAlarmStatus = eFlowControllerNoErr;
    if(gs_fcReSendCnt < MAX_RESEND_CNT)
    {

        if((s_fcErrNoAlarmStatus != eResponseCodeErr)
                && (s_fcErrNoAlarmStatus != eResponseCheckSumErr)
                && (s_fcErrNoAlarmStatus != eResponseLengthErr)
                && (s_fcErrNoAlarmStatus != eResponseParamErr))
        {
            s_fcErrNoAlarmStatus = eNoResponseErr;

        }
    }
    else
    {
        s_fcErrNoAlarmStatus = eFlowControllerNoAlarm;
    }

    return s_fcErrNoAlarmStatus;
}

struct HandleResendCmd
{
        uint8_t gs_fcReSendCnt;
        E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus;
        E_FlowControllerNoAlarmStatus result;
};
struct HandleResendCmd HandleResendCmdTestCase[] =
{
        {MAX_RESEND_CNT - 1, eFlowControllerNoErr, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eChecksumErr, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eCommandErr, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eParamErr, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eNoResponseErr, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eFlowReset, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eFlowControllerNoAlarm, eNoResponseErr},

        {MAX_RESEND_CNT - 1, eResponseLengthErr, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eResponseCodeErr, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eResponseCheckSumErr, eNoResponseErr},
        {MAX_RESEND_CNT - 1, eResponseParamErr, eNoResponseErr},

        {MAX_RESEND_CNT + 2, eFlowControllerNoErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eChecksumErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eCommandErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eParamErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eNoResponseErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eResponseLengthErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eResponseCodeErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eResponseCheckSumErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eResponseParamErr, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eFlowReset, eFlowControllerNoAlarm},
        {MAX_RESEND_CNT + 2, eFlowControllerNoAlarm, eFlowControllerNoAlarm}
};
class HandleResendCmdTest : public ::testing::TestWithParam<HandleResendCmd>
{
};
TEST_P(HandleResendCmdTest, HandleResendCmdTestCase)
{
    uint8_t gs_fcReSendCnt = GetParam().gs_fcReSendCnt;
    E_FlowControllerNoAlarmStatus s_fcErrNoAlarmStatus = GetParam().s_fcErrNoAlarmStatus;
    E_FlowControllerNoAlarmStatus result = GetParam().result;

    EXPECT_EQ(result, flowCtrl_HandleResendCmd(gs_fcReSendCnt, s_fcErrNoAlarmStatus));
}
INSTANTIATE_TEST_CASE_P(HandleResendCmdTest,
                        HandleResendCmdTest,
                        testing::ValuesIn(HandleResendCmdTestCase)
);

size_t flowCtrl_SendFlowRateCmd(size_t idx)
{
    if (idx != -1)
    {
        return idx;
    }
    else
    {
        return idx;
    }
}
TEST_F(FlowCtrUnitTest, flowCtrl_SendFlowRateCmd)
{
    EXPECT_EQ(1, flowCtrl_SendFlowRateCmd(1));
    EXPECT_EQ(11, flowCtrl_SendFlowRateCmd(11));
    EXPECT_EQ(-1, flowCtrl_SendFlowRateCmd(-1));
}

unsigned char flowCtrl_ReadFromDevice(unsigned char Data)
{

    switch (Data){
        case 'A':
            return 'A';
            break;
        case 'B':
            return 'B';
            break;
        case 'C':
            return 'C';
            break;
        case 'D':
            return 'D';
            break;
        case 'E':
            return 'E';
            break;
        case 'F':
            return 'F';
            break;
        case 'G':
            return 'G';
            break;
        case 'H':
            return 'H';
            break;
        case 'W':
            return 'W';
            break;
        default:
            return 'K';
            break;
    }
}
unsigned char Data[] =
{
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'W', 'K'
};
class TestReadFromDevice : public ::testing::TestWithParam<unsigned char>
{
};
TEST_P(TestReadFromDevice, TestReadFromDevice)
{
    EXPECT_EQ(GetParam(), flowCtrl_ReadFromDevice(GetParam()));
}
INSTANTIATE_TEST_CASE_P(TestReadFromDevice,
                        TestReadFromDevice,
                        testing::ValuesIn(Data)
);

E_FlowCtrlSequence flowCtrl_Handle(E_FlowCtrlSequence gs_fcSeq)
{
    switch (gs_fcSeq)
    {
        case FC_NORMAL_SEQ:
            return FC_NORMAL_SEQ;
            break;
        case FC_REBOOT_SEQ:
            return FC_NORMAL_SEQ;
            break;
        case FC_RESET_SEQ:
            return FC_NORMAL_SEQ;
            break;
        default:
            break;
    }
}

E_FlowCtrlSequence TestCase[] =
{
        FC_NORMAL_SEQ, FC_NORMAL_SEQ, FC_NORMAL_SEQ
};
class flowCtrl_HandleTest : public ::testing::TestWithParam<E_FlowCtrlSequence>
{
};
TEST_P(flowCtrl_HandleTest, flowCtrl_HandleTestCaseList)
{
    EXPECT_EQ(GetParam(), flowCtrl_Handle(GetParam()));
}
INSTANTIATE_TEST_CASE_P(flowCtrl_HandleTestCaseList,
                        flowCtrl_HandleTest,
                        testing::ValuesIn(TestCase)
);

bool flowCtrl_GetResetStatus(E_FlowCtrlSequence gs_fcSeq)
{
    if (gs_fcSeq == FC_NORMAL_SEQ)
    {
        return false;
    }
    else
    {
        return true;
    }
}

TEST_F(FlowCtrUnitTest, flowCtrl_GetResetStatus)
{
    EXPECT_EQ(false, flowCtrl_GetResetStatus(FC_NORMAL_SEQ));
    EXPECT_EQ(true, flowCtrl_GetResetStatus(FC_RESET_SEQ));
    EXPECT_EQ(true, flowCtrl_GetResetStatus(FC_REBOOT_SEQ));
}
