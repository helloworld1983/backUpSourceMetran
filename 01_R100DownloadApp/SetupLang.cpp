//$COMMON.CPP$
//   File Name:  SetupLang.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:  dialog which allows the user to select the language version
//      of the product that they wish to install
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
#include "_setup.h"
#include "SetupLang.h"
#include "Installer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupLang property page


CSetupLang::CSetupLang(CPropertySheet* psetsht, CInstaller* pinstlr) : CPropertyPage(CSetupLang::IDD)
{
    SetHelpID(CSetupLang::IDD);
    ASSERT_VALID(psetsht);
    m_psetsht = psetsht;
    ASSERT(AfxIsValidAddress(pinstlr, sizeof(CInstaller)));
    m_pinstlr = pinstlr;

    //{{AFX_DATA_INIT(CSetupLang)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}

CSetupLang::~CSetupLang()
{
}

void CSetupLang::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSetupLang)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupLang, CPropertyPage)
    //{{AFX_MSG_MAP(CSetupLang)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupLang message handlers

BOOL CSetupLang::OnSetActive() 
{
    m_psetsht->SetWizardButtons(PSWIZB_BACK | PSWIZB_NEXT);

    //find all of the language versions available in the current working
    //   directory

    return CPropertyPage::OnSetActive();
}

BOOL CSetupLang::OnInitDialog() 
{
    CPropertyPage::OnInitDialog();
    CString lang = m_pinstlr->GetFirstLanguage();
    LONG i = 0;
    CComboBox *pList =(CComboBox *) GetDlgItem(IDC_COMBO_LANGUAGES);
    i = pList->AddString(lang);
    while ( (lang = m_pinstlr->GetNextLanguage()) != "")
        i = pList->AddString(lang);

    pList->SetCurSel(0);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CSetupLang::OnWizardNext() 
{
    //get the selected language
    CComboBox *pCombo =(CComboBox *) GetDlgItem(IDC_COMBO_LANGUAGES);
    int curSel = pCombo->GetCurSel();
    CString langText;
    pCombo->GetLBText(curSel,langText);
    m_pinstlr->SetLanguage(langText);
    if (m_pinstlr->OpenDownloadFile() != eSuccess)
    {
        return (LRESULT) -1;
    }
    else
    {
        return CPropertyPage::OnWizardNext();
    }
    return -1;
}
