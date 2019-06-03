#include "EnableTestCase.h"
#ifdef BD_MODE_UNIT_TEST

#include "gtest.h"
#include "gmock.h"
#include "BDFixture.h"
#include "ApneaBackupMode.h"
#include "DebugTrace.h"

using namespace ::testing;

#include "ModeMgrMockHeader.h"
#include "PhaseMgrMockHeader.h"
#include "AlarmMockHeader.h"
#include "TriggerMgrMockHeader.h"
#include "StepMotorMockHeader.h"
#include "MgrBdSettingMockHeader.h"
#include "BreathMgrMockHeader.h"


ApneaBackupMode* ApneaBackupMode::S_Instance = NULL;

ApneaBackupMode* ApneaBackupMode::S_GetInstance(void)
{
    // If the object has not already been created, create it.
    if (NULL == S_Instance)
    {
        S_Instance = new ApneaBackupMode();
        ASSERTION(S_Instance != NULL);
    }

    return(S_Instance);

}   // end S_GetInstance()

void ApneaBackupMode::ProcessTriggerID (E_TriggerId id)
{
    switch (id)
    {
        case ePtInhTrigger:
            ProcessPtInhTrigger();
            break;

        case eAlarmResetTrigger:
            ProcessAlarmResetTrigger();
            break;

        case eManualInhOpReqTrigger:
            ProcessManualInhOpReqTrigger();
            break;

        case eMandPressInhTimeTrigger:
            ProcessMandPressInhTimeTrigger();
            break;


        case eVcvInhTimeTrigger:
            ProcessVcvInhTimeTrigger();
            break;

        case ePlateauTimeTrigger:
            ProcessPlateauTimeTrigger();
            break;

        case eBpTimeTrigger:
            ProcessBpTimeTrigger();
            break;

        case eApneaAlarmTimeTrigger:
            ProcessApneaAlarmTimeTrigger();
            break;

        default:
            ASSERTION(eFalse);
            break;
    }
}

ApneaBackupMode::ApneaBackupMode(void):MixedMode()
{
    fromMode = eStandbyMode;
    IsPtInhTrigger = false;
}

