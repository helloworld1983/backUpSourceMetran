/******************************************************************************/
//$COMMON.CPP$
//   File Name:  Phase.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: 
//      This file contains the Phase class definition.  The Phase
//      class is defined as an abstract base class.  It is only defined so
//      that breath phases (VCV Inh, PCV Inh, Plateau, Flow Trigger based Exh,
//      etc.) can be handled similarly by the PhaseMgr object.  An empty default 
//      constructor is defined (instead of relying on the one provided by
//      the compiler) simply to resolve a compiler error that results because
//      the copy constructor is declared.
//
//   Interfaces:
//      None
//
//   Restrictions:
//      There are no instances of this class.
/******************************************************************************/
#include <Phase.h>

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: Phase()
//
//    Processing: 
//      This operation is the constructor for objects of the Phase class.
//      It performs no work.
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
Phase::Phase (void)
{

}   // end Phase()

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ~Phase()
//
//    Processing:
//      This operation is the destructor for objects of the Phase class.
//      It performs no work.
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
Phase::~Phase ()
{
}

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: ~Phase()
//
//    Processing:
//      This operation is the copy constructor for objects of the Phase class.
//      It performs no work.
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
Phase::Phase (const Phase&)
{
}
