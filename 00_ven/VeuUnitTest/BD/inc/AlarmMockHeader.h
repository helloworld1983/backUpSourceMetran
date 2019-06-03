#ifndef INC_ALARMMOCKHEADER_H_
#define INC_ALARMMOCKHEADER_H_

SHORT BDAlarmTaskSetIsActive(E_GUIEventID Id, E_AlarmStatus Status);

E_AlarmStatus BDAlarmTaskGetIsActive(E_GUIEventID Id);

E_Bool O2SettingO2SourceFault(void);

E_Bool alarmResetTriggerPtrGetIsArmed(void);

E_AutoResetState apneaArTriggerPtrGetState(void);

E_AlarmLevel BDAlarmTaskGetLevel(E_GUIEventID);

void BDAlarmTaskSetResetFlag(E_Bool);

void AlarmResetOpReqTriggerSetIsIgnoreAlarmReset(E_Bool);

void BDAlarmTaskClearAllAlarm(void);
void BDAlarmTaskClearHFOAlarm(void);


#endif /* INC_ALARMMOCKHEADER_H_ */
