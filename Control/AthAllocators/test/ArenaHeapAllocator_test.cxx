/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ArenaHeapAllocator_test.cxx 470529 2011-11-24 23:54:22Z ssnyder $
/**
 * @file AthAllocators/test/ArenaHeapAllocator_test.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2007
 * @brief Regression tests for ArenaHeapAllocator.
 */

#undef NDEBUG
#include "AthAllocators/ArenaHeapAllocator.h"
#include "AthAllocators/ArenaBlock.h"
#include "CxxUtils/checker_macros.h"
#include <vector>
#include <cassert>
#include <algorithm>
#include <iostream>


//==========================================================================

std::atomic<int> nclear;
struct Payload
{
  Payload();
  ~Payload();
  Payload& operator= (const Payload&) = default;
  void clear();

  int x;
  int y;
  static std::atomic<int> n;
  static std::vector<int> v ATLAS_THREAD_SAFE;
};

Payload::Payload()
{
  x = n++;
  y = 0;
  v.push_back (x);
}

Payload::~Payload()
{
  v.push_back (-x);
}

void Payload::clear ()
{
  y = 0;
  ++nclear;
}

std::atomic<int> Payload::n;
std::vector<int> Payload::v;

//==========================================================================

void test1()
{
  std::cout << "test1\n";
  SG::ArenaHeapAllocator aha
    (SG::ArenaHeapAllocator::initParams<Payload, true>(100, "foo"));
  assert (aha.name() == "foo");
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse == 0);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == 0);
  size_t elt_size = aha.params().eltSize;
  size_t block_ov = SG::ArenaBlock::overhead();

  int nptr = 987;
  std::vector<Payload*> ptrs;
  for (int i=0; i < nptr; i++) {
    Payload* p = reinterpret_cast<Payload*> (aha.allocate());
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  assert  (Payload::v.size() == 1000);
  for (int i=0; i < 1000; ++i) {
    assert (Payload::v[i] == i);
  }
  assert (aha.stats().elts.inuse == 987);
  assert (aha.stats().elts.free == 13);
  assert (aha.stats().elts.total == 1000);
  assert (aha.stats().blocks.inuse == 10);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total == 10);

  for (size_t i = 0; i < ptrs.size(); i += 2)
    aha.free ((char*)ptrs[i]);
  assert (aha.stats().elts.inuse == 493);
  assert (aha.stats().elts.free == 507);
  assert (aha.stats().elts.total == 1000);
  assert (aha.stats().blocks.inuse == 10);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total == 10);
  for (size_t i = 0; i < ptrs.size(); i += 2)
    assert (ptrs[i]->y == 0);
  for (size_t i = 0; i < 300; i++)
    ptrs.push_back (reinterpret_cast<Payload*>(aha.allocate()));
  //printf ("%d %d %d\n", aha.stats().elts.inuse, aha.stats().elts.free, aha.stats().elts.total);
  assert (aha.stats().elts.inuse == 793);
  assert (aha.stats().elts.free == 207);
  assert (aha.stats().elts.total == 1000);
  assert (aha.stats().blocks.inuse == 10);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total == 10);

  assert (aha.stats().bytes.inuse ==  (793 * elt_size + 10 * block_ov));
  assert (aha.stats().bytes.free  ==  207 * elt_size);
  assert (aha.stats().bytes.total == (1000 * elt_size + 10 * block_ov));

  aha.reset();
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 1000);
  assert (aha.stats().elts.total == 1000);
  assert (aha.stats().blocks.inuse ==  0);
  assert (aha.stats().blocks.free  == 10);
  assert (aha.stats().blocks.total == 10);

  ptrs.clear();
  for (size_t i = 0; i < 300; i++)
    ptrs.push_back (reinterpret_cast<Payload*>(aha.allocate()));
  assert (aha.stats().elts.inuse == 300);
  assert (aha.stats().elts.free == 700);
  assert (aha.stats().elts.total == 1000);
  assert (aha.stats().blocks.inuse ==  3);
  assert (aha.stats().blocks.free  ==  7);
  assert (aha.stats().blocks.total == 10);

  aha.reserve (550);
  assert (aha.stats().elts.inuse == 300);
  assert (aha.stats().elts.free == 300);
  assert (aha.stats().elts.total == 600);
  assert (aha.stats().blocks.inuse ==  3);
  assert (aha.stats().blocks.free  ==  3);
  assert (aha.stats().blocks.total ==  6);

  aha.reserve (1000);
  //printf ("%d %d %d\n", aha.stats().elts.inuse, aha.stats().elts.free, aha.stats().elts.total);
  assert (aha.stats().elts.inuse == 300);
  assert (aha.stats().elts.free == 700);
  assert (aha.stats().elts.total == 1000);
  assert (aha.stats().blocks.inuse ==  3);
  assert (aha.stats().blocks.free  ==  4);
  assert (aha.stats().blocks.total ==  7);

  Payload::v.clear();
  aha.erase();
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse ==  0);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total ==  0);
  assert (Payload::v.size() == 1000);
  std::sort (Payload::v.begin(), Payload::v.end());
  for (size_t i = 0; i < 700; i++) {
    assert (Payload::v[i] == (int)i-1399);
    //printf ("%d %d\n", Payload::v[i], i);
  }
  for (size_t i = 700; i < Payload::v.size(); i++) {
    assert (Payload::v[i] == (int)i-999);
  }
}


