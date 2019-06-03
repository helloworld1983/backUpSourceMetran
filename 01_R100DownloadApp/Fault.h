//******************************************************************************/
//$COMMON.H$
//   File Name:  Fault.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Fault
//
//
//   Revision History:
//
//      Rev:  Date:       Engineer:             Project:
//      02    05/20/98    Dan Seibert	    Morph
//      Description: Added jump table vector for Fault::S_SoftFailure(). 
//          The jump table provides fixed addresses for FLASH routines 
//          called from ROM.
//
//      Rev:  Date:   	Engineer:     		Project:
//      01    02/14/97    Jung Verheiden   			  Morph
//      Description: Initial Revision
//
//******************************************************************************/

#ifndef	FAULT_H
#define FAULT_H
#include "standard.h"
#include "FMacro.h"

// Enums that define Software failure condition types.
class Fault 
{
	public:
			static void S_SoftFailure(const CHAR *pFilename, const ULONG lineNumber,
                const CHAR *pExpr, E_ConditionId cId);
			static void J_SoftFailure(const CHAR *pFilename, const ULONG lineNumber,
                const CHAR *pExpr, E_ConditionId cId);
	protected:
	private:
};

#endif //FAULT_H
