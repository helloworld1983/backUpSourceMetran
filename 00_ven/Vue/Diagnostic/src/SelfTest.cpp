//$COMMON.CPP$
//   File Name:  SelfTest.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: A class for which all self tests are derived. Provides state
//      transition and event routing services. 
//
//   Interfaces:
//
//   Restrictions:
/******************************************************************************/

#include "SelfTest.h"
#include "TestServices.h"

SelfTest* SelfTest::S_ActiveTest;
TestServices* SelfTest::S_Serv;

//*****************************************************************************/
//    Operation Name: SelfTest()
//
//    Processing: Class constructor.
//
//    Input Parameters:
//      client - a pointer to the client test.
//
//    Output Parameters:
//
//    Return Values:
//
//    Pre-Conditions:
//
//    Miscellaneous:
//
//    Requirements:  
//
//*****************************************************************************/
SelfTest::SelfTest(SelfTest* parent, E_StringID id) :
    Parent(parent),
    testNameID(id)
{
    S_Serv = &TheTestServices();
}





