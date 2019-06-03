/********************************************************************/
/*                                                                  */
/* File Name     : PanelKeyUnitTest.cpp                             */
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

#define DEVICE_TASK_DELAY_TIME      5
#define KEY_PANEL_DELAY_TIME 40
#define KEY_PANEL_DELAY_LOOP_CNT KEY_PANEL_DELAY_TIME/DEVICE_TASK_DELAY_TIME
#define ZERO_SECOND_LOOP_CNT 0/KEY_PANEL_DELAY_TIME
#define ONE_SECOND_LOOP_CNT 1000/KEY_PANEL_DELAY_TIME
#define TWO_SECOND_LOOP_CNT 2000/KEY_PANEL_DELAY_TIME
#define THREE_SECOND_LOOP_CNT 3000/KEY_PANEL_DELAY_TIME
#define FOUR_SECOND_LOOP_CNT 4000/KEY_PANEL_DELAY_TIME
#define FIVE_SECOND_LOOP_CNT 5000/KEY_PANEL_DELAY_TIME
#define SIX_SECOND_LOOP_CNT 6000/KEY_PANEL_DELAY_TIME
#define SEVEN_SECOND_LOOP_CNT 7000/KEY_PANEL_DELAY_TIME
#define POWER_OFF_FC_OPEN_TIME 1000
#define SW1_PRESSED_VOLTAGE 0.5
#define SW2_PRESSED_VOLTAGE 0.1
#define SW3_PRESSED_VOLTAGE 0.80
#define SW4_PRESSED_VOLTAGE 1.7
#define SW5_PRESSED_VOLTAGE 2.6
#define RESET_SW_STATE_LOOP_CNT 100/KEY_PANEL_DELAY_TIME

#define NONE_PRESSED 0
#define SW1_PRESSED 1
#define SW2_PRESSED 2
#define SW3_PRESSED 4
#define SW4_PRESSED 8
#define SW5_PRESSED 16
#define KEY_CNT_WAIT_RELEASE -1
#define KEY_CNT_2S 2000/KEY_PANEL_DELAY_TIME
#define KEY_CNT_05S 500/KEY_PANEL_DELAY_TIME
#define KEY_4S_ID 14
#define KEY_4S_TICK 4000
#define KEY_1S_ID 15
#define KEY_1S_TICK 1000
#define KEY_3100MS_ID 16
#define KEY_3100MS_TICK 3100//2200
#define NUMBER_OF_MUTE_BUTTON_TOGGLE_UPDATE 3
#define NUMBER_OF_MODE_CHANGE_BUTTON_TOGGLE_UPDATE 2
#define NUMBER_OF_INCREASE_BUTTON_TOGGLE_UPDATE 1
#define NUMBER_OF_DECREASE_BUTTON_TOGGLE_UPDATE 1
#define NUMBER_OF_MUTE_BUTTON_PUSH2S_UPDATE 1

typedef enum
{
  TOGGLE,
  TOGGLESHORT,
  TOGGLE2S,
  IDLE,
} KEY_EVENT;

uint32_t keyPanel_TimerCallback( uint32_t timerId )
{
    switch (timerId)
    {
        case KEY_4S_ID:
            return KEY_4S_ID;
            break;
        case KEY_1S_ID:
            return KEY_1S_ID;
            break;
        case KEY_3100MS_ID:
            return KEY_3100MS_ID;
            break;
        default:
            break;
    }
}

class TimerCallback : public ::testing::TestWithParam<uint32_t>
{
};

uint32_t TimerCallbackTestCase[] =
{
        KEY_4S_ID, KEY_1S_ID, KEY_3100MS_ID

};
TEST_P(TimerCallback, TimerCallbackTestCase)
{
    EXPECT_EQ(GetParam(), keyPanel_TimerCallback(GetParam()));
}
INSTANTIATE_TEST_CASE_P(TimerCallback,
                        TimerCallback,
                        testing::ValuesIn(TimerCallbackTestCase)
);

uint16_t keyPanel_PowerOffSystem(uint16_t s_cnt)
{
    switch(s_cnt)
    {
        case ZERO_SECOND_LOOP_CNT:
            return ZERO_SECOND_LOOP_CNT;
            break;
        case ONE_SECOND_LOOP_CNT:
            return ONE_SECOND_LOOP_CNT;

            break;
        case TWO_SECOND_LOOP_CNT:
            return TWO_SECOND_LOOP_CNT;

            break;
        case THREE_SECOND_LOOP_CNT:
            return THREE_SECOND_LOOP_CNT;
            break;
        case FOUR_SECOND_LOOP_CNT:
            return FOUR_SECOND_LOOP_CNT;
            break;
        default:
            break;
    }
}