void test2()
{
  std::cout << "test2\n";
  SG::ArenaHeapAllocator::Params params = 
    SG::ArenaHeapAllocator::initParams<Payload, true>(100);
  params.mustClear = true;
  params.canReclear = false;
  SG::ArenaHeapAllocator aha (params);
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse ==  0);
  assert (aha.stats().blocks.free  ==  0);
  assert (aha.stats().blocks.total ==  0);

  nclear = 0;
  std::vector<Payload*> ptrs;
  for (int i=0; i < 987; i++) {
    Payload* p = reinterpret_cast<Payload*> (aha.allocate());
    ptrs.push_back (p);
    p->y = 2*p->x;
  }
  for (size_t i = 0; i < ptrs.size(); i+=2) {
    aha.free ((char*)ptrs[i]);
    assert (ptrs[i]->y == 0);
    ptrs[i]->y = 1;
  }
  aha.reset();
  assert (nclear == 987);
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 1000);
  assert (aha.stats().elts.total == 1000);
  assert (aha.stats().blocks.inuse ==  0);
  assert (aha.stats().blocks.free  == 10);
  assert (aha.stats().blocks.total == 10);

  for (size_t i = 0; i < ptrs.size(); i++) {
    if ((i&1) != 0)
      assert (ptrs[i]->y == 0);
    else
      assert (ptrs[i]->y == 1);
  }
}


void test3()
{
  std::cout << "test3\n";

  Payload::v.clear();
  SG::ArenaHeapAllocator aha
    (SG::ArenaHeapAllocator::initParams<Payload, true>(100, "bar"));
  for (int i=0; i < 150; i++) {
    aha.allocate();
  }

  assert (aha.name() == "bar");
  assert (aha.params().name == "bar");
  assert (aha.stats().elts.inuse == 150);
  assert (aha.stats().elts.free == 50);
  assert (aha.stats().elts.total == 200);
  assert (aha.stats().blocks.inuse == 2);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == 2);

  SG::ArenaHeapAllocator aha2 (std::move (aha));
  assert (aha.name() == "bar");
  assert (aha.params().name == "bar");
  assert (aha2.name() == "bar");
  assert (aha2.params().name == "bar");
  assert (aha2.stats().elts.inuse == 150);
  assert (aha2.stats().elts.free == 50);
  assert (aha2.stats().elts.total == 200);
  assert (aha2.stats().blocks.inuse == 2);
  assert (aha2.stats().blocks.free  == 0);
  assert (aha2.stats().blocks.total == 2);
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse == 0);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == 0);

  aha = std::move (aha2);
  assert (aha.name() == "bar");
  assert (aha.params().name == "bar");
  assert (aha2.name() == "bar");
  assert (aha2.params().name == "bar");
  assert (aha.stats().elts.inuse == 150);
  assert (aha.stats().elts.free == 50);
  assert (aha.stats().elts.total == 200);
  assert (aha.stats().blocks.inuse == 2);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == 2);
  assert (aha2.stats().elts.inuse == 0);
  assert (aha2.stats().elts.free == 0);
  assert (aha2.stats().elts.total == 0);
  assert (aha2.stats().blocks.inuse == 0);
  assert (aha2.stats().blocks.free  == 0);
  assert (aha2.stats().blocks.total == 0);

  aha.swap (aha2);
  assert (aha.name() == "bar");
  assert (aha.params().name == "bar");
  assert (aha2.name() == "bar");
  assert (aha2.params().name == "bar");
  assert (aha2.stats().elts.inuse == 150);
  assert (aha2.stats().elts.free == 50);
  assert (aha2.stats().elts.total == 200);
  assert (aha2.stats().blocks.inuse == 2);
  assert (aha2.stats().blocks.free  == 0);
  assert (aha2.stats().blocks.total == 2);
  assert (aha.stats().elts.inuse == 0);
  assert (aha.stats().elts.free == 0);
  assert (aha.stats().elts.total == 0);
  assert (aha.stats().blocks.inuse == 0);
  assert (aha.stats().blocks.free  == 0);
  assert (aha.stats().blocks.total == 0);
}


int main()
{
  test1();
  test2();
  test3();
  return 0;
}
