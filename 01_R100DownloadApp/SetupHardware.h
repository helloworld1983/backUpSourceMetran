//$COMMON.H$
//    File Name:  SetupHardware.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: CSetupHardware
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  08/06/98 	Erik Blume			Morph
//      Description: Initial Revision
//
/******************************************************************************/


#if !defined(AFX_SETUPHARDWARE_H__21481456_152A_11D2_B7C2_00104B0A7386__INCLUDED_)
#define AFX_SETUPHARDWARE_H__21481456_152A_11D2_B7C2_00104B0A7386__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SetupHardware.h : header file
//
class CInstaller;

/////////////////////////////////////////////////////////////////////////////
// CSetupHardware dialog

class CSetupHardware : public CPropertyPage
{

// Construction
public:
	CSetupHardware(CPropertySheet* psetsht, CInstaller* pinstlr);
	~CSetupHardware();
	void SetStatus(LPCTSTR lpszMessage);

// Dialog Data
	//CBitmapCtrl	m_bmcSetup;
	CStatic m_bmcSetup;
	//{{AFX_DATA(CSetupHardware)
	enum { IDD = IDD_SETUP_HARDWARE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSetupHardware)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	virtual LRESULT OnWizardBack();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CPropertySheet* m_psetsht;
	CInstaller* m_pinstlr;

	// Generated message map functions
	//{{AFX_MSG(CSetupHardware)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPHARDWARE_H__21481456_152A_11D2_B7C2_00104B0A7386__INCLUDED_)