class PowerOffSystem : public ::testing::TestWithParam<uint16_t>
{
};

uint16_t PowerOffSystemTestCase[] =
{
        ZERO_SECOND_LOOP_CNT, ONE_SECOND_LOOP_CNT, TWO_SECOND_LOOP_CNT,
        THREE_SECOND_LOOP_CNT, FOUR_SECOND_LOOP_CNT
};
TEST_P(PowerOffSystem, PowerOffSystemTestCase)
{
    EXPECT_EQ(GetParam(), keyPanel_PowerOffSystem(GetParam()));
}
INSTANTIATE_TEST_CASE_P(PowerOffSystemTestCase,
                        PowerOffSystem,
                        testing::ValuesIn(PowerOffSystemTestCase)
);

uint8_t keyPanel_ReadValue(float voltVal_Key1, float voltVal_Key0)
{
    uint8_t Keypressed = NONE_PRESSED;
    if(voltVal_Key1 <= SW2_PRESSED_VOLTAGE)
    {
        Keypressed = SW2_PRESSED;
    }
    else if(voltVal_Key1 <= SW3_PRESSED_VOLTAGE)
    {
        Keypressed = SW3_PRESSED;
    }
    else if(voltVal_Key1 <= SW4_PRESSED_VOLTAGE)
    {
        Keypressed = SW4_PRESSED;
    }
    else if(voltVal_Key1 <= SW5_PRESSED_VOLTAGE)
    {
        Keypressed = SW5_PRESSED;
    }

    if(voltVal_Key0 <= SW1_PRESSED_VOLTAGE)
    {
        Keypressed = SW1_PRESSED;
    }
    return Keypressed;
}
struct ReadValue
{
        float voltVal_Key1;
        float voltVal_Key0;
        uint8_t Keypressed;
};
struct ReadValue ReadValueTestCase [] =
{
        {0, SW1_PRESSED_VOLTAGE + 1, SW2_PRESSED},

        {SW2_PRESSED_VOLTAGE, SW1_PRESSED_VOLTAGE + 1, SW3_PRESSED},
        {SW2_PRESSED_VOLTAGE, SW1_PRESSED_VOLTAGE + 1, SW3_PRESSED},

        {0.5, SW1_PRESSED_VOLTAGE + 1, NONE_PRESSED|SW3_PRESSED},

        {1, SW1_PRESSED_VOLTAGE + 1, NONE_PRESSED|SW4_PRESSED},

        {SW5_PRESSED_VOLTAGE, 0, SW1_PRESSED},
        {SW5_PRESSED_VOLTAGE, SW1_PRESSED_VOLTAGE, SW1_PRESSED}
};

class ReadValueTest : public ::testing::TestWithParam<ReadValue>
{
};
TEST_P(ReadValueTest, ReadValueTestCase)
{
    float voltVal_Key1 = GetParam().voltVal_Key1;
    float voltVal_Key0 = GetParam().voltVal_Key0;
    uint8_t Keypressed = GetParam().Keypressed;

    EXPECT_EQ(Keypressed, keyPanel_ReadValue(voltVal_Key1, voltVal_Key0));
}

INSTANTIATE_TEST_CASE_P(ReadValueTest,
                        ReadValueTest,
                        testing::ValuesIn(ReadValueTestCase)
);

void keyPanel_ProcessEventSW1(KEY_EVENT gs_SW1state, KEY_EVENT &gs_prevSW1state, KEY_EVENT &SW1state)
{
    switch(gs_SW1state)
    {
        case TOGGLE:
            gs_prevSW1state=TOGGLE;
            SW1state = IDLE;
            break;
        case TOGGLESHORT:
            gs_prevSW1state=TOGGLESHORT;
            SW1state = IDLE;
            break;
        case TOGGLE2S:
            gs_prevSW1state=TOGGLE2S;
            SW1state = IDLE;
            break;
        default:
            break;
    }
}
struct ProcessEventSW1
{
        KEY_EVENT gs_SW1state;
        KEY_EVENT gs_prevSW1state;
        KEY_EVENT SW1state;
};

struct ProcessEventSW1 ProcessEventSW1TestCase [] =
{
        {TOGGLE, TOGGLE, IDLE},
        {TOGGLESHORT, TOGGLESHORT, IDLE},
        {TOGGLE2S, TOGGLE2S, IDLE}
};

