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
#include "gmock.h"
//Include output print in console
#include <stdio.h>

//Main function for UnitTest
GTEST_API_ int main(int argc, char **argv) {
    // The following line must be executed to initialize Google Mock
    // (and Google Test) before running the tests.
  printf("Running main() from gtest_main.cc\n");

  testing::InitGoogleTest(&argc, argv);
  testing::InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}
