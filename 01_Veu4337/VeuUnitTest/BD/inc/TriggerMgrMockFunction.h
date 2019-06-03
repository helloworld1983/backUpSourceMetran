#ifndef INC_TRIGGERMGRMOCKFUNCTION_H_
#define INC_TRIGGERMGRMOCKFUNCTION_H_

#include "gmock.h"


class TriggerMgrMockFunction
{
public :
    virtual ~TriggerMgrMockFunction(){};

    MOCK_METHOD1(triggerMgrPtrEnableTrigger, void(E_TriggerId));
    MOCK_METHOD1(TriggerMgrEnableTrigger, void(E_TriggerId));

    MOCK_METHOD2(triggerMgrPtrEnableTriggerWith, void(E_TriggerId, int));

    MOCK_METHOD1(triggerMgrPtrDisableTrigger, void(E_TriggerId));
    MOCK_METHOD1(TriggerMgrDisableTrigger, void(E_TriggerId));

    MOCK_METHOD1(ApneaAutoResetTriggerUpdateState, void(E_TriggerId));

    MOCK_METHOD1(TriggerMgrReEnableTrigger, void(E_TriggerId));

    MOCK_METHOD1(triggerMgrPtrGetTimePeriod, UNSIGNED(E_TriggerId));

    MOCK_METHOD0(triggerMgrPtrDisableAllTriggers, void(void));

    MOCK_METHOD0(triggerMgrPtrEnableIMVTrigger, void(void));

    MOCK_METHOD0(TriggerMgrEnableExhaleTrigger, void(void));

    MOCK_METHOD0(TriggerMgrEnableInhaleTrigger, void(void));

    MOCK_METHOD0(ApneaAlarmTimeTriggerEnableTrigger, void(void));

    MOCK_METHOD1(HipAlarmTriggerSetInactiveTimer, void(int));

    MOCK_METHOD0(TriggerMgrDisableAllTriggers, void(void));

};




#endif /* INC_TRIGGERMGRMOCKFUNCTION_H_ */
