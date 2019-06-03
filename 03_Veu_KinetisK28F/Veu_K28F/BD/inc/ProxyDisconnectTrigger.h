/******************************************************************************/
//$COMMON.H$
//   File Name:  ProxyDisconnectTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      It is a repository for ProxyDisconnectTrigger class
/******************************************************************************/
#ifndef PROXYDISCONNECTTRIGGER_H
#define PROXYDISCONNECTTRIGGER_H
#include "Trigger.h"
#include "TriggerMgr.h"

class ProxyDisconnectTrigger : public Trigger
{
public:
    //Constructor
    ProxyDisconnectTrigger(void);

    ////Check condition and process action
    virtual E_Bool CheckCondition(void);

    //Process action base on trigger ID
    virtual inline void ProcessTrigger(Mode* mode);
};

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessTrigger(Mode* mode)
//
//    Processing: Process action for each trigger ID
//
//    Input Parameters:
//      E_TriggerId id
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//
//    Requirements:
//
/******************************************************************************/
inline void ProxyDisconnectTrigger::ProcessTrigger(Mode* mode)
{
    DisableTrigger();
    TriggerMgr::S_GetInstance()->EnableTrigger(eProxyConnectTrigger);
    mode->ProcessTriggerID(eProxyDisconnectTrigger);

}
#endif // PROXYDISCONNECTTRIGGER_H
