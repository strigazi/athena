//Dear emacs, this is -*-c++-*- 
/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#include "FileMetaData.h"
#include <iostream>

void FileMetaData::dump(std::ostream& out) const {

  if (!m_valid) {
    out << "File Meta Data is invalid" << std::endl;
    return;
  }


  out << "SG Keys: ";
  for (const std::string& k : m_sgKeys) {out << k << " ";}
  out << std::endl;

  out << "Run Number(s): ";
  for (const auto rn : m_runNumbers) {out << rn << " ";}
  out << std::endl;

  out << "Lumiblock number(s): ";
  for (const auto lb :  m_lbNumbers) {out << lb << " ";}
  out  << std::endl;
  
  out << "Project: " << m_project << std::endl;
  out << "Stream:" << m_stream << std::endl;
  
  out << "GUID: " << m_guid << std::endl;
  
  out << "Conditions Tag: " <<  m_condTag << std::endl;
  out << "Geometry Tag: " <<  m_geoTag << std::endl;

  out << "Beam Energy: " << m_beamEnergy << std::endl;
  out << "Beam Type: " << m_beamType << std::endl;
  
  out << "Number of events: " << m_nEvents << std::endl;

  return;
}


void  FileMetaData::keyValueDump(std::ostream& out) const {
  
  if (!m_valid) return;

  out << "RunNumber:";
  for (const auto rn : m_runNumbers) {out << " " << rn;}
  out << std::endl;

  out << "LBNumber: ";
  for (const auto lb :  m_lbNumbers) {out << " " << lb;}
  out  << std::endl;
  
  out << "Project: " << m_project << std::endl;
  out << "Stream:" << m_stream << std::endl;
  
  out << "GUID: " << m_guid << std::endl;
  
  out << "ConditionsTag: " <<  m_condTag << std::endl;
  out << "GeometryTag: " <<  m_geoTag << std::endl;

  out << "BeamEnergy: " << m_beamEnergy << std::endl;
  out << "BeamType: " << m_beamType << std::endl;
  
  out << "NEvents: " << m_nEvents << std::endl;


  out << "SGKeys: ";
  for (const std::string& k : m_sgKeys) {out << k << " ";}
  out << std::endl;

  return;
}
