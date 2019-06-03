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
//#include "TriggerMgr.h"

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

#endif // PROXYDISCONNECTTRIGGER_H
