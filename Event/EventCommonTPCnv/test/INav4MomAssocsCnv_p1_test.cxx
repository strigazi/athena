/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
/*
 */
/**
 * @file EventCommonTPCnv/test/INav4MomAssocsCnv_p1_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date Nov, 2018
 * @brief Regression tests.
 */

#undef NDEBUG
#include "EventCommonTPCnv/INav4MomAssocsCnv_p1.h"
#include "NavFourMom/INav4MomAssocs.h"
#include "NavFourMom/Nav4MomWrapper.h"
#include "FourMom/P4PxPyPzE.h"
#include "TestTools/leakcheck.h"
#include "TestTools/initGaudi.h"
#include "StoreGate/StoreGateSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ServiceHandle.h"
#include <cassert>
#include <iostream>


class Part
  : virtual public INavigable4Momentum,
    public P4PxPyPzE,
    public NavigableTerminalNode
{
public:
};



void compare (const INav4MomAssocs& a1,
              const INav4MomAssocs& a2)
{
  assert (a1.getAssocStores() == a2.getAssocStores());

  assert (a1.size() == a2.size());
  INav4MomAssocs::object_iterator i1 = a1.beginObject();
  INav4MomAssocs::object_iterator i2 = a2.beginObject();
  for (size_t i = 0; i < a1.size(); ++i, ++i1, ++i2) {
    assert (i1.getObjectLink() == i2.getObjectLink());

    assert (i1.getNumberOfAssociations() == i2.getNumberOfAssociations());
    INav4MomAssocs::asso_iterator j1 = i1.second();
    INav4MomAssocs::asso_iterator j2 = i2.second();
    for (size_t j = 0; j < i1.getNumberOfAssociations(); ++j, ++j1, ++j2) {
      assert (j1.getLink() == j2.getLink());
    }
  }
}


void testit (const INav4MomAssocs& trans1, StoreGateSvc* sg)
{
  MsgStream log (0, "test");
  INav4MomAssocsCnv_p1 cnv (sg);
  INav4MomAssocs_p1 pers;
  cnv.transToPers (&trans1, &pers, log);
  INav4MomAssocs trans2;
  cnv.persToTrans (&pers, &trans2, log);
  compare (trans1, trans2);
}


void test1()
{
  std::cout << "test1\n";

  ServiceHandle<StoreGateSvc> sg ("StoreGateSvc", "test");

  INavigable4MomentumCollection* in4mc = new INavigable4MomentumCollection;
  for (int i=0; i < 10; i++)
    in4mc->push_back (CxxUtils::make_unique<Part>());
  assert (sg->record (in4mc, "in").isSuccess());

  INavigable4MomentumCollection* xn4mc = new INavigable4MomentumCollection;
  for (int i=0; i < 10; i++)
    xn4mc->push_back (CxxUtils::make_unique<Part>());
  assert (sg->record (xn4mc, "xn").isSuccess());

  // Get proxies created.
  ElementLink<INavigable4MomentumCollection> dum1 ("in", 3);
  ElementLink<INavigable4MomentumCollection> dum2 ("xn", 2);

  Athena_test::Leakcheck check;

  INav4MomAssocs trans1;
  trans1.addAssociation (ElementLink<INavigable4MomentumCollection> ("in", 3),
                         ElementLink<INavigable4MomentumCollection> ("xn", 2));
  trans1.addAssociation (ElementLink<INavigable4MomentumCollection> ("in", 6),
                         ElementLink<INavigable4MomentumCollection> ("xn", 4));

  testit (trans1, &*sg);
}


int main()
{
  std::cout << "INav4MomAssocsCnv_p1_test\n";

  ISvcLocator* pSvcLoc;
  if (!Athena_test::initGaudi("EventCommonTPCnv_test.txt", pSvcLoc)) {
    std::cerr << "This test can not be run" << std::endl;
    return 0;
  }

  test1();
  return 0;
}