void ApneaBackupMode::Enter(E_VentilationMode fromModeId)
{
    switch(fromModeId)
    {
        case eHighPressMode:
        case eHardwareErrorMode:
            ModeMgrGoToOperatorSetMode();
            break;
        default:
            fromMode = fromModeId;
            IsPtInhTrigger = false;
            Window = eMandWindow;

            E_State TempState;
//            TempState = eOn;
//            BDRequestDevice(eInpedanceLineSetStateeOn);
//            TempState = eOff;
//            BDRequestDevice(eInpedanceLineSetDesiredStateeOff);
//            TempState = eClose;
//            BDRequestDevice(eInpedanceLineSetDesiredStateeOff);

            //Disable all trigger
//            TriggerMgr* triggerMgrPtr = TriggerMgr::S_GetInstance();
            triggerMgrPtrDisableAllTriggers();

            //Enable IMV trigger
            triggerMgrPtrEnableIMVTrigger();

            //Disable eApneaAlarmTimeTrigger
            triggerMgrPtrDisableTrigger(eApneaAlarmTimeTrigger);

            UNSIGNED breathPeriod = triggerMgrPtrGetTimePeriod(eBpBackupTimeTrigger);
            triggerMgrPtrEnableTriggerWith(eBpBackupTimeTrigger,breathPeriod);


//            PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

            E_VentilationType currentVentType = phaseMgrPtrGetCurrentVentType();

            if(ePressureVentilation == currentVentType)
            {
                BDRequestDevice(eCMVServoControllerEnable);
            }
            else
            {
                BDRequestDevice(eVCVServoControllerEnable);
            }

            phaseMgrPtrProcessNewPhase(eMandInhPhase);

            BDAlarmTaskSetIsActive(eApnea, eActive);

            break;
    }
    //    AlarmSettingLogEntry entry;
    //    entry.strID = eSTR_LOG_BackupModeStart;
    //    entry.dateTimeStr = QDateTime::currentDateTime().toString(LOG_DATE_TIME_FORMAT);
    //    LogFileHandlerpostCustomEvent<AlarmSettingLogEntry>(LogFileHandler::eLogHandlerAlarmSettingLogFileType, entry);
}
void ApneaBackupMode::Exit()
{
    BDAlarmTaskSetIsActive(eApnea, eNotActive);

    MgrBreathDataInitializeBreathDataPtr();
    BDRequestDevice(eCMVServoControllerDisable);
    BDRequestDevice(eVCVServoControllerDisable);

    //    AlarmSettingLogEntry entry;
    //    entry.strID = eSTR_LOG_BackupModeStop;
    //    entry.dateTimeStr = QDateTime::currentDateTime().toString(LOG_DATE_TIME_FORMAT);
    //    LogFileHandlerpostCustomEvent<AlarmSettingLogEntry>(LogFileHandler::eLogHandlerAlarmSettingLogFileType, entry);
}
void ApneaBackupMode::ProcessPtInhTrigger(void)
{
    // Initialize pointer to the PhaseMgr object
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // A patient trigger should only fire during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);

    // Start a new inhalation.  The type of inhalation (mand or
    // spont) depends upon the window that is currently
    // open.  If the mandatory window is open, deliver a mand
    // breath and close the mandatory window since this breath
    // will meet the mandatory breath criteria for this breath
    // period
    if (eMandWindow == Window)
    {
        phaseMgrPtrProcessNewPhase(eMandInhPhase);

        // Close the mandatory window; open the spontaneous window.
        Window = eSpontWindow;
    }

    else if (eSpontWindow == Window)
    {
        // If the spontaneous window is open, deliver a spontaneous
        // inhalation.  The spontaneous window remains open until the
        // breath period timer elapses.
        phaseMgrPtrProcessNewPhase(eMandInhPhase);
    }
    else
    {
        // Window contains illegal value
        ASSERTION((Window == eMandWindow) || (Window == eSpontWindow));
    }


    IsPtInhTrigger = true;

    //    E_VentilationMode nextMode = (E_VentilationMode)MgrBdSettingGetCurrentValue(eMode);

}   // end ProcessPtInhTrigger()
void ApneaBackupMode::ProcessAlarmResetTrigger()
{
    //    E_VentilationMode nextMode = (E_VentilationMode)MgrBdSettingGetCurrentValue(eMode);

    MgrBreathDataSetFirstBreath();

    ModeMgrGoToOperatorSetMode();
}
void ApneaBackupMode::ProcessManualInhOpReqTrigger()
{
//    PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

    // Operator initiated inhalation triggers should only fire
    // during the exhalation phase.
    ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eExhPhase);


    TriggerMgrReEnableTrigger(eBpBackupTimeTrigger);

    // Start a mandatory inhalation.
    phaseMgrPtrProcessNewPhase(eMandInhPhase);
    Window = eMandWindow;

}
void ApneaBackupMode::ProcessMandPressInhTimeTrigger()
{

    if(!IsPtInhTrigger)
    {
        // Get a pointer to the PhaseMgr object
//        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        // If the breath phase is a mandatory inhalation, transition to exhalation.
        // Otherwise, perform error handling.
        ASSERTION(phaseMgrPtr->GetCurrentPhaseId() == eMandInhPhase);
        phaseMgrPtrProcessNewPhase(eExhPhase);
    }
    else
    {
        ModeMgrGoToOperatorSetMode();
    }
}
void ApneaBackupMode::ProcessVcvInhTimeTrigger()
{
    if(!IsPtInhTrigger)
    {
        // Get a handle to the PhaseMgr
//        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        // The trigger should only fire in the mandatory inhalation phase.
        ASSERTION (phaseMgrPtr->GetCurrentPhaseId() == eMandInhPhase);

        // Assume that the ventilator will transition to plateau
        // from a VCV inhalation.  It is possible that the ventilator
        // will not go to plateau but instead will go to exhalation.
        // The phase manager takes care of that.
        phaseMgrPtrProcessNewPhase(ePlateauPhase);

    }
    else
    {
        ModeMgrGoToOperatorSetMode();
    }
}
void ApneaBackupMode::ProcessPlateauTimeTrigger()
{
    if(!IsPtInhTrigger)
    {
//        PhaseMgr* phaseMgrPtr = PhaseMgr::S_GetInstance();

        // Plateau Time Trigger is only valid in the Plateau Phase.
        ASSERTION (phaseMgrPtr->GetCurrentPhaseId() == ePlateauPhase);

        // Transition to the exhalation phase.
        phaseMgrPtrProcessNewPhase(eExhPhase);

    }
    else
    {
        ModeMgrGoToOperatorSetMode();
    }
}
void ApneaBackupMode::ProcessBpTimeTrigger(void)
{
    E_PhaseId phaseId;          // Current Phase ID
//    PhaseMgr* phaseMgrPtr;      // Pointer to the PhaseMgr object
    switch (Window)
    {
        case eMandWindow:
            // Initialize pointer to the PhaseMgr object
//            phaseMgrPtr = PhaseMgr::S_GetInstance();

            // Make sure the ventilator is in the exhalation phase.
            phaseId = phaseMgrPtrGetCurrentPhaseId();
            ASSERTION(phaseId == eExhPhase);
            phaseMgrPtrProcessNewPhase(eMandInhPhase);
            break;

        case eSpontWindow:
            // If the breath period fires while the spontaneous window is open,
            // the ventilator just re-opens the mandatory window.
//            phaseMgrPtr = PhaseMgr::S_GetInstance();

            //Make sure the ventilator is in the exhalation phase.
            phaseId = phaseMgrPtrGetCurrentPhaseId();
            ASSERTION(phaseId == eExhPhase);
            if(phaseId == eExhPhase)
            {
                phaseMgrPtrProcessNewPhase(eMandInhPhase);
            }

            Window = eMandWindow;
            break;

        default:
            // Trigger fired in wrong window
            ASSERTION((Window == eSpontWindow) || (Window == eMandWindow));
            break;
    }

}   // end ProcessBpTimeTrigger()
void ApneaBackupMode::ProcessApneaAlarmTimeTrigger()
{
    //Do nothing
}

