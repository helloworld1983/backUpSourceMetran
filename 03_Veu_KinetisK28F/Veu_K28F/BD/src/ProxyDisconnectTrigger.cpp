/******************************************************************************/
//$COMMON.H$
//   File Name:  ProxyDisconnectTrigger.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class:
//      It is a repository for ProxyDisconnectTrigger class
/******************************************************************************/
#include <DeviceInterface.h>
#include "ProxyDisconnectTrigger.h"
#include "ProxyFlowSensor.h"

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProxyDisconnectTrigger::ProxyDisconnectTrigger():Trigger(eProxyDisconnectTrigger)
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
ProxyDisconnectTrigger::ProxyDisconnectTrigger():Trigger(eProxyDisconnectTrigger)
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
E_Bool ProxyDisconnectTrigger::CheckCondition()
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
