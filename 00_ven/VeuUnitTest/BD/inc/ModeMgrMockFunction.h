#ifndef INC_MODEMGRMOCKFUNCTION_H_
#define INC_MODEMGRMOCKFUNCTION_H_

#include "gmock.h"

class ModeMgrMockFunction
{
public :
    virtual ~ModeMgrMockFunction(){};

    MOCK_METHOD1(ModeMgrGoToAlarmMode, void(E_VentilationMode));
    MOCK_METHOD0(ModeMgrGoToOperatorSetMode, void(void));
    MOCK_METHOD0(modeMgrPtrReturnToLastMode, void(void));
    MOCK_METHOD0(modeMgrPtProcessAutoReset, void(void));
    MOCK_METHOD0(ModeMgrReturnToLastMode, void(void));
    MOCK_METHOD0(modeMgrPtrProcessAutoReset, void(void));

    MOCK_METHOD0(BDModeMgrGetCurrentModeId, E_VentilationMode(void));

    MOCK_METHOD0(ModeS_IsInhPtTriggered, E_Bool(void));

    MOCK_METHOD0(ModeMgrProcessManualReset, void(void));

    MOCK_METHOD0(MixedModeGetWindow, E_WindowType(void));

    MOCK_METHOD1(HighPressModeSetCauseOfAlarm, void(E_GUIEventID));
    MOCK_METHOD1(modeProcessTriggerID, void(E_TriggerId));
};

#endif /* INC_MODEMGRMOCKFUNCTION_H_ */