class ApneaBackupModeUnitMock : public BDTestFixture
{
public:
    ApneaBackupModeUnitMock(void) : BDTestFixture(new BaseModuleMock)
{
}
    virtual ~ApneaBackupModeUnitMock(void){}
    virtual void SetUp(void)
    {
        MyApneaBackupMode = new ApneaBackupMode();
    }
    virtual void TearDown(void)
    {
        delete MyApneaBackupMode;
    }
    ApneaBackupMode *MyApneaBackupMode;
};

//test forApneaBackupMode* ApneaBackupMode::S_GetInstance(void)
TEST_F(ApneaBackupModeUnitMock, S_GetInstance)
{
    MyApneaBackupMode->S_GetInstance();

    EXPECT_NE(MyApneaBackupMode->S_Instance, (void*)0);
}

//test for void ApneaBackupMode::ProcessTriggerID (E_TriggerId id)
TEST_F(ApneaBackupModeUnitMock, ProcessTriggerID)
{
    //all calling function is tested, not need to test
}

//test for ApneaBackupMode::ApneaBackupMode(void):MixedMode()
TEST_F(ApneaBackupModeUnitMock, ApneaBackupMode)
{
    EXPECT_EQ(MyApneaBackupMode->fromMode, eStandbyMode);
    EXPECT_EQ(MyApneaBackupMode->IsPtInhTrigger, false);
}

//test for void ApneaBackupMode::Enter(E_VentilationMode fromModeId)
TEST_F(ApneaBackupModeUnitMock, Enter_0)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaBackupMode->Enter(eHighPressMode);
}
TEST_F(ApneaBackupModeUnitMock, Enter_1)
{
    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaBackupMode->Enter(eHardwareErrorMode);
}
TEST_F(ApneaBackupModeUnitMock, Enter_2)
{
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableAllTriggers()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableIMVTrigger()).Times(1);
    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrDisableTrigger(eApneaAlarmTimeTrigger)).Times(1);

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrGetTimePeriod(eBpBackupTimeTrigger)).Times(1).WillOnce(Return(1));

    EXPECT_CALL(*_TriggerMgr, triggerMgrPtrEnableTriggerWith(eBpBackupTimeTrigger,1)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentVentType()).Times(1).WillOnce(Return(ePressureVentilation));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eActive));

    MyApneaBackupMode->Enter(ePcvACMode);

    EXPECT_EQ(MyApneaBackupMode->fromMode, ePcvACMode);
    EXPECT_EQ(MyApneaBackupMode->IsPtInhTrigger, false);
    EXPECT_EQ(MyApneaBackupMode->Window, eMandWindow);
}

//test for void ApneaBackupMode::Exit()
TEST_F(ApneaBackupModeUnitMock, Exit)
{
    EXPECT_CALL(*_Alarm, BDAlarmTaskSetIsActive(eApnea, eNotActive)).Times(1);

    EXPECT_CALL(*_BreathMgr, MgrBreathDataInitializeBreathDataPtr()).Times(1);

    EXPECT_CALL(*_StepMotor, BDRequestDevice(eCMVServoControllerDisable)).Times(1);
    EXPECT_CALL(*_StepMotor, BDRequestDevice(eVCVServoControllerDisable)).Times(1);

    MyApneaBackupMode->Exit();
}

