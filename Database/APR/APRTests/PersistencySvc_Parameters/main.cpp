/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include <iostream>
#include <stdexcept>
#include "TestDriver.h"

int main( int, char** ) {
  try {    
    std::cout << "[OVAL] Creating the test driver." << std::endl;
    pool::TestDriver driver;

    std::cout << "[OVAL] Writing parameters in the database." << std::endl;
    driver.write();
    std::cout << "[OVAL] ...done" << std::endl;

    std::cout << "[OVAL] Reading the parameters back from the database." << std::endl;
    driver.read();
    std::cout << "[OVAL] ...done" << std::endl;
  }
  catch ( std::exception& e ) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  std::cout << "[OVAL] Exiting..." << std::endl;
  return 0;
}