class TestProcessEventSW1 : public ::testing::TestWithParam<ProcessEventSW1>
{
};
TEST_P(TestProcessEventSW1, TestProcessEventSW1Case)
{
    KEY_EVENT gs_SW1state = GetParam().gs_SW1state;
    KEY_EVENT gs_prevSW1state = GetParam().gs_prevSW1state;
    KEY_EVENT In_prevSW1state;
    KEY_EVENT SW1state = GetParam().SW1state;
    KEY_EVENT INSW1state;
    keyPanel_ProcessEventSW1(gs_SW1state, In_prevSW1state, INSW1state);
    EXPECT_EQ(In_prevSW1state, gs_prevSW1state);
    EXPECT_EQ(INSW1state, SW1state);
}
INSTANTIATE_TEST_CASE_P(TestProcessEventSW1,
                        TestProcessEventSW1,
                        testing::ValuesIn(ProcessEventSW1TestCase)
);

void keyPanel_ProcessEventSW2(KEY_EVENT gs_SW2state, KEY_EVENT &gs_prevSW2state, KEY_EVENT &SW2state)
{
    switch(gs_SW2state)
    {
        case TOGGLE:
            gs_prevSW2state=TOGGLE;
            SW2state = IDLE;
            break;
        case TOGGLESHORT:
            gs_prevSW2state=TOGGLESHORT;
            SW2state = IDLE;
            break;
        case TOGGLE2S:
            gs_prevSW2state=TOGGLE2S;
            SW2state = IDLE;
            break;
        default:
            break;
    }
}
struct ProcessEventSW2
{
        KEY_EVENT gs_SW2state;
        KEY_EVENT gs_prevSW2state;
        KEY_EVENT SW2state;
};

struct ProcessEventSW2 ProcessEventSW2TestCase [] =
{
        {TOGGLE, TOGGLE, IDLE},
        {TOGGLESHORT, TOGGLESHORT, IDLE},
        {TOGGLE2S, TOGGLE2S, IDLE}
};

class TestProcessEventSW2 : public ::testing::TestWithParam<ProcessEventSW2>
{
};
TEST_P(TestProcessEventSW2, TestProcessEventSW2Case)
{
    KEY_EVENT gs_SWstate = GetParam().gs_SW2state;
    KEY_EVENT gs_prevSWstate = GetParam().gs_prevSW2state;
    KEY_EVENT In_prevSWstate;
    KEY_EVENT SWstate = GetParam().SW2state;
    KEY_EVENT INSWstate;
    keyPanel_ProcessEventSW2(gs_SWstate, In_prevSWstate, INSWstate);
    EXPECT_EQ(In_prevSWstate, gs_prevSWstate);
    EXPECT_EQ(INSWstate, SWstate);
}
INSTANTIATE_TEST_CASE_P(TestProcessEventSW2,
                        TestProcessEventSW2,
                        testing::ValuesIn(ProcessEventSW2TestCase)
);

void keyPanel_ProcessEventSW3(KEY_EVENT gs_SW3state, KEY_EVENT &gs_prevSW3state, KEY_EVENT &SW3state)
{
    switch(gs_SW3state)
    {
        case TOGGLE:
            gs_prevSW3state=TOGGLE;
            SW3state = IDLE;
            break;
        case TOGGLESHORT:
            gs_prevSW3state=TOGGLESHORT;
            SW3state = IDLE;
            break;
        case TOGGLE2S:
            gs_prevSW3state=TOGGLE2S;
            SW3state = IDLE;
            break;
        default:
            break;
    }
}
struct ProcessEventSW3
{
        KEY_EVENT gs_SW3state;
        KEY_EVENT gs_prevSW3state;
        KEY_EVENT SW3state;
};

struct ProcessEventSW3 ProcessEventSW3TestCase [] =
{
        {TOGGLE, TOGGLE, IDLE},
        {TOGGLESHORT, TOGGLESHORT, IDLE},
        {TOGGLE2S, TOGGLE2S, IDLE}
};

