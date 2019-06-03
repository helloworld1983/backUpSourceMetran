//$COMMON.CPP$
//   File Name:  SetupHardware.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:  dialog which allows the user to select the COM port
//      before proceeding with the download
//
//   Interfaces:
//
//   Restrictions:
//
//   Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      01    08/06/98  Erik Blume          Morph
//      Description: Initial Revision
//
/******************************************************************************/
#using <System.dll>

#include "stdafx.h"
#include "_setup.h"
#include "SetupHardware.h"
#include "Installer.h"
#include <stdlib.h>  
#include <string.h>  
#include <msclr\marshal.h> 

using namespace msclr::interop;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupHardware property page


CSetupHardware::CSetupHardware(CPropertySheet* psetsht, CInstaller* pinstlr) : CPropertyPage(CSetupHardware::IDD)
{
    SetHelpID(CSetupHardware::IDD);
    ASSERT_VALID(psetsht);
    m_psetsht = psetsht;
    ASSERT(AfxIsValidAddress(pinstlr, sizeof(CInstaller)));
    m_pinstlr = pinstlr;
    //{{AFX_DATA_INIT(CSetupHardware)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

CSetupHardware::~CSetupHardware()
{
}

void CSetupHardware::SetStatus(LPCTSTR lpszMessage)
{
  ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                  // been created. Can be called from OnInitDialog
  CWnd *pWndStatus = GetDlgItem(IDC_HARDWARE_STATUS);
  
  // Verify that the static text control exists
  ASSERT(pWndStatus!=NULL);
  pWndStatus->SetWindowText(lpszMessage);

}

void CSetupHardware::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSetupHardware)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupHardware, CPropertyPage)
    //{{AFX_MSG_MAP(CSetupHardware)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupHardware message handlers

 BOOL CSetupHardware::OnSetActive() 
{
    m_psetsht->SetWizardButtons(PSWIZB_BACK | PSWIZB_FINISH);
    
    return CPropertyPage::OnSetActive();
}

BOOL CSetupHardware::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
	CString defaultPort;
	int selection;
	
	
    CComboBox *pCombo =(CComboBox *) GetDlgItem(IDC_COMBO_COMMPORTS);
	//select the most recently used port
	pCombo->SetCurSel(0);

	//initialize array of String to save a list of serial port names
	array <System::String^>^ serialPorts = nullptr;
	// Get a list of serial port names.
	serialPorts = System::IO::Ports::SerialPort::GetPortNames();
	
	for each(System::String^ port in serialPorts)
	{
		marshal_context^ context = gcnew marshal_context();
		const char* result;
		//Convert String^ to LPCSTR(Const *char)
		result = context->marshal_as<const char*>(port);

		//add to combo box
		pCombo->AddString(result);
		delete context;
	}

    defaultPort = m_pinstlr->GetDefaultPortName();
    selection = pCombo->SelectString(0,defaultPort);
	
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CSetupHardware::OnWizardFinish() 
{
    CComboBox *pCombo =(CComboBox *) GetDlgItem(IDC_COMBO_COMMPORTS);
    int curSel = pCombo->GetCurSel();
    CString portName;
    pCombo->GetLBText(curSel,portName);
    m_pinstlr->SetCommPortName(portName);
    // now see if the installer can get a sync character
    //   if not, allow the user to select another COM port
    CWaitCursor wait;
    CString status;
    status.LoadString(IDS_VERIFYING_CONNECTION);
    SetStatus(status); //".....verifying connection with ventilator");
	UINT errorString = m_pinstlr->VerifyCommConnection();
    if (errorString == IDS_SUCCESSFUL_CONNECTION)
    {
        return CPropertyPage::OnWizardFinish();
    }
	else if (errorString == IDS_ERR_HANDSHAKE)
	{
		CString message;
		message.LoadString(errorString);
		AfxMessageBox(message);
		SetStatus("");
		return 0;
	}
	else
    {
        CString message;
        message.LoadString(errorString);
        AfxMessageBox(message);
        SetStatus("");
        return 0;  //don't destroy the property sheet
    }
}

LRESULT CSetupHardware::OnWizardBack() 
{

    // close the download file; it will be reopened again when the
    //    set language dialog responds to OnWizardNext
    m_pinstlr->CloseDownloadFile();
    return CPropertyPage::OnWizardBack();
}
