//******************************************************************************/
//$COMMON.CPP$
//   File Name:  Fault.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: Fault Handling class. The Fault class provides capability
//            to report various fatal software, hardware faults and exception cases.
// 			  The class also provides capability log system events to the NovRam memory
//			  which is not a fault condition. The class allows logging of external
//			  hardware failures and conditions for diagnostic purpose.
//	
//		      It logs a reported fault information to system's NOVRAM memory,
//			  these fault messages can reviewed through the MORPH service mode
//			  GUI interface.
//
//			  When a system type failure occurs, the system resets after logging
//			  a system failure log message.
//
//   Interfaces: All the subsystems in the Morph machine calls this class to
//				report a fatal error condition.
//
//   Restrictions: None
//
//   Revision History:
//      Rev:  Date:     Engineer:           Project:
//      03    02/23/98  R.P. Rush           Morph
//      Description: SCR-149. Added Code review comments
//
//      Rev:  Date:   	Engineer:     		Project:
//      02    05/07/97  Rush    			Morph
//      Description: Change the /n to /n/r to clean up the display of the 
//				     messages.
//      Rev:  Date:   	Engineer:     		Project:
//      01    02/14/97   Jung Verheiden    	Morph
//      Description: Initial Revision
//
/******************************************************************************/

#define FAULT_CPP

#include "Fault.h"
#include "CPUUART.h"
#include "UTIL.h"
#include "errorhan.h"
#include <stdio.h>
//******************************************************************************/
//    Operation Name: S_SoftwareFailure(subSystemId, fileId, pFilname, lineNum,
//						pExpr, classId )
//
//    Processing: Prints out error base using inputs into function 
//
//    Input Parameters: None
//
//    Output Parameters: None
//
//    Return Values: None
//
//    Pre-Conditions: None
//
//    Miscellaneous: None
//
//    Requirements: None  
//
//******************************************************************************/
void Fault::S_SoftFailure(const CHAR *pFilename, const ULONG lineNum,
                          const CHAR *pExpr, E_ConditionId conditionId)
{
    char buffer[60];
    CHAR lineString[30];

#if !defined(PRODUCTION_BUILD)
#if !defined(V851_SIM)

    CpuUart  *port = CpuUart::S_GetInstance();

    port->PutString((UCHAR *) "\n\r\n\r");
	switch(conditionId)
	{
		case eClassPreCondition:
			port->PutString((UCHAR *) "PRE_CONDITION");
			break;

		case eClassAssert:
			port->PutString((UCHAR *) "ASSERT");
			break;

		case eClassPostCondition:
			port->PutString((UCHAR *) "POST_CONDITION");
			break;

		default:
			port->PutString((UCHAR *) "UNKNOWN-CONDITION");
    }
	((UCHAR *) "\n");
    port->PutString((UCHAR *) " FAILURE(");
    port->PutString((UCHAR *)pExpr);
    port->PutString((UCHAR *) ") {\n\r");

    port->PutString((UCHAR *) "   File:         ");
    port->PutString((UCHAR *) pFilename );
    port->PutString((UCHAR *) "\n\r");

	ltoa(lineNum, &lineString[0], 10); // Convert a line number to an ascii string.
    port->PutString((UCHAR *) "   Line:         ");
    port->PutString((UCHAR *)lineString);

    port->PutString( (UCHAR *) "\n\r};\n\r");
#endif //!defined(V851_SIM)
#endif //!defined(PRODUCTION_BUILD)

    // Log the event and reset the system.
    ltoa(lineNum, &lineString[0], 10); // Convert a line number to an ascii string.
    sprintf(buffer,"ASSERT: %s, FILE %s, Line: %s",
            pExpr, pFilename, lineString); 
    ErrorHandler::S_GetInstance()->HandleRecoverable(buffer);
}