class TestProcessEventSW3 : public ::testing::TestWithParam<ProcessEventSW3>
{
};
TEST_P(TestProcessEventSW3, TestProcessEventSW3Case)
{
    KEY_EVENT gs_SWstate = GetParam().gs_SW3state;
    KEY_EVENT gs_prevSWstate = GetParam().gs_prevSW3state;
    KEY_EVENT In_prevSWstate;
    KEY_EVENT SWstate = GetParam().SW3state;
    KEY_EVENT INSWstate;
    keyPanel_ProcessEventSW3(gs_SWstate, In_prevSWstate, INSWstate);
    EXPECT_EQ(In_prevSWstate, gs_prevSWstate);
    EXPECT_EQ(INSWstate, SWstate);
}
INSTANTIATE_TEST_CASE_P(TestProcessEventSW3,
                        TestProcessEventSW3,
                        testing::ValuesIn(ProcessEventSW3TestCase)
);

void keyPanel_ProcessEventSW4(KEY_EVENT gs_SW4state, KEY_EVENT &gs_prevSW4state, KEY_EVENT &SW4state)
{
    switch(gs_SW4state)
    {
        case TOGGLE:
            gs_prevSW4state=TOGGLE;
            SW4state = IDLE;
            break;
        case TOGGLESHORT:
            gs_prevSW4state=TOGGLESHORT;
            SW4state = IDLE;
            break;
        case TOGGLE2S:
            gs_prevSW4state=TOGGLE2S;
            SW4state = IDLE;
            break;
        default:
            break;
    }
}
struct ProcessEventSW4
{
        KEY_EVENT gs_SW4state;
        KEY_EVENT gs_prevSW4state;
        KEY_EVENT SW4state;
};

struct ProcessEventSW4 ProcessEventSW4TestCase [] =
{
        {TOGGLE, TOGGLE, IDLE},
        {TOGGLESHORT, TOGGLESHORT, IDLE},
        {TOGGLE2S, TOGGLE2S, IDLE}
};

class TestProcessEventSW4 : public ::testing::TestWithParam<ProcessEventSW4>
{
};
TEST_P(TestProcessEventSW4, TestProcessEventSW4Case)
{
    KEY_EVENT gs_SWstate = GetParam().gs_SW4state;
    KEY_EVENT gs_prevSWstate = GetParam().gs_prevSW4state;
    KEY_EVENT In_prevSWstate;
    KEY_EVENT SWstate = GetParam().SW4state;
    KEY_EVENT INSWstate;
    keyPanel_ProcessEventSW4(gs_SWstate, In_prevSWstate, INSWstate);
    EXPECT_EQ(In_prevSWstate, gs_prevSWstate);
    EXPECT_EQ(INSWstate, SWstate);
}
INSTANTIATE_TEST_CASE_P(TestProcessEventSW4,
                        TestProcessEventSW4,
                        testing::ValuesIn(ProcessEventSW4TestCase)
);

void keyPanel_ProcessEventSW5(KEY_EVENT gs_SW5state, KEY_EVENT &gs_prevSW5state, KEY_EVENT &SW5state)
{
    switch(gs_SW5state)
    {
        case TOGGLE:
            gs_prevSW5state=TOGGLE;
            SW5state = IDLE;
            break;
        case TOGGLESHORT:
            gs_prevSW5state=TOGGLESHORT;
            SW5state = IDLE;
            break;
        case TOGGLE2S:
            gs_prevSW5state=TOGGLE2S;
            SW5state = IDLE;
            break;
        default:
            break;
    }
}
struct ProcessEventSW5
{
        KEY_EVENT gs_SW5state;
        KEY_EVENT gs_prevSW5state;
        KEY_EVENT SW5state;
};

struct ProcessEventSW5 ProcessEventSW5TestCase [] =
{
        {TOGGLE, TOGGLE, IDLE},
        {TOGGLESHORT, TOGGLESHORT, IDLE},
        {TOGGLE2S, TOGGLE2S, IDLE}
};

class TestProcessEventSW5 : public ::testing::TestWithParam<ProcessEventSW5>
{
};
TEST_P(TestProcessEventSW5, TestProcessEventSW5Case)
{
    KEY_EVENT gs_SWstate = GetParam().gs_SW5state;
    KEY_EVENT gs_prevSWstate = GetParam().gs_prevSW5state;
    KEY_EVENT In_prevSWstate;
    KEY_EVENT SWstate = GetParam().SW5state;
    KEY_EVENT INSWstate;
    keyPanel_ProcessEventSW5(gs_SWstate, In_prevSWstate, INSWstate);
    EXPECT_EQ(In_prevSWstate, gs_prevSWstate);
    EXPECT_EQ(INSWstate, SWstate);
}
INSTANTIATE_TEST_CASE_P(TestProcessEventSW5,
                        TestProcessEventSW5,
                        testing::ValuesIn(ProcessEventSW5TestCase)
);













