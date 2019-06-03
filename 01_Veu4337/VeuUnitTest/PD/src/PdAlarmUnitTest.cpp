#include "EnableTestCase.h"
#ifdef PD_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "PDFixture.h"

#include "DebugTrace.h"
#include "PdAlarm.h"

#define DEFAUTL     (1)

using namespace ::testing;

static const ULONG O2TIMELIMIT = 1;//5;          // 30 seconds
static const ULONG O2RESETTIMELIMIT = 1;//2;    // 2 samples at 1 seconds intervals
static const ULONG ONE_SECOND = 1000;
static const ULONG TWO_SECOND = ONE_SECOND * 2;

PdAlarms* PdAlarms::S_Instance = NULL;

PdAlarms* PdAlarms::S_GetInstance (void)
{
    //Create object if it is null
    if(S_Instance == NULL)
    {
        S_Instance = new PdAlarms ();
    }
    //check it is null or not
    ASSERTION(S_Instance != NULL);


    return (S_Instance);
}

PdAlarms::PdAlarms (void)
{
    mHighO2AlarmDelayTimeAfterFlushO2 = 0;
    mLowO2AlarmDelayTimeAfterFlushO2 = 0;

}

void PdAlarms::CheckHiCondition (LONG hilimit, LONG value, int alarmID, int SensitiveValue, int SimulatesenValCounter)
{
    static int senValCounter = SimulatesenValCounter;

    //when value is higher than limit value, check alarm status
    if(value > hilimit)
    {
        //if alarm status is not active
        if(eNotActive ==  GetIsActive(alarmID))
        {
            //increase counter
            ++senValCounter;
            //if counter higher or equal SensitiveValue
            if(senValCounter >= SensitiveValue)
            {
                //Reset senVal
                senValCounter = 0;
                //set alarm status acitve
                SetIsActive (alarmID, eActive);
            }
        }
        //Do nothing when alarm status is active currently.

    }
    //when value is lower than limit value
    else
    {
        //if alarm status is still active, deactivate alarm status
        if(eActive ==  GetIsActive(alarmID))
        {
            //Reset senVal
            senValCounter = 0;
            //deactivate alarm status
            SetIsActive (alarmID, eNotActive);
        }
        //Do nothing when alarm status is currently deactivate
    }
}

void PdAlarms::CheckLowCondition (LONG lowlimit, LONG value, int alarmID, int SensitiveValue, int SimulatesenValCounter)
{
    static int senValCounter = SimulatesenValCounter;

    //when value is lower than limit value, check alarm status
    if(value < lowlimit)
    {
        //if alarm status is not active
        if(eNotActive ==  GetIsActive(alarmID))
        {
            //increase counter
            ++senValCounter;
            //if counter higher or equal SensitiveValue
            if(senValCounter >= SensitiveValue)
            {
                //Reset senVal
                senValCounter = 0;
                //set alarm status acitve
                SetIsActive (alarmID, eActive);
            }
        }
        //Do nothing when alarm status is active currently.

    }
    //when value is higher than limit value
    else
    {
        //if alarm status is still active, deactivate alarm status
        if(eActive ==  GetIsActive(alarmID))
        {
            //Reset senVal
            senValCounter = 0;
            //deactivate alarm status
            SetIsActive (alarmID, eNotActive);
        }
        //Do nothing when alarm status is currently deactivate
    }
}

