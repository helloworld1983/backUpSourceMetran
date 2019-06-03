//*****************************************************************************/
//$COMMON.H$
//   File Name:  GUIClient.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: GUIClient
//
//      A pure virtual base class used to communicate back with an anonymous
//      client. A client must inherit from this class, provide the 
//      implementation for each pure virtual method, and register the pointer
//      with GUI. When client communication is required, a call to 
//      one of these pure virtual method is made via the registered pointer.
//*****************************************************************************/

#ifndef GUICLIENT_H
#define GUICLIENT_H

//#include "GUIInterface.h"
#include "CommonTypes.h"
#include "ComponentIDs.h"

enum E_Context
{
    eSST_Context,
    eEST_Context,
    eHardwareContext,
    eSoftwareContext,
    eUserConfigContext,
    eInformationContext,
    eErrorCodesContext
};

enum E_SettingChanged
{
    // diagnostic setting IDs
    eAirValveSettingID,
    eOxygenValveSettingID,
    eExhalationValveSettingID,
    eMonitorsSettingID,
    eVoltageWrapSettingID,
    eBlowerSettingID,
    eMonthSettingID,
    eDaySettingID,
    eYearSettingID,
    eHourSettingID,
    eMinuteSettingID,
    eSecondSettingID,
    eAltitudeSettingID
};

class GUIClient 
{
public:
    GUIClient() { }
    ~GUIClient() { }

    // GUI callbacks to client
    virtual void StartSST(void) = 0;
    virtual void StartEST(void) = 0;
    virtual void CancelTest(void) = 0;
    virtual void Response(E_ResponseType) = 0;
    virtual void ContextSwitch(E_Context) = 0;
    virtual void SettingChanged(E_SettingChanged, LONG) = 0;
    virtual void OutputChange(E_ComponentID, E_Bool) = 0;
    virtual E_Bool IsTesting(void) = 0;

protected:
private:
    // prevent copying
    GUIClient(const GUIClient&);
    void operator=(GUIClient&);
};
#endif //GUICLIENT_H



