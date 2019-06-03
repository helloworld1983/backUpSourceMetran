//$COMMON.H$
//    File Name:  SetupLang.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: CSetupLang
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  08/06/98 	Erik Blume			Morph
//      Description: Initial Revision
//
/******************************************************************************/

#if !defined(AFX_SETUPLANG_H__21481455_152A_11D2_B7C2_00104B0A7386__INCLUDED_)
#define AFX_SETUPLANG_H__21481455_152A_11D2_B7C2_00104B0A7386__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SetupLang.h : header file
//
class CInstaller;

/////////////////////////////////////////////////////////////////////////////
// CSetupLang dialog

class CSetupLang : public CPropertyPage
{

// Construction
public:
	CSetupLang(CPropertySheet* psetsht, CInstaller* pinstlr);
	~CSetupLang();

// Dialog Data
	//CBitmapCtrl	m_bmcSetup;
	CStatic	m_bmcSetup;
	//{{AFX_DATA(CSetupLang)
	enum { IDD = IDD_SETUP_LANGUAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSetupLang)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertySheet* m_psetsht;
	CInstaller* m_pinstlr;

protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupLang)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPLANG_H__21481455_152A_11D2_B7C2_00104B0A7386__INCLUDED_)
