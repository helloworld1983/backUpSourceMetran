; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CSetupWindow
LastTemplate=CPropertyPage
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "_setup.h"
LastPage=0

ClassCount=16
Class1=CSetupApp
Class2=CExitSuccess
Class3=CLicenseAgreement
Class4=CMaintenanceSheet
Class5=CMaintenanceType
Class6=CSetupDirectories
Class7=CSetupOptions
Class8=CSetupProgressDlg
Class9=CSetupSheet
Class10=CSetupType
Class11=CSetupWelcome
Class12=CSetupWindow
Class13=CUserInformation

ResourceCount=6
Resource1=IDD_WELCOME
Resource2=IDD_SETUP_LANGUAGE
Resource3=IDD_PROGRESS
Resource4=IDD_EXITSUCCESS
Class14=CSetupLanguage
Class15=CSetupLang
Class16=CSetupHardware
Resource5=IDD_SETUP_HARDWARE
Resource6=IDD_GET_SERIAL_NUMBER

[CLS:CSetupApp]
Type=0
BaseClass=CWinApp
HeaderFile=_Setup.h
ImplementationFile=_Setup.cpp

[CLS:CExitSuccess]
Type=0
BaseClass=CDialog
HeaderFile=ExitSuccess.h
ImplementationFile=ExitSuccess.cpp
LastObject=CExitSuccess
Filter=D
VirtualFilter=dWC

[CLS:CLicenseAgreement]
Type=0
BaseClass=CPropertyPage
HeaderFile=LicenseAgreement.h
ImplementationFile=LicenseAgreement.cpp
LastObject=CLicenseAgreement

[CLS:CMaintenanceSheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=MaintenanceSheet.h
ImplementationFile=MaintenanceSheet.cpp

[CLS:CMaintenanceType]
Type=0
BaseClass=CPropertyPage
HeaderFile=MaintenanceType.h
ImplementationFile=MaintenanceType.cpp

[CLS:CSetupDirectories]
Type=0
BaseClass=CPropertyPage
HeaderFile=SetupDirectories.h
ImplementationFile=SetupDirectories.cpp
LastObject=CSetupDirectories

[CLS:CSetupOptions]
Type=0
BaseClass=CPropertyPage
HeaderFile=SetupOptions.h
ImplementationFile=SetupOptions.cpp

[CLS:CSetupProgressDlg]
Type=0
BaseClass=CDialog
HeaderFile=SetupProgressDlg.h
ImplementationFile=SetupProgressDlg.cpp

[CLS:CSetupSheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=SetupSheet.h
ImplementationFile=SetupSheet.cpp

[CLS:CSetupType]
Type=0
BaseClass=CPropertyPage
HeaderFile=SetupType.h
ImplementationFile=SetupType.cpp

[CLS:CSetupWelcome]
Type=0
BaseClass=CPropertyPage
HeaderFile=SetupWelcome.h
ImplementationFile=SetupWelcome.cpp
LastObject=CSetupWelcome
Filter=D

[CLS:CSetupWindow]
Type=0
BaseClass=CWnd
HeaderFile=SetupWindow.h
ImplementationFile=SetupWindow.cpp
Filter=W
VirtualFilter=WC
LastObject=CSetupWindow

[CLS:CUserInformation]
Type=0
BaseClass=CPropertyPage
HeaderFile=UserInformation.h
ImplementationFile=UserInformation.cpp

[DLG:IDD_EXITSUCCESS]
Type=1
Class=CExitSuccess
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_SUCCEEDED,static,1342308480
Control3=IDC_MESSAGE,static,1342308353
Control4=IDOK,button,1342373889

[DLG:IDD_PROGRESS]
Type=1
Class=CSetupProgressDlg
ControlCount=4
Control1=IDCANCEL,button,1342242817
Control2=IDC_PROGDLG_PROGRESS,msctls_progress32,1350565888
Control3=IDC_PROGDLG_PERCENT,static,1342308353
Control4=IDC_PROGDLG_STATUS,static,1342308352

[DLG:IDD_WELCOME]
Type=1
Class=CSetupWelcome
ControlCount=7
Control1=IDC_STATIC,button,1342177287
Control2=IDC_STATIC,static,1342179331
Control3=IDC_WELCOME,static,1342308353
Control4=IDC_WELCOME1,static,1342308352
Control5=IDC_WELCOME2,static,1342308353
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342181902

[CLS:CSetupLanguage]
Type=0
HeaderFile=SetupLanguage.h
ImplementationFile=SetupLanguage.cpp
BaseClass=CDialog
Filter=D
LastObject=IDCANCEL

[DLG:IDD_SETUP_LANGUAGE]
Type=1
Class=CSetupLang
ControlCount=4
Control1=IDC_COMBO_LANGUAGES,combobox,1344339971
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,button,1342177287
Control4=IDC_STATIC,static,1342181902

[CLS:CSetupLang]
Type=0
HeaderFile=SetupLang.h
ImplementationFile=SetupLang.cpp
BaseClass=CPropertyPage
Filter=D
VirtualFilter=idWC
LastObject=CSetupLang

[DLG:IDD_SETUP_HARDWARE]
Type=1
Class=CSetupHardware
ControlCount=6
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1350569998
Control5=IDC_COMBO_COMMPORTS,combobox,1342242819
Control6=IDC_HARDWARE_STATUS,static,1342308864

[CLS:CSetupHardware]
Type=0
HeaderFile=SetupHardware.h
ImplementationFile=SetupHardware.cpp
BaseClass=CPropertyPage
Filter=D
LastObject=CSetupHardware
VirtualFilter=idWC

[DLG:IDD_GET_SERIAL_NUMBER]
Type=1
Class=?
ControlCount=5
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_SERIAL_NUMBER,edit,1350631552
Control4=IDC_STATIC,static,1342308353
Control5=IDC_SN_NOTE,static,1342308353

