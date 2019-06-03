//******************************************************************************//
//$COMMON.CPP$
//   File Name:  gTestMain.cpp
//   Copyright 1997 Inventive Technologies,Inc. All Rights Reserved.
//
//   Purpose: This file is used to run all Unit test of Vue project
//      Unit test is wrote base on
//          + The Google C++ Testing Framework ver 1.8.0
//          + GNU Make 4.2.1
//          + g++ (GCC) 6.4.0
//
//   Interfaces:
//
//   Restrictions:
//******************************************************************************
//Include Gtest frame work
#include "gtest.h"
//Include output print in console
#include <stdio.h>

/******************************************************************************/
//$COMMON.OPERATION$
//    Operation Name: main
//
//    Processing: Run all test case
//
//    Input Parameters: int argc
//                      char **argv
//
//    Output Parameters: None
//
//    Return Values: GTEST_API_
//
//    Pre-conditions: None
//
//    Miscellaneous: None
//
//    Requirements:
//
/******************************************************************************/
GTEST_API_ int main(int argc, char **argv)
{
    printf("Running main() from gTestMain\n");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
