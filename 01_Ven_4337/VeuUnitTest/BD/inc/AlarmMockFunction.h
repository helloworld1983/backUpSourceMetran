#ifndef INC_ALARMMOCKFUNCTION_H_
#define INC_ALARMMOCKFUNCTION_H_

#include "gmock.h"

class AlarmMockFunction
{
public :
    virtual ~AlarmMockFunction(){};

    MOCK_METHOD2(BDAlarmTaskSetIsActive, SHORT(E_GUIEventID, E_AlarmStatus));
    MOCK_METHOD1(BDAlarmTaskGetIsActive, E_AlarmStatus(E_GUIEventID));

    MOCK_METHOD0(O2SettingO2SourceFault, E_Bool(void));

    MOCK_METHOD0(alarmResetTriggerPtrGetIsArmed, E_Bool(void));
    MOCK_METHOD0(apneaArTriggerPtrGetState, E_AutoResetState(void));

    MOCK_METHOD1(BDAlarmTaskGetLevel, E_AlarmLevel(E_GUIEventID));

    MOCK_METHOD1(BDAlarmTaskSetResetFlag, void(E_Bool));
    MOCK_METHOD1(AlarmResetOpReqTriggerSetIsIgnoreAlarmReset, void(E_Bool));

    MOCK_METHOD0(BDAlarmTaskClearAllAlarm, void(void));
    MOCK_METHOD0(BDAlarmTaskClearHFOAlarm, void(void));
};




#endif /* INC_ALARMMOCKFUNCTION_H_ */
