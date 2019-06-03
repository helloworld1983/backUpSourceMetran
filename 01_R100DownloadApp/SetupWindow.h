//$COMMON.H$
//    File Name:  SetupWindow.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: CSetupWindow
//
//
//    Revision History:
//
//      Rev:  Date:     Engineer:           Project:
//      01    08/06/98  Erik Blume          Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  SETUPWINDOW_H
#define  SETUPWINDOW_H


class CSetupWindow : public CWnd
{
// Construction
public:
    CSetupWindow();

    BOOL Create();

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CSetupWindow)
    //}}AFX_VIRTUAL

// Implementation
public:
    virtual ~CSetupWindow();

protected:

    // Generated message map functions
protected:
    //{{AFX_MSG(CSetupWindow)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif