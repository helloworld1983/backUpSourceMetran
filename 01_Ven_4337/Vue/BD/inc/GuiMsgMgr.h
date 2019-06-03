/******************************************************************************/
//$COMMON.H$
//   File Name:  GuiMsgMgr.h
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Class: 
//      The Message Manager defines a communication interface between the BD
//      task and GUI Task.
/******************************************************************************/
#ifndef GUIMSGMGR_H
#define GUIMSGMGR_H

#include "STANDARD.H"
#include "DebugTrace.h"

/******************************************************************************/
//      CONSTANT DEFINITIONS
/******************************************************************************/
const SHORT MAX_NUM_MSGS = 5;

/******************************************************************************/
//      TYPE DEFINITIONS
/******************************************************************************/
struct MsgData
{
//    GUIEvent GuiEvent;
        SHORT NumTries;
};

/******************************************************************************/
//      CLASS DECLARATIONS
/******************************************************************************/
class GuiMsgMgr
{
    public:
        static GuiMsgMgr* S_GetInstance (void);
        void ProcessBdCycle (void);
//        void SendMsg(E_GUIEventID mnvrId, E_MnvrStatus mnvrStatus, LONG data = 0);

    protected:

    private:
        GuiMsgMgr (void);
        GuiMsgMgr (const GuiMsgMgr&);

        //$COMMON.ATTRIBUTE$
        //  Name: S_Instance
        //  Description:  Pointer to the single instance of this class.  
        //  Units: None
        //  Range: n/a
        static GuiMsgMgr* S_Instance;

        //$COMMON.ATTRIBUTE$
        //  Name: MsgList
        //  Description:   Array of Messsges
        //  Units: None
        //  Range: n/a
        MsgData MsgList[MAX_NUM_MSGS];

        //$COMMON.ATTRIBUTE$
        //  Name: FirstMsgIx
        //  Description:   Index of first message in MsgList
        //  Units: None
        //  Range: n/a
        SHORT FirstMsgIx;

        //$COMMON.ATTRIBUTE$
        //  Name: NextMsgIx
        //  Description:   Index of place to put next message in MsgList
        //  Units: None
        //  Range: n/a
        SHORT NextMsgIx;

        //$COMMON.ATTRIBUTE$
        //  Name: IsListFull
        //  Description:   Indicates if the MsgList is full.
        //  Units: None
        //  Range: n/a
        E_Bool IsListFull;

};

/******************************************************************************/
//      INLINE OPERATIONS
/******************************************************************************/

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: S_GetInstance()
//
//    Processing: 
//      Since only 1 instance of this object should be created, any access
//      to the object comes via this operation.  If any other object wants 
//      access to the GuiMsgMgr object, it invokes GuiMsgMgr::S_GetInstance(),
//      which returns a pointer to the GuiMsgMgr object.
//
//      If the object has not already been created, this method instantiates 
//      it and returns a pointer to the object.  If the object has already been 
//      instantiated, a pointer to the object is returned.
//
//    Input Parameters:
//      None
//
//    Output Parameters:
//      None
//
//    Return Values:
//      GuiMsgMgr* - pointer to the object instantiated from this class
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
inline GuiMsgMgr* GuiMsgMgr::S_GetInstance (void)
{
    // If the object has not already been created, create it.
    if(NULL == S_Instance)
    {
        S_Instance = new GuiMsgMgr ();
        ASSERTION(S_Instance != NULL);
    }

    return (S_Instance);

}   // end S_GetInstance()

#endif  // GuiMsgMgr_H
