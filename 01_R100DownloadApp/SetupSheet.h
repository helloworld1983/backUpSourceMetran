//$COMMON.H$
//    File Name:  SetupSheet.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: 
//
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  08/06/98 	blumee				Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  SETUPSHEET_H
#define	 SETUPSHEET_H

/////////////////////////////////////////////////////////////////////////////
// CSetupSheet

class CInstaller;

class CSetupSheet 
: public CPropertySheet
{
	DECLARE_DYNAMIC(CSetupSheet)

// Construction
public:
	CSetupSheet(CInstaller* pinstlr, CWnd* pParentWnd = NULL);

// Attributes
public:
	HICON m_hIcon;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupSheet)
	public:
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSetupSheet();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSetupSheet)
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif