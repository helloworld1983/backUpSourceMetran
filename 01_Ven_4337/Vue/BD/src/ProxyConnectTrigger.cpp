/******************************************************************************/
//$COMMON.H$
//   File Name:  ProxyConnectTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      It is a repository for ProxyConnectTrigger class
/******************************************************************************/
#include <DeviceInterface.h>
#include "ProxyConnectTrigger.h"
#include "ProxyFlowSensor.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name:ProxyConnectTrigger::ProxyConnectTrigger():Trigger(eProxyConnectTrigger)
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
ProxyConnectTrigger::ProxyConnectTrigger(void):Trigger(eProxyConnectTrigger)
{
}
/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: CheckCondition()
//
//    Processing:
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      The input command must be Start.
//
//    Requirements:
//
/******************************************************************************/
E_Bool ProxyConnectTrigger::CheckCondition()
{
    ProxyStatus Status = ProxyFlowSensor::S_GetInstance()->getLastStatus();

    if((Status ==eCalibrated)||(Status==eResetAndRecovered))
    {
        TriggerDidFire = eTrue;
    }
    else
    {
        TriggerDidFire = eFalse;
    }

    return TriggerDidFire;
}
