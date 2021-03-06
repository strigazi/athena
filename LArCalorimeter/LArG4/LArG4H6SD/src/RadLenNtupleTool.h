/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef LARG4H6SD_RADLENNTUPLETOOL_H
#define LARG4H6SD_RADLENNTUPLETOOL_H

// System includes
#include <string>

// Infrastructure includes
#include "G4AtlasTools/UserActionToolBase.h"

// Local includes
#include "RadLenNtuple.h"

namespace G4UA
{
  /// @class RadLenNtupleTool
  /// @brief Tool which manages the RadLenNtuple action.
  ///
  /// Create the RadLenNtuple for each worker thread
  ///
  /// @author Andrea Di Simone
  ///
  class RadLenNtupleTool : public UserActionToolBase<RadLenNtuple>
  {

  public:

    /// Standard constructor
    RadLenNtupleTool(const std::string& type, const std::string& name,
                     const IInterface* parent);

    /// Framework methods
    StatusCode initialize() override final;

  protected:

    /// Create action for this thread
    virtual std::unique_ptr<RadLenNtuple>
    makeAndFillAction(G4AtlasUserActions&) override final;

  private:
    /// Configuration parameters
    RadLenNtuple::Config m_config;

  }; // class RadLenNtupleTool

} // namespace G4UA

#endif
