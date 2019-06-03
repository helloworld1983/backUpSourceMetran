#include "EnableTestCase.h"
#ifdef BD_TRIGGER_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "occltrigger.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"

OcclTrigger* OcclTrigger::S_Instance = NULL;

const SHORT MAX_INH_PRESS_SCALED_IN_IMV = 125 * SCALE;     // 125 cmH2O, scaled
const SHORT MAX_EXH_PRESS_SCALED_IN_IMV = 125 * SCALE;     // 125 cmH2O, scaled
const SHORT MAX_PRESS_SCALED_IN_HFO = 200 * SCALE;     // 125 cmH2O, scaled

OcclTrigger* OcclTrigger::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new OcclTrigger();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}
OcclTrigger::OcclTrigger (E_TriggerId triggerId, LONG resistance, LONG offset) :
                                Trigger (triggerId)
{
    // Store the Resistance and Offset in the private attributes.
    Resistance = resistance;
    Offset = offset;

    // Initialize condition status to FALSE.
    OcclCondition[ePvFlowRes].IsCondMet = eFalse;
    OcclCondition[ePexhPdiff].IsCondMet = eFalse;

    // Time limits are each 100 ms
    OcclCondition[ePvFlowRes].TimeLimit = 100 / MS_PER_TICK;
    OcclCondition[ePexhPdiff].TimeLimit = 100 / MS_PER_TICK;

}   // end OcclTrigger()
OcclTrigger::OcclTrigger(): Trigger(eOcclusionTrigger)
{

    LastExhPress = 0;
    Resistance = 0;
    Offset = 0;

    for(int i = 0;i< eNumOcclIds; i++)
    {
        OcclCondition[i].TimeFirstMet = 0;
        OcclCondition[i].TimeLimit = 0;
        OcclCondition[i].IsCondMet = eFalse;
    }

}
void OcclTrigger::EnableTrigger (void)
{
    //    LastExhPress = ExhPressSensor->GetCurrentReading();

    OcclCondition[ePvFlowRes].IsCondMet = eFalse;
    OcclCondition[ePexhPdiff].IsCondMet = eFalse;

    // Invoke base class method.
    Trigger::EnableTrigger ();

}   // end EnableTrigger()
E_Bool OcclTrigger::CheckCondition (SHORT xexhPress, SHORT yinhPress)
{
    SHORT exhPress = xexhPress;             // pressure measured in the exhalation limb
    SHORT inhPress = yinhPress;             // pressure measured in the inhalation limb

//    DeviceInterface::S_GetInstance()->RequestDevice(eInhPressSensorGetLastReading, &inhPress);
//    DeviceInterface::S_GetInstance()->RequestDevice(eExhPressSensorGetLastReading, &exhPress);

    E_VentilationMode currentMode = BDModeMgrGetCurrentModeId();
    switch (currentMode)
    {
    case eHFOMode:
        if ((inhPress > MAX_PRESS_SCALED_IN_HFO) || (exhPress > MAX_PRESS_SCALED_IN_HFO))
        {
            TriggerDidFire = eTrue;
        }
        break;
    case eMixedMode:
    case eSpontaneousMode:
        if ((inhPress > MAX_INH_PRESS_SCALED_IN_IMV) || (exhPress > MAX_EXH_PRESS_SCALED_IN_IMV))
        {
            TriggerDidFire = eTrue;
        }
        break;
    default:
        TriggerDidFire = eFalse;
        break;
    }

    return(TriggerDidFire);

}   // end CheckCondition()

class OcclTriggerUnitMock : public BDTestFixture
{
public:
    OcclTriggerUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~OcclTriggerUnitMock(void){}
    virtual void SetUp(void)
    {
        MyOcclTrigger = new OcclTrigger(eFirstHFOTrigger, 1, 2);
        SingleOcclTrigger = new OcclTrigger();

    }
    virtual void TearDown(void)
    {
        delete MyOcclTrigger;
        delete SingleOcclTrigger;
    }
    OcclTrigger *MyOcclTrigger;
    OcclTrigger *SingleOcclTrigger;
};

//test for OcclTrigger* OcclTrigger::S_GetInstance(void)
TEST_F(OcclTriggerUnitMock, S_GetInstance)
{
    MyOcclTrigger->S_GetInstance();

    EXPECT_NE(MyOcclTrigger->S_Instance, (void*)0);
}

