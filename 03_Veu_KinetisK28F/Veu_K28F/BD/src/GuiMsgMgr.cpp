/******************************************************************************/
//$COMMON.CPP$
//   File Name:  GuiMsgMgr.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This class contains operations that are used to interface with
//      the GUI task with regard to messages that the GUI must receive in
//      the order that the BD sent them.  For example, the status of the
//      exhalation hold maneuver is sent to the GUI task as the maneuver is
//      performed.
//
//   Interfaces:
//      Operations in this class interface with the GUI Task by invoking
//      operations to put messages on the GUI queue.
//
//   Restrictions:
//      There is only 1 instance of this class.  Also, operations in this
//      class are intended to only be invoked in a single thread of control 
//      (BD Task).
/******************************************************************************/
#include "GuiMsgMgr.h"

// define statics
GuiMsgMgr* GuiMsgMgr::S_Instance = NULL;

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ProcessBdCycle()
//
//    Processing: 
//      This operation is invoked each cycle of the BD Task.  It checks to 
//      see if there are any messages queued up to be sent to the GUI.  If
//      so, it attempts to send them.  When the BD Task tries to send a message
//      to the GUI, it does not block on the GUI queue.  Instead, it stores
//      the message in the MsgList.  
//
//      Each BD cycle, the BD Task will attempt to send to the GUI a message
//      from the list (if any exist).  If the attempt to send fails 
//      MAX_GUI_Q_ATTEMPTS times, the system is reset.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
void GuiMsgMgr::ProcessBdCycle (void)
{
//    STATUS status;          // status returned from GuiTask::Send()

// If FirstMsgIx is equal to NextMsgIx, then there are no messages that
// need to be sent.  The BD only tries to put 1 message on the GUI queue
// each time it runs.
//    if ((FirstMsgIx != NextMsgIx) || (eTrue == IsListFull))
//    {
//        // Try to put the message on the GUI queue.  Don't suspend.
//        status = (GUITask::S_GetInstance())->
//            Send((MnvrGUIEvent&) MsgList[FirstMsgIx].GuiEvent, NU_NO_SUSPEND);
//
//        // If the queue update was successful, remove the message from the
//        // list of messages to be sent.  Wrap around if necessary.
//        if (status == NU_SUCCESS)
//        {
//            FirstMsgIx++;
//            if (FirstMsgIx >= MAX_NUM_MSGS)
//            {
//                FirstMsgIx = 0;
//            }
//
//            // Since a message was just removed, the list can no longer be full.
//            IsListFull = eFalse;
//        }
//        else
//        {
//            // If more than MAX_GUI_Q_ATTEMPTS attempts to put
//            // on the queue have occurred and failed, invoke error handling.
//            MsgList[FirstMsgIx].NumTries++;
////            ASSERTION(MsgList[FirstMsgIx].NumTries <= MAX_GUI_Q_ATTEMPTS);
//        }
//
//    }

}   // end ProcessBdCycle()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: SendMsg()
//
//    Processing: 
//      This operation attemps to put a maneuver message on the GUI queue.
//      If the attempt fails, the message is added to the MsgList[] to be
//      sent later.  If the list is already full, the system is reset.
//
//    Input Parameters:
//      mnvrId - ID of the maneuver message
//      mnvrStatus - status of the maneuver
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
//void GuiMsgMgr::SendMsg(E_GUIEventID mnvrId, E_MnvrStatus mnvrStatus, LONG data)
//{
//    STATUS status;
//    MnvrGUIEvent event;
//
//    event.ID = mnvrId;
//    event.MnvrStatus = mnvrStatus;
//    event.Data = data;
//
//    // Try to put the message on the GUI queue.  Don't suspend.
//    status = (GUITask::S_GetInstance())->Send(event, NU_NO_SUSPEND);
//
//    // If the queue update was not successful, add it to the list of messages
//    // to be sent to the GUI.
//    if (status != NU_SUCCESS)
//    {
//        // If the list is full, assume the GUI task is not able to keep up with
//        // its queue; it's an error.
////        ASSERTION(IsListFull == eFalse);
//
//        // Copy message contents
//        MsgList[NextMsgIx].GuiEvent.MnvrEvent.ID = mnvrId;
//        MsgList[NextMsgIx].GuiEvent.MnvrEvent.MnvrStatus = mnvrStatus;
//        MsgList[NextMsgIx].GuiEvent.MnvrEvent.Data = data;
//
//        // Already tried to send it once.
//        MsgList[NextMsgIx].NumTries = 1;
//
//        // Bump the NextMsgIx; wrap around if necessary.
//        NextMsgIx++;
//        if (NextMsgIx >= MAX_NUM_MSGS)
//        {
//            NextMsgIx = 0;
//        }
//
//        // If the index has incremented to the top of the list, the list
//        // is now full.
//        if (NextMsgIx == FirstMsgIx)
//        {
//            IsListFull = eTrue;
//        }
//    }
//
//}   // end SendMsg()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: GuiMsgMgr()
//
//    Processing: 
//      This operation is the constructor for the GuiMsgMgr object.  This
//      operation initializes the attributes of the class.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      None
//
//    Pre-Conditions:
//      None
//
//    Miscellaneous:
//      None
//
//    Requirements:  
//
/******************************************************************************/
GuiMsgMgr::GuiMsgMgr (void) :
        FirstMsgIx (0), NextMsgIx (0), IsListFull (eFalse)

{

}   // end GuiMsgMgr()
