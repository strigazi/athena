/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file AthContainers/test/DataVector_e_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Sep, 2013
 * @brief Regression tests for DataVector
 *
 * The @c DataVector regression tests are split into several pieces,
 * in order to reduce the memory required for compilation.
 */

#undef NDEBUG


#include "DataVector_test.icc"


void test2_e()
{
  std::cout << "test2_e\n";
  do_test2<AAux, BAux> ();
}


int main()
{
  test2_e();
  return 0;
}
