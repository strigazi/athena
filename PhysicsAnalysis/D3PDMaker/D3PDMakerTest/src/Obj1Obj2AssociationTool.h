// This file's extension implies that it's C, but it's really -*- C++ -*-.

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id$
/**
 * @file D3PDMakerTest/src/Obj1Obj2AssociationTool.h
 * @author scott snyder <snyder@bnl.gov>
 * @date Jan, 2010
 * @brief Test code: associate from an Obj1 to a single Obj2.
 */


#ifndef D3PDMAKERTEST_OBJ1OBJ2ASSOCIATIONTOOL_H
#define D3PDMAKERTEST_OBJ1OBJ2ASSOCIATIONTOOL_H


#include "D3PDMakerUtils/SingleAssociationTool.h"


namespace D3PDTest {


class Obj1;
class Obj2;


/**
 * @brief Test code: associate from an Obj1 to a single Obj2.
 */
class Obj1Obj2AssociationTool
  : public D3PD::SingleAssociationTool<Obj1, Obj2>
{
public:
  typedef D3PD::SingleAssociationTool<Obj1, Obj2> Base;


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  Obj1Obj2AssociationTool (const std::string& type,
                           const std::string& name,
                           const IInterface* parent);


  /**
   * @brief Return the target object.
   * @param p The source object for the association.
   *
   * Return the target of the association, or 0.
   */
  virtual const Obj2* get (const Obj1& p);
};


} // namespace D3PDTest



#endif // not D3PDMAKERTEST_OBJ1OBJ2ASSOCIATIONTOOL_H