//test for void ApneaBackupMode::ProcessPtInhTrigger(void)
TEST_F(ApneaBackupModeUnitMock, ProcessPtInhTrigger_0)
{
    MyApneaBackupMode->Window = eMandWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyApneaBackupMode->ProcessPtInhTrigger();

    EXPECT_EQ(MyApneaBackupMode->IsPtInhTrigger, true);
    EXPECT_EQ(MyApneaBackupMode->Window, eSpontWindow);
}
TEST_F(ApneaBackupModeUnitMock, ProcessPtInhTrigger_1)
{
    MyApneaBackupMode->Window = eMandWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyApneaBackupMode->ProcessPtInhTrigger();

    EXPECT_EQ(MyApneaBackupMode->IsPtInhTrigger, true);
}

//test for void ApneaBackupMode::ProcessAlarmResetTrigger()
TEST_F(ApneaBackupModeUnitMock, ProcessAlarmResetTrigger)
{
    EXPECT_CALL(*_MgrBdSetting, MgrBreathDataSetFirstBreath()).Times(1);

    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaBackupMode->ProcessAlarmResetTrigger();
}

//test for void ApneaBackupMode::ProcessManualInhOpReqTrigger()
TEST_F(ApneaBackupModeUnitMock, ProcessManualInhOpReqTrigger)
{
    EXPECT_CALL(*_TriggerMgr, TriggerMgrReEnableTrigger(eBpBackupTimeTrigger)).Times(1);

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyApneaBackupMode->ProcessManualInhOpReqTrigger();

    EXPECT_EQ(MyApneaBackupMode->Window, eMandWindow);
}

//test for void ApneaBackupMode::ProcessMandPressInhTimeTrigger()
TEST_F(ApneaBackupModeUnitMock, ProcessMandPressInhTimeTrigger_0)
{
    MyApneaBackupMode->IsPtInhTrigger = true;

    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaBackupMode->ProcessMandPressInhTimeTrigger();
}
TEST_F(ApneaBackupModeUnitMock, ProcessMandPressInhTimeTrigger_1)
{
    MyApneaBackupMode->IsPtInhTrigger = false;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eExhPhase)).Times(1);

    MyApneaBackupMode->ProcessMandPressInhTimeTrigger();
}

//test for void ApneaBackupMode::ProcessVcvInhTimeTrigger()
TEST_F(ApneaBackupModeUnitMock, ProcessVcvInhTimeTrigger_0)
{
    MyApneaBackupMode->IsPtInhTrigger = true;

    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaBackupMode->ProcessVcvInhTimeTrigger();
}
TEST_F(ApneaBackupModeUnitMock, ProcessVcvInhTimeTrigger_1)
{
    MyApneaBackupMode->IsPtInhTrigger = false;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(ePlateauPhase)).Times(1);

    MyApneaBackupMode->ProcessVcvInhTimeTrigger();
}

//test for void ApneaBackupMode::ProcessPlateauTimeTrigger()
TEST_F(ApneaBackupModeUnitMock, ProcessPlateauTimeTrigger_0)
{
    MyApneaBackupMode->IsPtInhTrigger = true;

    EXPECT_CALL(*_ModeMgr, ModeMgrGoToOperatorSetMode()).Times(1);

    MyApneaBackupMode->ProcessPlateauTimeTrigger();
}
TEST_F(ApneaBackupModeUnitMock, ProcessPlateauTimeTrigger_1)
{
    MyApneaBackupMode->IsPtInhTrigger = false;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eExhPhase)).Times(1);

    MyApneaBackupMode->ProcessPlateauTimeTrigger();
}

//test for void ApneaBackupMode::ProcessBpTimeTrigger(void)
TEST_F(ApneaBackupModeUnitMock, ProcessBpTimeTrigger_0)
{
    MyApneaBackupMode->Window = eMandWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1);
    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyApneaBackupMode->ProcessBpTimeTrigger();
}
TEST_F(ApneaBackupModeUnitMock, ProcessBpTimeTrigger_1)
{
    MyApneaBackupMode->Window = eSpontWindow;

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrGetCurrentPhaseId()).Times(1).WillOnce(Return(eExhPhase));

    EXPECT_CALL(*_PhaseMgrPtr, phaseMgrPtrProcessNewPhase(eMandInhPhase)).Times(1);

    MyApneaBackupMode->ProcessBpTimeTrigger();
}

//test for void ApneaBackupMode::ProcessApneaAlarmTimeTrigger()
TEST_F(ApneaBackupModeUnitMock, ProcessApneaAlarmTimeTrigger)
{
    //no source, not need to test
}

#endif //end define BD_MODE_UNIT_TEST
