//$COMMON.CPP$
//   File Name:  SetupWelcome.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Initial dialog of the Setup wizard
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

#include "stdafx.h"
#include "_Setup.h"
#include "Installer.h"
#include "SetupWelcome.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupWelcome property page

CSetupWelcome::CSetupWelcome(CPropertySheet* psetsht, CInstaller* pinstlr) 
: CPropertyPage(MapResourceID(CSetupWelcome::IDD))
{
    ASSERT_VALID(psetsht);
    m_psetsht = psetsht;
    ASSERT_POINTER(pinstlr, CInstaller);
    m_pinstlr = pinstlr;

    SetHelpID(CSetupWelcome::IDD);

    //{{AFX_DATA_INIT(CSetupWelcome)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

CSetupWelcome::~CSetupWelcome()
{
}

void CSetupWelcome::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSetupWelcome)
    DDX_Control(pDX, IDC_WELCOME, m_stWelcome);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupWelcome, CPropertyPage)
    //{{AFX_MSG_MAP(CSetupWelcome)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupWelcome message handlers

BOOL CSetupWelcome::OnSetActive() 
{
    m_psetsht->SetWizardButtons(PSWIZB_NEXT);
    
    return CPropertyPage::OnSetActive();
}

BOOL CSetupWelcome::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
    
    CFont* pfontNormal = m_stWelcome.GetFont();
    LOGFONT lf;
    if(pfontNormal->GetLogFont(&lf))
    {
        lf.lfWeight = 1000;
        if(m_fontBold.CreateFontIndirect(&lf))
        {
            m_stWelcome.SetFont(&m_fontBold);
        }
    }

    CString strAppTitle; 
    VERIFY(strAppTitle.LoadString(IDS_INSTALLEDAPPLICATIONTITLE));
    CString strSetupTitle; 
    VERIFY(strSetupTitle.LoadString(IDS_SETUPAPPLICATIONTITLE));
    CString strFormat;
    CString strWelcome;

    GetDlgItemText(IDC_WELCOME, strFormat);
    strWelcome.Format(strFormat, strAppTitle);
    SetDlgItemText(IDC_WELCOME, strWelcome);

    GetDlgItemText(IDC_WELCOME1, strFormat);
    strWelcome.Format(strFormat, strSetupTitle);
    SetDlgItemText(IDC_WELCOME1, strWelcome);

    GetDlgItemText(IDC_WELCOME2, strFormat);
    strWelcome.Format(strFormat, strSetupTitle);
    SetDlgItemText(IDC_WELCOME2, strWelcome);

    
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
