/******************************************************************************/
//$COMMON.H$
//   File Name:  ProxyConnectTrigger.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      It is a repository for ProxyConnectTrigger class
/******************************************************************************/
#ifndef PROXYCONNECTTRIGGER_H
#define PROXYCONNECTTRIGGER_H

#include "Trigger.h"
#include "TriggerMgr.h"

class ProxyConnectTrigger : public Trigger
{
public:
    //Constructor
    ProxyConnectTrigger(void);

    //Check condition and process action
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
inline void ProxyConnectTrigger::ProcessTrigger(Mode* mode)
{
    DisableTrigger();
    TriggerMgr::S_GetInstance()->EnableTrigger(eProxyDisconnectTrigger);
    mode->ProcessTriggerID(eProxyConnectTrigger);
}

#endif // PROXYCONNECTTRIGGER_H
