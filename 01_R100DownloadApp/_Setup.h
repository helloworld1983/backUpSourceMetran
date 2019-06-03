//$COMMON.H$
//    File Name:  _Setup.h
//    Copyright 1996 InVentive Technologies, Inc.  All Rights Reserved.
//
//    Class: CSetupApp
//		The main application which initiates downloading of software to
//      the Esprit ventilator
//
//    Revision History:
//
//      Rev:  Date: 	Engineer: 			Project:
//      01 	  07/24/98 	Erik Blume  		Morph
//      Description: Initial Revision
//
/******************************************************************************/
#ifndef  _SETUP_H
#define	 _SETUP_H


#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#ifndef ALL_WARNINGS
#pragma warning(disable: 4355)  // 'this' : used in base member initializer list
#endif //!ALL_WARNINGS

#include "SetupUtilities.h"

class CSetupApp : public CWinApp
{
public:
	CSetupApp();

public:
	enum EXITCODE
	{
		EC_SUCCESS = EXIT_SUCCESS,
		EC_FAILURE = EXIT_FAILURE,
		EC_CANCELED
	};
	EXITCODE m_fExitCode;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InteractiveSetup();

	//{{AFX_MSG(CSetupApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


//*****************************************************************************/
//    Operation Name: MapResourceID()
//
//    Processing: 
//
//    	Given the base resource ID, this function returns the platform dependent
//	    resource id.
//    Input Parameters:	NONE
//
//    Output Parameters: NONE
//
//    Return Values:  NONE
//
//    Pre-Conditions: NONE
//
//    Miscellaneous:  
//    	This function should only be called when there are platform dependent
//	    resources to choose from. Do not call for platform independent resources.
//    	Currently, Win16 resources are 1 greater than Win32 resources.    
//
//
//    Requirements:  N/A
//
//*****************************************************************************/

inline UINT MapResourceID(UINT idResource)
{
	if(GetOSVersionInfo().dwPlatformId == VER_PLATFORM_WIN32s)
		return idResource + 1;


	return idResource;
}

#endif


