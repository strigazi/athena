///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METTauAssociator.h 
// Header file for class METTauAssociator
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 
#ifndef METRECONSTRUCTION_METTAUASSOCIATOR_H
#define METRECONSTRUCTION_METTAUASSOCIATOR_H 1

// METReconstruction includes
#include "METReconstruction/METAssociator.h"

namespace met{
  class METTauAssociator final
    : public METAssociator
  { 
    // This macro defines the constructor with the interface declaration
    ASG_TOOL_CLASS(METTauAssociator, IMETAssocToolBase)


    /////////////////////////////////////////////////////////////////// 
    // Public methods: 
    /////////////////////////////////////////////////////////////////// 
    public: 

    // Constructor with name
    METTauAssociator(const std::string& name);
    ~METTauAssociator();

    // AsgTool Hooks
    StatusCode  initialize();
    StatusCode  finalize();

    /////////////////////////////////////////////////////////////////// 
    // Const methods: 
    ///////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////// 
    // Non-const methods: 
    /////////////////////////////////////////////////////////////////// 

    /////////////////////////////////////////////////////////////////// 
    // Private data: 
    /////////////////////////////////////////////////////////////////// 
    protected: 

    StatusCode executeTool(xAOD::MissingETContainer* metCont, xAOD::MissingETAssociationMap* metMap) const final;
    StatusCode extractTopoClusters(const xAOD::IParticle* obj,
				   std::vector<const xAOD::IParticle*>& tclist,
				   const met::METAssociator::ConstitHolder& constits) const final;

    StatusCode extractPFO(const xAOD::IParticle* obj,
			  std::vector<const xAOD::IParticle*>& pfolist,
			  const met::METAssociator::ConstitHolder& constits,
			  std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta) const final;

/*
    StatusCode GetPFOWana(const xAOD::IParticle*,
          std::vector<const xAOD::IParticle*>&,
          const met::METAssociator::ConstitHolder&,
          std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &,
          std::vector<double>&,
          unsigned int&,
          float&) const final
    {return StatusCode::FAILURE;} // should not be called          
*/ 

    StatusCode hadrecoil_PFO(std::vector<const xAOD::IParticle*>, 
                            const met::METAssociator::ConstitHolder&, 
                            TLorentzVector&,
                            std::vector<double>&) const final
    {return StatusCode::SUCCESS;} // should not be called  


    StatusCode GetPFOWana(const xAOD::IParticle* obj,
                         std::vector<const xAOD::IParticle*>& pfolist,
                         const met::METAssociator::ConstitHolder& constits,
                         std::map<const xAOD::IParticle*,MissingETBase::Types::constvec_t> &momenta,
                         std::vector<double>& vPhiRnd,
                         unsigned int& lept_count,
                         float& UEcorr) const final;

/*
    StatusCode hadrecoil_PFO(std::vector<const xAOD::IParticle*> hardObjs, 
                             const met::METAssociator::ConstitHolder& constits, 
                             TLorentzVector& HR,
                             std::vector<double>& vPhiRnd) const final; 
*/    
    
    StatusCode extractTracks(const xAOD::IParticle* obj,
			     std::vector<const xAOD::IParticle*>& constlist,
			     const met::METAssociator::ConstitHolder& constits) const final;

    private:
 
    /// Default constructor: 
    METTauAssociator();

  }; 

}

#endif //> !METRECONSTRUCTION_METTAUASSOCIATOR_H
