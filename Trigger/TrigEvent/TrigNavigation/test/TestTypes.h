/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#pragma once

#include "AthContainers/AuxElement.h"
#include "xAODCore/AuxContainerBase.h"

#define TRIGSTORAGEDEF_MERGEPACKS
#include "TrigNavigation/TypeRegistration.h"

HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(struct TestA, struct TestA, struct DataVector<TestA>, struct TestAuxA)
  HLT_REGISTER_TYPE(struct TestB, struct TestBContainer, struct TestBContainer, struct TestAuxB)
  HLT_REGISTER_TYPE(struct TestC, struct TestC, struct DataVector<TestC>)
  HLT_REGISTER_TYPE(struct TestD, struct TestDContainer, struct TestDContainer)
HLT_END_TYPE_REGISTRATION(TrigNavigationUnitTest)

#define LTYPEMAPCLASS(name)\
  struct class_##name{\
    typedef TypeInfo_##name map;\
    static const std::string package_name;\
  };

LTYPEMAPCLASS(TrigNavigationUnitTest)

struct TypeInfo_EDM {
  typedef  HLT::TypeInformation::newlist
  ::add<class_TrigNavigationUnitTest>  ::go
  ::done map;
};
REGISTER_PACKAGE_WITH_NAVI(TrigNavigationUnitTest)

class TestA : public SG::AuxElement {
public:
  TestA(){a = 0;}
  TestA(int arg){a = arg;}
  ~TestA() { std::cerr << " deleteing TestA: "<< a << std::endl; }
  int value() const { return a; }
  int a;
};


CLASS_DEF(TestA, 6421, 1)
CLASS_DEF(DataVector<TestA>, 64210, 1)
typedef DataVector<TestA> TestAContainer;


class TestAuxA : public xAOD::AuxContainerBase {
public:
  std::vector<float> detail;
};
DATAVECTOR_BASE(TestAuxA,xAOD::AuxContainerBase);
CLASS_DEF(TestAuxA, 642300, 1)


class TestB : public SG::AuxElement{
public:
  TestB() : b(0) {  }
  TestB(int v) : b(v) {  }
  ~TestB() { std::cerr << " deleteing TestB: "<< b << std::endl; }
  int value() const { return b; }
  int b;
};


class TestBContainer : public DataVector<TestB> {
public:
  TestBContainer(SG::OwnershipPolicy policy = SG::OWN_ELEMENTS)
    : DataVector<TestB>(policy) { }
}; 

CLASS_DEF(TestBContainer, 96422, 1)



class TestAuxB : public xAOD::AuxContainerBase {
public:
  std::vector<float> detail;
};
CLASS_DEF(TestAuxB, 642311, 1)
DATAVECTOR_BASE(TestAuxB,xAOD::AuxContainerBase);
//HLT::AuxInit<TestAuxB> t;


class TestC {
public:
  TestC() :c(0) {}
  TestC(int v) : c(v) {}
  ~TestC()  {std::cerr << "Deleting TestC" << std::endl; }
  int value() const { return c; }
  int c;
};

CLASS_DEF(TestC, 7800, 1)
CLASS_DEF(DataVector<TestC>, 78001, 1)

class TestCtypeContainer : public DataVector<TestC> { };
CLASS_DEF(TestCtypeContainer, 78002, 1)

class TestD {
public:
  TestD() : b(0) {  }
  TestD(int v) : b(v) {  }
  ~TestD() { std::cerr << " deleteing TestD: "<< b << std::endl; }
  int value() const { return b; }
  int b;
};


class TestDContainer : public DataVector<TestD> {
public:

}; 

CLASS_DEF(TestDContainer, 96477, 1)

