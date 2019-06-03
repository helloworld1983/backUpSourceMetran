//$COMMON.CPP$
//   File Name:  SetupSheet.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: PropertySheet which provides the framework for the Setup wizard.
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
#include "SetupSheet.h"
#include "SetupWelcome.h"
#include "SetupLang.h"
#include "SetupHardware.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupSheet

IMPLEMENT_DYNAMIC(CSetupSheet, CPropertySheet)

CSetupSheet::CSetupSheet(CInstaller* pinstlr, CWnd* pParentWnd)
    : CPropertySheet(IDS_SETUPAPPLICATIONTITLE, pParentWnd)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    //pinstlr->SetInstallType(RUNTIME_CLASS(CTypicalInstall));

    AddPage(new CSetupWelcome(this, pinstlr));
    AddPage(new CSetupLang(this, pinstlr));
    AddPage(new CSetupHardware(this,pinstlr));

    SetWizardMode();
}

CSetupSheet::~CSetupSheet()
{
    while(GetPageCount() > 0)
    {
        CPropertyPage* ppage = GetPage(0);
        RemovePage(ppage);
        delete ppage;
    }
}

BEGIN_MESSAGE_MAP(CSetupSheet, CPropertySheet)
    //{{AFX_MSG_MAP(CSetupSheet)
    ON_WM_QUERYDRAGICON()
    ON_WM_PAINT()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupSheet message handlers

BOOL CSetupSheet::OnInitDialog() 
{
    // Calling SetWizardButtons here causes a GPF

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);         // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon

    CWnd* pwndParent = CWnd::GetSafeOwner();
    if(pwndParent)
        pwndParent->EnableWindow();

    return CPropertySheet::OnInitDialog();
}

void CSetupSheet::OnPaint() 
{
    if(IsIconic())
    {
        CPaintDC dc(this);

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CPropertySheet::OnPaint();
    }
}

HCURSOR CSetupSheet::OnQueryDragIcon() 
{
    return (HCURSOR)m_hIcon;
}

