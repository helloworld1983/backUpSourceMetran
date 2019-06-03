#include "gtest.h"
#include "gmock.h"
#include <stdio.h>


GTEST_API_ int main(int argc, char **argv) {
    // The following line must be executed to initialize Google Mock
    // (and Google Test) before running the tests.
  printf("Running main() from gtest_main.cc\n");

  testing::InitGoogleTest(&argc, argv);
  testing::InitGoogleMock(&argc, argv);

  return RUN_ALL_TESTS();
}
