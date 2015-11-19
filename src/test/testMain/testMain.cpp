
#include <stdio.h>
#include "gtest/gtest.h"
#include "easylogging/easylogging++.h"

INITIALIZE_EASYLOGGINGPP

GTEST_API_ int main(int argc, char **argv) {
  el::Configurations conf("log_test.conf");
  el::Loggers::reconfigureAllLoggers(conf);
  printf("Running main()\n");
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
