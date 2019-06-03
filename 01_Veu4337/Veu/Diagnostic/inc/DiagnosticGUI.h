//*****************************************************************************/
//$COMMON.H$
//   File Name:  DiagnosticGUI.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: DiagnosticGUI
//      Provides the interface from GUI back to Diagnostic. 
//*****************************************************************************/

#ifndef DIAGNOSTICGUI_H
#define DIAGNOSTICGUI_H

#include "GUIClient.h"

class DiagnosticGUI : public GUIClient
{
public:
    DiagnosticGUI();
    ~DiagnosticGUI() { }

    // callbacks from GUI 
    void StartSST(void);
    void StartEST(void);
    void CancelTest(void);
    void Response(E_ResponseType response);
    void ContextSwitch(E_Context);
    void SettingChanged(E_SettingChanged, LONG);
    void OutputChange(E_ComponentID, E_Bool);
    E_Bool IsTesting(void);

protected:
private:
    // prevent copying
    DiagnosticGUI(const DiagnosticGUI&);
    void operator=(DiagnosticGUI&);
};

DiagnosticGUI& TheDiagnosticGUI(void);

#endif //DIAGNOSTICGUI_H

