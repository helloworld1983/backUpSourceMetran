#ifndef INC_TRIGGERMGRMOCKHEADER_H_
#define INC_TRIGGERMGRMOCKHEADER_H_

#include "TriggerIds.h"

void triggerMgrPtrEnableTrigger(E_TriggerId Id);
void TriggerMgrEnableTrigger(E_TriggerId Id);

void triggerMgrPtrEnableTriggerWith(E_TriggerId Id, int i);

void triggerMgrPtrDisableTrigger(E_TriggerId Id);
void TriggerMgrDisableTrigger(E_TriggerId Id);

void TriggerMgrReEnableTrigger(E_TriggerId Id);

void ApneaAutoResetTriggerUpdateState(E_TriggerId Id);

UNSIGNED triggerMgrPtrGetTimePeriod(E_TriggerId Id);

void triggerMgrPtrDisableAllTriggers(void);

void triggerMgrPtrEnableIMVTrigger(void);

void TriggerMgrEnableExhaleTrigger(void);

void TriggerMgrEnableInhaleTrigger(void);

void ApneaAlarmTimeTriggerEnableTrigger(void);

void HipAlarmTriggerSetInactiveTimer(int);

void TriggerMgrDisableAllTriggers(void);

#endif /* INC_TRIGGERMGRMOCKHEADER_H_ */
