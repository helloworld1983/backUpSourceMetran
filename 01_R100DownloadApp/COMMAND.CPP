//******************************************************************************/
//$COMMON.CPP$
//   File Name:  Command.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose:  Defines a set of Download command strings that are used between
//       the Download Server on a PC and a Morph download program to
//		 establish a download protocol.
//
//   Interfaces: This class gets never instantiated, but provides a few static
//			attributes that defines Download Commands.
//
//
//   Restrictions:
//
//   Revision History:
//      Rev:  Date:   	Engineer:     		Project:
//      01    02/05/97   Jung Verheiden    			  Morph
//      Description: Initial Revision
//
//******************************************************************************/

#include "Command.h"
#include "string.h"

//Initialize the static attributes
CHAR *Command::S_Request = "MP";
CHAR *Command::S_Reply = "MR";
CHAR *Command::S_InfoRequest = "SR";
CHAR *Command::S_InfoSend = "SV";

USHORT Command::S_RequestLen = 0;
USHORT Command::S_ReplyLen = 0;
USHORT Command::S_InfoRequestLen = 0;
USHORT Command::S_InfoSendLen = 0;

//******************************************************************************/
//    Operation Name: S_InitCommandSizes()
//
//    Processing:  Initializes class static variables which contains sizes of
//         each corresponding command string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: a pointer to the download reply command.
//
//    Pre-Conditions:
//
//    Miscellaneous: By keeping the size of a command string in a variable
//					 eliminates to call 'strlen' routine each time when size
//					 information is needed.
//
//    Requirements:  
//
//******************************************************************************/
void Command::S_InitCommandSizes()
{
	S_RequestLen = strlen(S_Request);
	S_ReplyLen  = strlen(S_Reply);
	S_InfoRequestLen  = strlen(S_InfoRequest);
	S_InfoSendLen  = strlen(S_InfoSend);
}

