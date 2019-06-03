//$COMMON.H$
//    File Name:  SetupWelcome.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: CSetupWelcome
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  08/06/98 	Erik Blume			Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  SETUPWELCOME_H
#define	 SETUPWELCOME_H



// SetupWelcome.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupWelcome dialog

class CInstaller;

class CSetupWelcome 
: public CPropertyPage
{
// Construction
public:
	CSetupWelcome(CPropertySheet* psetsht, CInstaller* pinstlr);
	~CSetupWelcome();

// Dialog Data
	//CBitmapCtrl	m_bmcSetup;
	CStatic m_bmcSetup;
	//{{AFX_DATA(CSetupWelcome)
	enum { IDD = IDD_WELCOME };
	CStatic	m_stWelcome;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSetupWelcome)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertySheet* m_psetsht;
	CInstaller* m_pinstlr;
	CFont m_fontBold;

protected:
	// Generated message map functions
	//{{AFX_MSG(CSetupWelcome)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

#endif