void PdAlarms::CheckHiO2 (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    static ULONG notactivetime = 0;
    const LONG HIGH_O2_ALARM_DELAY_TIME_LIMIT = 0;
    const ULONG RESET = 0;

    //Do nothing when time delay after flushing O2 is equal LIMIT value
    //Do nothing when time delay after flushing O2 is equal LIMIT value
    if(mHighO2AlarmDelayTimeAfterFlushO2 >= HIGH_O2_ALARM_DELAY_TIME_LIMIT)
    {
        --mHighO2AlarmDelayTimeAfterFlushO2;
        return;
    }

    //when value is higher than limit and alarm status is not active
    if((value >= hilimit)
            && (eNotActive ==  GetIsActive (alarmID)))
    {
        //increase counter activetime
        ++activetime;
    }
    else
    {
        //Reset counter
        activetime = RESET;
    }

    //if activetime counter is passed, set alarm status active
    if(activetime >= O2TIMELIMIT)
    {
        SetIsActive (alarmID, eActive);
    }

    //when value is lower than limit and alarm status is active
    if((value < hilimit)
            && (eActive ==  GetIsActive (alarmID)))
    {
        //increase notactivetime counter
        ++notactivetime;
    }
    else
    {
        //Reset notactivetime counter
        notactivetime = RESET;
    }

    //when notactivetime counter is passed, set alarm status deactivate
    if(notactivetime >= O2RESETTIMELIMIT)
    {
        SetIsActive (alarmID, eNotActive);
    }

}

void PdAlarms::CheckLowO2 (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    static ULONG notactivetime = 0;
    const LONG LOW_O2_ALARM_DELAY_TIME_LIMIT = 0;
    const ULONG RESET = 0;

    if(mLowO2AlarmDelayTimeAfterFlushO2 >= LOW_O2_ALARM_DELAY_TIME_LIMIT)
    {
        --mLowO2AlarmDelayTimeAfterFlushO2;
        return;
    }

    if((value <= lowlimit)
            && (eNotActive ==  GetIsActive (alarmID)))
    {
        ++activetime;
    }
    else
    {
        activetime = RESET;
    }

    if(activetime >= O2TIMELIMIT)
    {
        SetIsActive (alarmID, eActive);
    }

    if((value > lowlimit)
            && (eActive ==  GetIsActive (alarmID)))
    {
        ++notactivetime;
    }
    else
    {
        notactivetime = RESET;
    }

    if(notactivetime >= O2RESETTIMELIMIT)
    {
        SetIsActive (alarmID, eNotActive);
    }

}

void PdAlarms::CheckHiInternalO2 (LONG value)
{
    // 25 percent O2 concentration
    const LONG HILIMIT = 568;
    static ULONG activetime = 0;
    const ULONG RESET = 0;

    if(value >= HILIMIT)
    {
        activetime++;
    }
    else
    {
        activetime = RESET;
    }

    //#ifndef AARC
    if(activetime >= O2TIMELIMIT)
    {
        SetIsActive (eHighInternalO2, eActive);
    }
    //#endif

}

void PdAlarms::ResetO2Time()
{
    //30 sec
    mHighO2AlarmDelayTimeAfterFlushO2 = 30;
    //30 sec
    mLowO2AlarmDelayTimeAfterFlushO2 = 30;

    //Reset alarm status of eHighO2 and eLowO2
    SetIsActive(eHighO2, eNotActive);
    SetIsActive(eLowO2, eNotActive);
}


