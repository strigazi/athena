/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGL2MUONSA_ALIGNMENTBARRELLUT_H
#define TRIGL2MUONSA_ALIGNMENTBARRELLUT_H

#include "AthenaBaseComps/AthAlgTool.h"

#include "TMath.h"

#include "GaudiKernel/Service.h"
#include "GaudiKernel/IInterface.h"
#include "GaudiKernel/StatusCode.h"

#include <string>

namespace TrigL2MuonSA {
  
class AlignmentBarrelLUT: public AthAlgTool
{

  public:
    static const InterfaceID& interfaceID();

    AlignmentBarrelLUT(const std::string& type, 
                       const std::string& name,
                       const IInterface*  parent);
    ~AlignmentBarrelLUT(void);

    virtual StatusCode initialize();
    virtual StatusCode finalize  ();

    StatusCode readLUT(std::string lut_fileName);

    double GetDeltaZ(int& saddress, double& etaMap, double& phiMap, double& MFphi, float& sp1R ) const;

    std::pair<int, int> GetBinNumber(int saddress, int innerR, double etaMap, double phiMap) const;

  private:
    double m_dZ[4][2][15][30][2]; // [s_address][innerR][eta][phi][etaQ]

    int m_NbinEta[4][2]; // [s_address][innerR]
    float m_EtaMin[4][2];
    float m_EtaMax[4][2];
    float m_EtaStep[4][2];
    int m_NbinPhi[4][2];
    float m_PhiMin[4][2];
    float m_PhiMax[4][2];
    float m_PhiStep[4][2];

  };

}

#endif
