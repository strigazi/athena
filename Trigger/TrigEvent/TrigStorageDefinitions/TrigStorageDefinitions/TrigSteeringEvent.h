/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGSTEERINGEVENT
#define TRIGSTORAGEDEF_TRIGSTEERINGEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

//need to fwd declare namespace classes already here
namespace LVL1 {
  struct RecJetRoI;
  struct RecJetEtRoI;
  struct RecEmTauRoI;
  struct RecEnergyRoI;
  struct RecMuonRoI;
}

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"

#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/MuonRoIAuxContainer.h"

#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetRoIAuxContainer.h"


HLT_BEGIN_TYPE_REGISTRATION
  HLT_REGISTER_TYPE(struct TrigRoiDescriptor,struct TrigRoiDescriptor,struct TrigRoiDescriptorCollection)
  HLT_REGISTER_TYPE(struct TrigSuperRoi,struct TrigSuperRoi,struct TrigSuperRoiCollection)
  HLT_REGISTER_TYPE(struct TrigPassBits,struct TrigPassBits,struct  TrigPassBitsCollection)
  HLT_REGISTER_TYPE(struct TrigPassFlags,struct TrigPassFlags,struct  TrigPassFlagsCollection)
  HLT_REGISTER_TYPE(struct TrigRoiDescriptor,struct TrigRoiDescriptorCollection,struct TrigRoiDescriptorCollection)
  HLT_REGISTER_TYPE(struct TrigOperationalInfo,struct TrigOperationalInfo,struct  TrigOperationalInfoCollection)
  HLT_REGISTER_TYPE(LVL1::RecJetRoI ,LVL1::RecJetRoI, DataVector<LVL1::RecJetRoI>)
  HLT_REGISTER_TYPE(LVL1::RecJetEtRoI,LVL1::RecJetEtRoI, DataVector<LVL1::RecJetEtRoI>)
  HLT_REGISTER_TYPE(LVL1::RecEmTauRoI, LVL1::RecEmTauRoI, DataVector<LVL1::RecEmTauRoI>)
  HLT_REGISTER_TYPE(LVL1::RecEnergyRoI, LVL1::RecEnergyRoI, DataVector<LVL1::RecEnergyRoI>)
  HLT_REGISTER_TYPE(LVL1::RecMuonRoI,LVL1::RecMuonRoI, DataVector<LVL1::RecMuonRoI>)
  HLT_REGISTER_TYPE(xAOD::EmTauRoI, xAOD::EmTauRoIContainer, xAOD::EmTauRoIContainer, xAOD::EmTauRoIAuxContainer)
  HLT_REGISTER_TYPE(xAOD::EmTauRoI, xAOD::EmTauRoI, xAOD::EmTauRoIContainer, xAOD::EmTauRoIAuxContainer)
  HLT_REGISTER_TYPE(xAOD::MuonRoI, xAOD::MuonRoIContainer, xAOD::MuonRoIContainer, xAOD::MuonRoIAuxContainer)
  HLT_REGISTER_TYPE(xAOD::MuonRoI, xAOD::MuonRoI, xAOD::MuonRoIContainer, xAOD::MuonRoIAuxContainer)
  HLT_REGISTER_TYPE(xAOD::JetRoI, xAOD::JetRoIContainer, xAOD::JetRoIContainer, xAOD::JetRoIAuxContainer)
  HLT_REGISTER_TYPE(xAOD::JetRoI, xAOD::JetRoI, xAOD::JetRoIContainer, xAOD::JetRoIAuxContainer)
HLT_END_TYPE_REGISTRATION(TrigSteeringEvent)

#endif
