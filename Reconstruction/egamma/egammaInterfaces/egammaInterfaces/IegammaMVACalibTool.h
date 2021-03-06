/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/
#ifndef EGAMMAINTERFACES_iEGAMMAMVACALIBTOOL_H
#define EGAMMAINTERFACES_iEGAMMAMVACALIBTOOL_H

#include "GaudiKernel/IAlgTool.h"

// EDM includes
#include "xAODEgamma/EgammaFwd.h"
#include "xAODCaloEvent/CaloClusterFwd.h"

/**
 * @class IegammaMVACalibTool
 * @brief A tool used by the egammaMVASvc to help manage the MVAs.
 **/
class IegammaMVACalibTool : virtual public IAlgTool {
public:
  DeclareInterfaceID(IegammaMVACalibTool, 1, 0);

  virtual ~IegammaMVACalibTool() override {};

  ///Return MVA energy for the given cluster, an eg object is optional
  virtual float getEnergy(const xAOD::CaloCluster& clus,
                         const xAOD::Egamma* eg) const = 0;

}; 

#endif