void PdAlarms::CheckLowAmlpitude (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 6;

    if (value < lowlimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            activetime = RESET;
            if(eNotActive == GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eActive == GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckHighAmlpitude (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 6;

    if (value > hilimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {

            activetime = RESET;
            if(eNotActive == GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eActive == GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckLowBaseLine (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG LOW_PEEP_ACTIVE_TIME_LIMIT = 5;
    const ULONG RESET = 0;

    if (value < lowlimit)
    {
        lowPEEPActiveTime++;

        if(lowPEEPActiveTime >= LOW_PEEP_ACTIVE_TIME_LIMIT)
        {
            lowPEEPActiveTime = RESET;

            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        lowPEEPActiveTime = RESET;

        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}


void PdAlarms::CheckLowBaseLine2Secs (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_2SEC = 2;
    if (value > lowlimit)
    {
        lowPEEP2SecActiveTime = RESET;
        lowPEEP2SecClearTime += NUMBER_OF_2SEC;

        if(lowPEEP2SecClearTime >= TWO_SECOND)
        {
            lowPEEP2SecClearTime = RESET;
            if(eNotActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eNotActive);
            }
        }
    }
    else if(value < lowlimit)
    {
        lowPEEP2SecClearTime = RESET;
        lowPEEP2SecActiveTime += NUMBER_OF_2SEC;

        if(lowPEEP2SecActiveTime >= TWO_SECOND)
        {
            lowPEEP2SecActiveTime = RESET;
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
}

void PdAlarms::CheckHiBaseLine (LONG hilimit, LONG value, int alarmID)
{
    const ULONG HIGH_PEEP_ACTIVE_TIME_LIMIT = 5;
    const ULONG RESET = 0;

    if (value > hilimit)
    {
        highPEEPActiveTime++;
        if(highPEEPActiveTime >= HIGH_PEEP_ACTIVE_TIME_LIMIT)
        {
            highPEEPActiveTime = RESET;
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        highPEEPActiveTime = RESET;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckHiBaseLine2Secs (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_2SEC = 2;

    if (value < lowlimit)
    {
        highPEEP2SecActiveTime = RESET;
        highPEEP2SecClearTime += NUMBER_OF_2SEC;
        if(highPEEP2SecClearTime >= TWO_SECOND)
        {

            highPEEP2SecClearTime = RESET;
            if(eNotActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eNotActive);
            }
        }
    }
    else
    {
        highPEEP2SecClearTime = RESET;
        highPEEP2SecActiveTime += NUMBER_OF_2SEC;
        if(highPEEP2SecActiveTime >= TWO_SECOND)
        {

            highPEEP2SecActiveTime = RESET;
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
}

void PdAlarms::CheckHiPip (LONG hilimit, LONG value, int alarmID)
{
    if (value < hilimit)
    {
        SetIsActive(alarmID, eNotActive);
    }
}


void PdAlarms::CheckLowPip (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;

    if (value < lowlimit)
    {
        activetime++;
        if(activetime > 1)
        {
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = 0;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckLowMap (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG ACTIVE_TIME_LIMIT = 6;
    const ULONG RESET = 0;

    if (value < lowlimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            activetime = RESET;
            if(eNotActive == GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eActive == GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckHiMap (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value > hilimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckLowMinuteVol (LONG lowlimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value < lowlimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            activetime = RESET;
            if(eNotActive == GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eActive == GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckHiMinuteVol (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value > hilimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            if(eNotActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eNotActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckHiBreathRate (LONG hilimit, LONG value, int alarmID)
{
    static ULONG activetime = 0;
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value > hilimit)
    {
        activetime++;
        if(activetime > ACTIVE_TIME_LIMIT)
        {
            if(eNotActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eNotActive);
            }
        }
    }
    else
    {
        activetime = RESET;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::ResetAlarmActiveTime ()
{
    highPEEPActiveTime = 0;
    lowPEEPActiveTime = 0;

    highPEEP2SecActiveTime = 0;
    lowPEEP2SecActiveTime = 0;

    lowPEEP2SecClearTime = 0;
    highPEEP2SecClearTime = 0;

    ResetPLowAlarmActiveTime();
}

void PdAlarms::ResetPLowAlarmActiveTime ()
{
    highPLow1SecActiveTime = 0;
    lowPLow1SecActiveTime = 0;

    highPLow1SecClearTime = 0;
    lowPLow1SecClearTime = 0;

    highPHigh2SecActiveTime = 0;
    lowPHigh2SecActiveTime = 0;

    lowPHigh2SecClearTime = 0;
    highPHigh2SecClearTime = 0;
}
void PdAlarms::ResetPEEPAlarmActiveTime ()
{
    highPEEP2SecActiveTime = 0;
    lowPEEP2SecActiveTime = 0;

    lowPEEP2SecClearTime = 0;
    highPEEP2SecClearTime = 0;
}
void PdAlarms::CheckLow_PLow (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_LOWPLOW_ACTIVE_ONE_TIMES = 2;

    if (value < lowlimit)
    {
        lowPLow1SecClearTime = RESET;
        lowPLow1SecActiveTime += NUMBER_OF_LOWPLOW_ACTIVE_ONE_TIMES ;
        if(lowPLow1SecActiveTime >= ONE_SECOND)
        {
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else{
        lowPLow1SecActiveTime = RESET;
        lowPLow1SecClearTime += NUMBER_OF_LOWPLOW_ACTIVE_ONE_TIMES ;
        if(lowPLow1SecClearTime >= ONE_SECOND)
        {
            if(eNotActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eNotActive);
            }
        }
    }
}
void PdAlarms::CheckHigh_PLow (LONG hilimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_HIGHPLOW_ONE_TIMES = 2;

    if (value > hilimit)
    {
        highPLow1SecClearTime = RESET;
        highPLow1SecActiveTime += NUMBER_OF_HIGHPLOW_ONE_TIMES;

        if(highPLow1SecActiveTime >= ONE_SECOND)
        {
            highPLow1SecActiveTime = 0;
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        highPLow1SecActiveTime = RESET ;
        highPLow1SecClearTime += NUMBER_OF_HIGHPLOW_ONE_TIMES;
        if(highPLow1SecClearTime >= ONE_SECOND)
        {
            if(eNotActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eNotActive);
            }
        }
    }
}

void PdAlarms::CheckLowPLow5Time (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;
    if (value < lowlimit)
    {
        lowPLowActive5Times++;
        if(lowPLowActive5Times >= ACTIVE_TIME_LIMIT)
        {
            lowPLowActive5Times = RESET;
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        lowPLowActive5Times = RESET;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}
void PdAlarms::CheckHighPLow5Time (LONG hilimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value > hilimit)
    {
        highPLowActive5Times++;
        if(highPLowActive5Times >= ACTIVE_TIME_LIMIT)
        {
            highPLowActive5Times = RESET;
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        highPLowActive5Times = RESET;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }

}

void PdAlarms::CheckLow_PHigh (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_LOEHPHIGH_ONE_TIMES = 2;

    if (value < lowlimit)
    {
        lowPHigh2SecClearTime = RESET;
        lowPHigh2SecActiveTime += NUMBER_OF_LOEHPHIGH_ONE_TIMES ;
        if(lowPHigh2SecActiveTime >= TWO_SECOND)
        {
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else{
        lowPHigh2SecActiveTime = RESET;
        lowPHigh2SecClearTime += NUMBER_OF_LOEHPHIGH_ONE_TIMES;
        if(lowPHigh2SecClearTime >= TWO_SECOND)
        {
            if(eNotActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eNotActive);
            }
        }
    }
}


void PdAlarms::CheckHigh_PHigh (LONG hilimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG NUMBER_OF_LOEHPHIGH_ONE_TIMES = 2;

    if (value > hilimit)
    {
        highPHigh2SecClearTime = RESET;
        highPHigh2SecActiveTime += NUMBER_OF_LOEHPHIGH_ONE_TIMES ;
        if(highPHigh2SecActiveTime >= TWO_SECOND)
        {
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }else{
        highPHigh2SecActiveTime = RESET;
        highPHigh2SecClearTime += NUMBER_OF_LOEHPHIGH_ONE_TIMES;
        if(highPHigh2SecClearTime >= TWO_SECOND)
        {
            if(eNotActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eNotActive);
            }
        }
    }
}

void PdAlarms::CheckLowPHigh5Time (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;
    const ULONG ACTIVE_TIME_LIMIT = 5;

    if (value < lowlimit)
    {
        lowPHighActive5Times++;
        if(lowPHighActive5Times >= ACTIVE_TIME_LIMIT)
        {
            lowPHighActive5Times = RESET;
            if(eActive != GetIsActive(alarmID))
            {
                SetIsActive(alarmID, eActive);
            }
        }
    }
    else
    {
        lowPHighActive5Times = RESET;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

void PdAlarms::CheckHighPHigh5Time (LONG lowlimit, LONG value, int alarmID)
{
    const ULONG RESET = 0;

    if (value > lowlimit)
    {

    }
    else
    {
        highPHighActive5Times = RESET;
        if(eNotActive != GetIsActive(alarmID))
        {
            SetIsActive(alarmID, eNotActive);
        }
    }
}

class PdAlarmUnitMock : public PDTestFixture
{
public:
    PdAlarmUnitMock(void) : PDTestFixture(new BaseModuleMock)
{
}
    virtual ~PdAlarmUnitMock(void){}
    virtual void SetUp(void)
    {
        MyPdAlarm = new PdAlarms();
    }
    virtual void TearDown(void)
    {
        delete MyPdAlarm;
    }
    PdAlarms *MyPdAlarm;
};

//test for PdAlarms* PdAlarms::S_GetInstance (void)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_S_GetInstance)
{
    MyPdAlarm->S_GetInstance();

    EXPECT_NE((void*)0, MyPdAlarm->S_Instance);
}

//test for PdAlarms::PdAlarms (void)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_PdAlarms)
{
    EXPECT_EQ(0, MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2);
    EXPECT_EQ(0, MyPdAlarm->mLowO2AlarmDelayTimeAfterFlushO2);
}

//test for void PdAlarms::CheckHiCondition (LONG hilimit, LONG value, int alarmID, int SensitiveValue)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiConditionLow)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckHiCondition(1, 2, DEFAUTL, 0, 1);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiConditionHigh)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);
    MyPdAlarm->CheckHiCondition(2, 1, DEFAUTL, 0, 1);
}

//test for void PdAlarms::CheckLowCondition (LONG lowlimit, LONG value, int alarmID, int SensitiveValue)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowConditionLow)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckLowCondition(2, 1, DEFAUTL, 0, 2);
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowConditionHi)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHiCondition(2, 1, DEFAUTL, 0, 2);
}

//test for void PdAlarms::CheckHiO2 (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiO2_LIMIT_value)
{
    MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2 = 1;
    MyPdAlarm->CheckHiO2(DEFAUTL, DEFAUTL, DEFAUTL);

    EXPECT_EQ(0, MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2);

    MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2 = 2;
    MyPdAlarm->CheckHiO2(DEFAUTL, DEFAUTL, DEFAUTL);

    EXPECT_EQ(1, MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiO2_LIMIT_Value_Higher)
{
    MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2 = -1;
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));

    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckHiO2(0, 2, DEFAUTL);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiO2_LIMIT_Value_Lower)
{
    MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2 = -1;
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));

    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHiO2(2, 0, DEFAUTL);
}

//test for void PdAlarms::CheckLowO2 (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowO2_LIMIT_value)
{
    MyPdAlarm->mLowO2AlarmDelayTimeAfterFlushO2 = 1;
    MyPdAlarm->CheckLowO2(DEFAUTL, DEFAUTL, DEFAUTL);

    EXPECT_EQ(0, MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2);

    MyPdAlarm->mLowO2AlarmDelayTimeAfterFlushO2 = 2;
    MyPdAlarm->CheckLowO2(DEFAUTL, DEFAUTL, DEFAUTL);

    EXPECT_EQ(1, MyPdAlarm->mLowO2AlarmDelayTimeAfterFlushO2);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowO2_LIMIT_Value_Higher)
{
    MyPdAlarm->mLowO2AlarmDelayTimeAfterFlushO2 = -1;
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));

    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckLowO2(2, 0, DEFAUTL);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowO2_LIMIT_Value_Lower)
{
    MyPdAlarm->mLowO2AlarmDelayTimeAfterFlushO2 = -1;
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));

    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowO2(0, 2, DEFAUTL);
}

//test void PdAlarms::CheckHiInternalO2 (LONG value)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiInternalO2)
{
    const LONG HILIMIT = 568;
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(eHighInternalO2, eActive)).Times(2);

    MyPdAlarm->CheckHiInternalO2(HILIMIT);
    MyPdAlarm->CheckHiInternalO2(HILIMIT + 1);
}

//test for void PdAlarms::ResetO2Time()
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_ResetO2Time)
{
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(eHighO2, eNotActive)).Times(1);
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(eLowO2, eNotActive)).Times(1);

    MyPdAlarm->ResetO2Time();

    EXPECT_EQ(30, MyPdAlarm->mHighO2AlarmDelayTimeAfterFlushO2);
    EXPECT_EQ(30, MyPdAlarm->mLowO2AlarmDelayTimeAfterFlushO2);
}

//test for void PdAlarms::CheckLowAmlpitude (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowAmlpitude_caseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowAmlpitude(1, 2, DEFAUTL);
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowAmlpitude_caseTrue)
{
    const int ACTIVE_TIME_LIMIT = 6;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    for(int i = 0; i < ACTIVE_TIME_LIMIT + 1; ++i)
    {
        MyPdAlarm->CheckLowAmlpitude(2, 1, DEFAUTL);
    }
}

//test for void PdAlarms::CheckHighAmlpitude (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHighAmlpitude_caseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHighAmlpitude(2, 1, DEFAUTL);
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHighAmlpitude_caseTrue)
{
    const int ACTIVE_TIME_LIMIT = 6;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    for(int i = 0; i < ACTIVE_TIME_LIMIT + 1; ++i)
    {
        MyPdAlarm->CheckHighAmlpitude(1, 2, DEFAUTL);
    }
}

//test for void PdAlarms::CheckLowBaseLine (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowBaseLine_caseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowBaseLine(1, 2, DEFAUTL);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowBaseLine_caseTrue)
{
    const ULONG LOW_PEEP_ACTIVE_TIME_LIMIT = 5;
    MyPdAlarm->lowPEEPActiveTime = 6;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    for(ULONG i = 0; i < LOW_PEEP_ACTIVE_TIME_LIMIT; ++i)
    {
        MyPdAlarm->CheckLowBaseLine(2, 1, DEFAUTL);
    }
}

//test for void PdAlarms::CheckLowBaseLine2Secs (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowBaseLine2Secs_caseTrue)
{
    MyPdAlarm->lowPEEP2SecClearTime = TWO_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowBaseLine2Secs(1, 2, DEFAUTL);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowBaseLine2Secs_caseFalse)
{
    MyPdAlarm->lowPEEP2SecActiveTime = TWO_SECOND + 2;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckLowBaseLine2Secs(2, 1, DEFAUTL);
}

//test for void PdAlarms::CheckHiBaseLine (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiBaseLine_caseTrue)
{
    const ULONG HIGH_PEEP_ACTIVE_TIME_LIMIT = 5;
    MyPdAlarm->highPEEPActiveTime = HIGH_PEEP_ACTIVE_TIME_LIMIT + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckHiBaseLine(1, 2, DEFAUTL);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiBaseLine_caseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHiBaseLine(2, 1, DEFAUTL);
}

//test for void PdAlarms::CheckHiBaseLine2Secs (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiBaseLine2Secs_caseTrue)
{
    MyPdAlarm->highPEEP2SecClearTime = TWO_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHiBaseLine2Secs(2, 1, DEFAUTL);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiBaseLine2Secs_caseFalse)
{
    MyPdAlarm->highPEEP2SecActiveTime = TWO_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckHiBaseLine2Secs(1, 2, DEFAUTL);
}

//test for void PdAlarms::CheckHiPip (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiPip)
{
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);
    MyPdAlarm->CheckHiPip(2, 1 , DEFAUTL);
}

//test for void PdAlarms::CheckLowPip (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowPip_caseTrue)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    for(int i = 0; i < 2; ++i)
    {
        MyPdAlarm->CheckLowPip(2, 1, DEFAUTL);
    }
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowPip_casefalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowPip(1, 2, DEFAUTL);
}

//test for void PdAlarms::CheckLowMap (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowMap_caseTrue)
{
    const ULONG ACTIVE_TIME_LIMIT = 6;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    for(ULONG i = 0; i < ACTIVE_TIME_LIMIT + 2; ++i)
    {
        MyPdAlarm->CheckLowMap(2, 1, DEFAUTL);
    }
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowMap_caseFlase)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowMap(1, 2, DEFAUTL);
}

//test for void PdAlarms::CheckHiMap (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiMap_caseTrue)
{
    const ULONG ACTIVE_TIME_LIMIT = 5;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    for(ULONG i = 0; i < ACTIVE_TIME_LIMIT + 1; ++i)
    {
        MyPdAlarm->CheckHiMap(1, 2, DEFAUTL);
    }
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiMap_caseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHiMap(2, 1, DEFAUTL);
}

//test for void PdAlarms::CheckLowMinuteVol (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowMinuteVol_caseTrue)
{
    const ULONG ACTIVE_TIME_LIMIT = 5;
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);


    for(ULONG i = 0; i < ACTIVE_TIME_LIMIT + 1; ++i)
    {
        MyPdAlarm->CheckLowMinuteVol(2, 1, DEFAUTL);
    }
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowMinuteVol_caseFlase)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowMinuteVol(1, 2, DEFAUTL);
}

//test for void PdAlarms::CheckHiMinuteVol (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiMinuteVol_caseTrue)
{
    const ULONG ACTIVE_TIME_LIMIT = 5;
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);


    for(ULONG i = 0; i < ACTIVE_TIME_LIMIT + 1; ++i)
    {
        MyPdAlarm->CheckHiMinuteVol(1, 2, DEFAUTL);
    }
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiMinuteVol_caseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHiMinuteVol(2, 1, DEFAUTL);
}

//test for void PdAlarms::CheckHiBreathRate (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiBreathRate_caseTrue)
{
    const ULONG ACTIVE_TIME_LIMIT = 5;
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);


    for(ULONG i = 0; i < ACTIVE_TIME_LIMIT + 1; ++i)
    {
        MyPdAlarm->CheckHiBreathRate(1, 2, DEFAUTL);
    }
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHiBreathRate_caseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHiBreathRate(2, 1, DEFAUTL);
}

//test for void PdAlarms::ResetAlarmActiveTime ()
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_ResetAlarmActiveTime)
{
    MyPdAlarm->ResetAlarmActiveTime();

    EXPECT_EQ(0, MyPdAlarm->highPEEPActiveTime);
    EXPECT_EQ(0, MyPdAlarm->lowPEEPActiveTime);
    EXPECT_EQ(0, MyPdAlarm->highPEEP2SecActiveTime);
    EXPECT_EQ(0, MyPdAlarm->lowPEEP2SecActiveTime);
    EXPECT_EQ(0, MyPdAlarm->lowPEEP2SecClearTime);
    EXPECT_EQ(0, MyPdAlarm->highPEEP2SecClearTime);
}

//test for void PdAlarms::ResetPLowAlarmActiveTime ()
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_ResetPLowAlarmActiveTime)
{
    MyPdAlarm->ResetPLowAlarmActiveTime();

    EXPECT_EQ(0, MyPdAlarm->highPLow1SecActiveTime);
    EXPECT_EQ(0, MyPdAlarm->lowPLow1SecActiveTime);
    EXPECT_EQ(0, MyPdAlarm->highPLow1SecClearTime);
    EXPECT_EQ(0, MyPdAlarm->lowPLow1SecClearTime);
    EXPECT_EQ(0, MyPdAlarm->highPHigh2SecActiveTime);
    EXPECT_EQ(0, MyPdAlarm->lowPHigh2SecActiveTime);
    EXPECT_EQ(0, MyPdAlarm->lowPHigh2SecClearTime);
    EXPECT_EQ(0, MyPdAlarm->highPHigh2SecClearTime);
}

//test for void PdAlarms::ResetPEEPAlarmActiveTime ()
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_ResetPEEPAlarmActiveTime)
{
    MyPdAlarm->ResetPEEPAlarmActiveTime();

    EXPECT_EQ(0, MyPdAlarm->highPEEP2SecActiveTime);
    EXPECT_EQ(0, MyPdAlarm->lowPEEP2SecActiveTime);
    EXPECT_EQ(0, MyPdAlarm->lowPEEP2SecClearTime);
    EXPECT_EQ(0, MyPdAlarm->highPEEP2SecClearTime);
}

//test for void PdAlarms::CheckLow_PLow (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLow_PLow_CaseTrue)
{
    MyPdAlarm->lowPLow1SecActiveTime = ONE_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckLow_PLow(2, 1, DEFAUTL);

}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLow_PLow_CaseFalse)
{
    MyPdAlarm->lowPLow1SecClearTime = ONE_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLow_PLow(1, 2, DEFAUTL);
}

//test for void PdAlarms::CheckHigh_PLow (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHigh_PLow_CaseTrue)
{
    MyPdAlarm->highPLow1SecActiveTime = ONE_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckHigh_PLow(1, 2, DEFAUTL);
}

TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHigh_PLow_CaseFlase)
{
    MyPdAlarm->highPLow1SecClearTime = ONE_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHigh_PLow(2, 1, DEFAUTL);
}
//test for void PdAlarms::CheckLowPLow5Time (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowPLow5Time_CaseTrue)
{
    const ULONG ACTIVE_TIME_LIMIT = 5;
    MyPdAlarm->lowPLowActive5Times = ACTIVE_TIME_LIMIT + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckLowPLow5Time(2, 1, DEFAUTL);
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowPLow5Time_CaseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowPLow5Time(1, 2, DEFAUTL);
}

//test for void PdAlarms::CheckHighPLow5Time (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHighPLow5Time_CaseTrue)
{
    const ULONG ACTIVE_TIME_LIMIT = 5;
    MyPdAlarm->highPLowActive5Times = ACTIVE_TIME_LIMIT + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckHighPLow5Time(1, 2, DEFAUTL);
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHighPLow5Time_CaseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHighPLow5Time(2, 1, DEFAUTL);
}

//test for void PdAlarms::CheckLow_PHigh (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLow_PHigh_CaseTrue)
{
    MyPdAlarm->lowPHigh2SecActiveTime = TWO_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckLow_PHigh(2, 1, DEFAUTL);
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLow_PHigh_CaseFalse)
{
    MyPdAlarm->lowPHigh2SecClearTime = TWO_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLow_PHigh(1, 2, DEFAUTL);
}

//test for void PdAlarms::CheckHigh_PHigh (LONG hilimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHigh_PHigh_CaseTrue)
{
    MyPdAlarm->highPHigh2SecActiveTime = TWO_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckHigh_PHigh(1, 2, DEFAUTL);
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHigh_PHigh_CaseFalse)
{
    MyPdAlarm->highPHigh2SecClearTime = TWO_SECOND + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHigh_PHigh(2, 1, DEFAUTL);
}

//test for void PdAlarms::CheckLowPHigh5Time (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowPHigh5Time_CaseTrue)
{
    const ULONG ACTIVE_TIME_LIMIT = 5;
    MyPdAlarm->lowPHighActive5Times = ACTIVE_TIME_LIMIT + 1;

    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eNotActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eActive)).Times(1);

    MyPdAlarm->CheckLowPHigh5Time(2, 1, DEFAUTL);
}
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckLowPHigh5Time_CaseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckLowPHigh5Time(1, 2, DEFAUTL);
}

//test for void PdAlarms::CheckHighPHigh5Time (LONG lowlimit, LONG value, int alarmID)
TEST_F(PdAlarmUnitMock, PdAlarmUnitMock_CheckHighPHigh5Time_CaseFalse)
{
    EXPECT_CALL(*_PdAlarmMock, GetIsActive(DEFAUTL)).Times(1).WillOnce(Return(eActive));
    EXPECT_CALL(*_PdAlarmMock, SetIsActive(DEFAUTL, eNotActive)).Times(1);

    MyPdAlarm->CheckHighPHigh5Time(2, 1, DEFAUTL);
}

#endif  //end define PD_UNIT_TEST