//test for OcclTrigger::OcclTrigger (E_TriggerId triggerId, LONG resistance, LONG offset) :
TEST_F(OcclTriggerUnitMock, OcclTrigger_withPara)
{
    EXPECT_EQ(MyOcclTrigger->Resistance, 1);
    EXPECT_EQ(MyOcclTrigger->Offset, 2);

    EXPECT_EQ(MyOcclTrigger->OcclCondition[ePvFlowRes].IsCondMet, eFalse);
    EXPECT_EQ(MyOcclTrigger->OcclCondition[ePexhPdiff].IsCondMet, eFalse);

    EXPECT_EQ(MyOcclTrigger->OcclCondition[ePexhPdiff].IsCondMet, eFalse);
    EXPECT_EQ(MyOcclTrigger->OcclCondition[ePexhPdiff].IsCondMet, eFalse);

}

//test for OcclTrigger::OcclTrigger(): Trigger(eOcclusionTrigger)
TEST_F(OcclTriggerUnitMock, OcclTrigger_Trigger)
{
    EXPECT_EQ(SingleOcclTrigger->LastExhPress, 0);
    EXPECT_EQ(SingleOcclTrigger->Resistance, 0);
    EXPECT_EQ(SingleOcclTrigger->Offset, 0);

    for(int i = 0;i< eNumOcclIds; i++)
    {
        EXPECT_EQ(SingleOcclTrigger->OcclCondition[i].TimeFirstMet, 0);
        EXPECT_EQ(SingleOcclTrigger->OcclCondition[i].TimeLimit, 0);
        EXPECT_EQ(SingleOcclTrigger->OcclCondition[i].IsCondMet, eFalse);
    }

}

//test for void OcclTrigger::EnableTrigger (void)
TEST_F(OcclTriggerUnitMock, EnableTrigger)
{
    MyOcclTrigger->EnableTrigger();

    EXPECT_EQ(MyOcclTrigger->OcclCondition[ePvFlowRes].IsCondMet, eFalse);
    EXPECT_EQ(MyOcclTrigger->OcclCondition[ePexhPdiff].IsCondMet, eFalse);
}

//test for E_Bool OcclTrigger::CheckCondition (SHORT xexhPress, SHORT yinhPress)
TEST_F(OcclTriggerUnitMock, CheckCondition_0)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eHFOMode));

    EXPECT_EQ(MyOcclTrigger->CheckCondition(MAX_PRESS_SCALED_IN_HFO + 1, 0), eTrue);
}
TEST_F(OcclTriggerUnitMock, CheckCondition_1)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eHFOMode));

    EXPECT_EQ(MyOcclTrigger->CheckCondition(0, MAX_PRESS_SCALED_IN_HFO + 1), eTrue);
}
TEST_F(OcclTriggerUnitMock, CheckCondition_2)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eMixedMode));

    EXPECT_EQ(MyOcclTrigger->CheckCondition(MAX_EXH_PRESS_SCALED_IN_IMV + 1, 0), eTrue);
}
TEST_F(OcclTriggerUnitMock, CheckCondition_3)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eSpontaneousMode));

    EXPECT_EQ(MyOcclTrigger->CheckCondition(MAX_EXH_PRESS_SCALED_IN_IMV + 1, 0), eTrue);
}
TEST_F(OcclTriggerUnitMock, CheckCondition_4)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eMixedMode));

    EXPECT_EQ(MyOcclTrigger->CheckCondition(0, MAX_INH_PRESS_SCALED_IN_IMV + 1), eTrue);
}
TEST_F(OcclTriggerUnitMock, CheckCondition_5)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eSpontaneousMode));

    EXPECT_EQ(MyOcclTrigger->CheckCondition(0, MAX_INH_PRESS_SCALED_IN_IMV + 1), eTrue);
}
TEST_F(OcclTriggerUnitMock, CheckCondition_6)
{
    EXPECT_CALL(*_ModeMgr, BDModeMgrGetCurrentModeId()).Times(1).WillOnce(Return(eSelfTestMode));
    EXPECT_EQ(MyOcclTrigger->CheckCondition(0, 0), eFalse);
}

#endif //end define BD_TRIGGER_UNIT_TEST
