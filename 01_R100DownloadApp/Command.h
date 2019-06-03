//******************************************************************************/
//$COMMON.H$
//   File Name:  Command.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: Defines a set of Download command strings that are used between
//       the Download Server on a PC and a Morph download program to establish
//       a download protocol.
//
//
//   Revision History:
//      Rev:  Date:   	Engineer:     		Project:
//      01    03/03/97    Jung Verheiden   			  Morph
//      Description: Initial Revision
//
//******************************************************************************/

#ifndef	COMMAND_H
#define COMMAND_H

#include "standard.h"

class Command  
{
	public:
		inline static CHAR *S_GetRequest();
		inline static CHAR *S_GetReply();
		inline static CHAR *S_GetInfoRequest();
		inline static CHAR *S_GetInfoSend();
		static void  S_InitCommandSizes();
		inline static USHORT S_GetRequestLen();
		inline static USHORT S_GetReplyLen();
		inline static USHORT S_GetInfoRequestLen();
		inline static USHORT S_GetInfoSendLen();

	private:
		static CHAR *S_Request ;
		static CHAR *S_Reply ;
		static CHAR *S_InfoRequest ;
		static CHAR *S_InfoSend ;
		static USHORT S_RequestLen;
		static USHORT S_ReplyLen;
		static USHORT S_InfoRequestLen;
		static USHORT S_InfoSendLen;
};

//******************************************************************************/
//    Operation Name: GetRequest()
//
//    Processing:  Get a download request command string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: a string pointer to the download request command .
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
inline CHAR *Command::S_GetRequest()
{
	return S_Request;
}
//******************************************************************************/
//    Operation Name: S_GetReply()
//
//    Processing:  Get a download reply command string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: a pointer to the download reply command.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
inline CHAR *Command::S_GetReply()
{
	return S_Reply;
}
//******************************************************************************/
//    Operation Name: S_GetInfoRequest()
//
//    Processing:  Get a Ventilator information request command string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: a pointer to the Ventilator information request command.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
inline CHAR *Command::S_GetInfoRequest()
{
	return S_InfoRequest;
}
//******************************************************************************/
//    Operation Name: S_GetInfoSend()
//
//    Processing:  Get a Ventilator information send command string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: a pointer to the Ventilator information send command.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
inline CHAR *Command::S_GetInfoSend()
{
	return S_InfoSend;
}

//******************************************************************************/
//    Operation Name: S_GetRequestLen()
//
//    Processing:  Get the length of the Download Request command string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: length of the Download Request command string in bytes.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
inline USHORT Command::S_GetRequestLen()
{
	DEBUG_PRE_CONDITION(S_RequestLen);

	return(S_RequestLen);
}

//******************************************************************************/
//    Operation Name: S_GetReplyLen()
//
//    Processing:  Get the length of the Download reply command string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: length of the Download reply command string in bytes.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
inline USHORT Command::S_GetReplyLen()
{
	DEBUG_PRE_CONDITION(S_ReplyLen);

	return(S_ReplyLen);
}

//******************************************************************************/
//    Operation Name: S_GetInfoRequestLen()
//
//    Processing: Get the length of the Download Information request command string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: length of the Information Request command string in bytes.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
inline USHORT Command::S_GetInfoRequestLen()
{
	DEBUG_PRE_CONDITION(S_InfoRequestLen);

	return(S_InfoRequestLen);
}

//******************************************************************************/
//    Operation Name: S_GetInfoSendLen()
//
//    Processing:  Get the length of the Ventilator information send command
//				   string.
//
//    Input Parameters:
//
//    Output Parameters:
//
//    Return Values: a length of the Ventilator information send command string
//					 in bytes.
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//******************************************************************************/
inline USHORT Command::S_GetInfoSendLen()
{
	DEBUG_PRE_CONDITION(S_InfoSendLen);

	return(S_InfoSendLen);
}
#endif //COMMAND_H
