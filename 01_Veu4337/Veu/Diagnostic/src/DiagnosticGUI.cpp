//$COMMON.CPP$
//   File Name:  DiagnosticGUI.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Provides the interface from GUI back to Diagnostic. This class
//      receives communications from the the GUI by inheriting from the 
//      GUIClient pure virtual base class. This class is responsible for 
//      providing the implementation to for each on of the pure virtual 
//      methods as to handle the GUI communications. 
//
//   Interfaces:
//
//   Restrictions: Communications from the GUI are executing within the 
//      GUI's task. Therefore, as to not hold off the GUI's task for too long, 
//      when a communication is received we need to do something quick. 
//      Typically posting an event to the Diagnostic's task to be handled
//      later.
/******************************************************************************/

#include "DiagnosticGUI.h"
//#include "GUI.h"
#include "Diagnostic.h"
#include "SelfTest.h"
//#include "HardwareDiagnostic.h"
#include "DiagnosticTask.h" 

//*****************************************************************************/
//    Operation Name: TheDiagnosticGUI()
//
//    Processing: Return a reference to a local static instance of this class. 
//      All access to this class should be made through this function. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//      DiagnosticGUI& - a ref to the only instance of this class. 
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
DiagnosticGUI& TheDiagnosticGUI(void)
{
    static DiagnosticGUI diagnosticGUI;
    return diagnosticGUI;
}

//*****************************************************************************/
//    Operation Name: DiagnosticGUI()
//
//    Processing: Class constructor.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
DiagnosticGUI::DiagnosticGUI()
{
    // register diagnostics with GUI
//    GUI::S_Register(this);
}

//*****************************************************************************/
//    Operation Name: StartSST()
//
//    Processing: Start the SST test Sequence. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void DiagnosticGUI::StartSST(void) 
{
    // post message to the DiagnosticTask
    TheDiagnosticTask().Send(eStartSSTEvent);
}

//*****************************************************************************/
//    Operation Name: StartEST()
//
//    Processing: Start the EST test Sequence. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void DiagnosticGUI::StartEST(void) 
{
    // post message to the DiagnosticTask
    TheDiagnosticTask().Send(eStartESTEvent);
}

//*****************************************************************************/
//    Operation Name: CancelTest()
//
//    Processing: Cancels testing for the active self test.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void DiagnosticGUI::CancelTest(void) 
{ 
    // post message to the DiagnosticTask
    TheDiagnosticTask().SendToFront(eCancelTestEvent);
    
    // set the cancel test event
    TheDiagnosticTask().SetCancelEvent();
}

//*****************************************************************************/
//    Operation Name: Response()
//
//    Processing: Indicates the client received a event from the user. 
//
//      When we need a response from the client, we send an event to 
//      the client indicating what we need. Once the client gets the required 
//      response, it calls us back. 
//
//    Input Parameters:
//      pData - pointer to response data.
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void DiagnosticGUI::Response(E_ResponseType response)
{ 
    // load the response into a test response structure
    SelfTest::ResponseData* pData = new SelfTest::ResponseData;
    pData->response = response;

    // post message to the DiagnosticTask
    TheDiagnosticTask().Send(eResponseEvent, pData);
}

//*****************************************************************************/
//    Operation Name: ContextSwitch()
//
//    Processing: The user switched screens diagnostic screens. 
//
//    Input Parameters:
//      context - the new screen context. 
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void DiagnosticGUI::ContextSwitch(E_Context context)
{
    // load the response into a test response structure
//    HardwareDiagnostic::ContextData* pData = new HardwareDiagnostic::ContextData;
//    pData->context = context;

    // post message to the DiagnosticTask
//    TheDiagnosticTask().Send(eContextSwitchEvent, pData);
}

//*****************************************************************************/
//    Operation Name: SettingChanged()
//
//    Processing: A setting has changed from with GUI and Diagnostic is being
//      notified of that change.
//
//    Input Parameters:
//      value - the new setting value.
//      event - the GUI event tied to that setting notification.
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void DiagnosticGUI::SettingChanged(E_SettingChanged setting, LONG value)
{
    // load the setting into a response structure
//    HardwareDiagnostic::SettingChangedData* pData =
//        new HardwareDiagnostic::SettingChangedData;
//    pData->setting = setting;
//    pData->value = value;

    // post message to the DiagnosticTask
//    TheDiagnosticTask().Send(eSettingChangedEvent, pData);
}

//*****************************************************************************/
//    Operation Name: OutputChange()
//
//    Processing: A solenoid position change has been detected from the user
//      via the GUI.
//
//    Input Parameters:
//      component - the component ID for the solenoid button pressed. 
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
void DiagnosticGUI::OutputChange(E_ComponentID component, E_Bool state)
{
    // load the setting into a response structure
//    HardwareDiagnostic::OutputChangeData* pData =
//        new HardwareDiagnostic::OutputChangeData;
//    pData->component = component;
//    pData->state = state;

    // post message to the DiagnosticTask
//    TheDiagnosticTask().Send(eOutputChangeEvent, pData);
}

//*****************************************************************************/
//    Operation Name: IsTesting()
//
//    Processing: Checks if a self test (e.g. SST or EST) is in progress. 
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values:
//      E_Bool - true if testing, false otherwise. 
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
E_Bool DiagnosticGUI::IsTesting(void)
{
//    return TheDiagnostic().IsTesting();
    return eFalse;
}



