//$COMMON.CPP$
//   File Name:	 SetupUtilities.cpp
//   Copyright 1996 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//
//   Interfaces:
//
//   Restrictions:
//
//   Revision History:
//
//   	Rev:  Date:		Engineer:			Project:
//		01	  11/03/98	eblume				Morph
//	    Description: Initial Revision
//
/******************************************************************************/

#include "SetupUtilities.h"

OSVERSIONINFO GetOSVersionInfo()
{
	BOOL success;
	OSVERSIONINFO OsVersionInfo;
	OsVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    success = GetVersionEx(&OsVersionInfo);
	if (!success)
	{
		OsVersionInfo.dwMajorVersion = 0;
		OsVersionInfo.dwMinorVersion = 0;
		OsVersionInfo.dwPlatformId = 0;
	}
	return OsVersionInfo;
}
