#ifndef INC_MODEMGRMOCKHEADER_H_
#define INC_MODEMGRMOCKHEADER_H_

void ModeMgrGoToAlarmMode(E_VentilationMode Mode);
void ModeMgrGoToOperatorSetMode(void);

void modeMgrPtrReturnToLastMode(void);
void modeMgrPtProcessAutoReset(void);

void ModeMgrReturnToLastMode(void);

void modeMgrPtrProcessAutoReset(void);

E_VentilationMode BDModeMgrGetCurrentModeId(void);

E_Bool ModeS_IsInhPtTriggered(void);

void ModeMgrProcessManualReset(void);

E_WindowType MixedModeGetWindow(void);

void HighPressModeSetCauseOfAlarm(E_GUIEventID);

void modeProcessTriggerID(E_TriggerId);

#endif /* INC_MODEMGRMOCKHEADER_H_ */
