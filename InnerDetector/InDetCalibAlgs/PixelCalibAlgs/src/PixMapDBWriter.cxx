/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

// PixelCalibAlgs
#include "PixelCalibAlgs/PixMapDBWriter.h"
#include "PixelCalibAlgs/PixelConvert.h"

// PixelConditions
#include "PixelConditionsServices/ISpecialPixelMapSvc.h"
#include "PixelConditionsData/SpecialPixelMap.h"

// geometry
#include "InDetIdentifier/PixelID.h"
#include "InDetReadoutGeometry/PixelDetectorManager.h" // kazuki
#include "InDetReadoutGeometry/SiDetectorElement.h" // kazuki
#include "InDetReadoutGeometry/PixelModuleDesign.h" // kazuki
#include "InDetReadoutGeometry/SiDetectorElementCollection.h" // kazuki

// ROOT
#include "TFile.h"
#include "TKey.h"
#include "TDirectory.h"
#include "TH2.h"
#include "TString.h" // kazuki

// standard library
#include <vector> // kazuki
#include <map> // kazuki
#include <string> // kazuki
#include <algorithm> // kazuki
#include <fstream> // kazuki


PixMapDBWriter::PixMapDBWriter(const std::string& name, ISvcLocator* pSvcLocator) :
  AthAlgorithm(name, pSvcLocator),
  m_specialPixelMapSvc("SpecialPixelMapSvc", name),
  m_specialPixelMapKey(""),
  m_doValidate(false),
  m_calculateOccupancy(false),
  m_nBCReadout(1),
  m_pixelPropertyName("noise"),
  m_pixelStatus(32769), //default for noisy pixels
  m_listSpecialPixels(true),
  m_isIBL(true), // kazuki
  m_pixMapFileName("PixMap.root"),
  m_pixelID(0),
  m_pixman(0) // kazuki
{
  declareProperty("SpecialPixelMapKey", m_specialPixelMapKey, "Key of the map to be merged with/compared to the new pixel map");
  declareProperty("DoValidate", m_doValidate, "Switch for validation mode");
  declareProperty("CalculateOccupancy", m_calculateOccupancy, "Switch for calculation of occupancy");
  declareProperty("NBCReadout", m_nBCReadout, "Number of bunch crossings read out per event");
  declareProperty("PixelPropertyName", m_pixelPropertyName, "Name of the special property of pixels, beginning of directory name in input root file");
  declareProperty("PixelStatus", m_pixelStatus, "Pixel status used for validation/writing");
  declareProperty("ListSpecialPixels", m_listSpecialPixels, "Switch for printout of complete special pixels list");
  declareProperty("isIBL", m_isIBL, "If false IBL not considered"); // kazuki
  declareProperty("PixMapFileName", m_pixMapFileName, "File name of the ROOT file generated by PixMapBuilder");
}

PixMapDBWriter::~PixMapDBWriter(){}



StatusCode PixMapDBWriter::initialize(){

  ATH_MSG_DEBUG( "Initializing PixMapDBWriter" );

  StatusCode sc = m_specialPixelMapSvc.retrieve();
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve SpecialPixelMapSvc" );
    return StatusCode::FAILURE;
  }

  sc = detStore()->retrieve( m_pixelID, "PixelID" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID helper" );
    return StatusCode::FAILURE;
  }

  // kazuki ==>
  sc = detStore()->retrieve( m_pixman, "Pixel" );
  if( !sc.isSuccess() ){
    ATH_MSG_FATAL( "Unable to retrieve pixel ID manager" );
    return StatusCode::FAILURE;
  }
  // <== //

  return StatusCode::SUCCESS;
}


StatusCode PixMapDBWriter::execute(){
  ATH_MSG_DEBUG( "Executing PixMapDBWriter" );

  return StatusCode::SUCCESS;
}


StatusCode PixMapDBWriter::finalize(){
  ATH_MSG_INFO( "Finalizing PixMapDBWriter" );

  ATH_MSG_DEBUG( "Reading map of special pixels from file" );

  TFile pixMapFile(m_pixMapFileName.c_str(), "READ");

  std::map<std::string, TH2C*> specialPixelHistograms; // kazuki
  std::map<std::string, TH2D*> hitMaps; // kazuki
  std::map<int, std::map<int, int> > ModulePixelidStatus; // kazuki

  // endcaps

  std::vector<std::string> directions;
  directions.push_back("A");
  directions.push_back("C");

  int nmodules=0, npixels=0;

  for(std::vector<std::string>::const_iterator direction = directions.begin(); direction != directions.end(); ++direction){

    std::string specialPixelHistogramsDirName = m_pixelPropertyName + std::string("Maps_endcap") + (*direction);

    for(int layer = 1; layer <= 3; layer++)
    {

      std::ostringstream component, specialPixelHistogramsPath;

      component << "Disk" << layer << (*direction);
      specialPixelHistogramsPath << m_pixelPropertyName + "Maps_endcap" << (*direction) << "/Disk" << layer;

      for(int index = 0; index < 48; index++) // loop in TDirectory
      {
        // get DCS ID
        std::string name((static_cast<TKey*>
              ((static_cast<TDirectory*>
                (pixMapFile.Get(specialPixelHistogramsPath.str().c_str())))->
               GetListOfKeys()->At(index)))->GetName());

        specialPixelHistograms[name] = /////////////////////////////////////////////////////////////////
          static_cast<TH2C*>
          ((static_cast<TKey*>
            ((static_cast<TDirectory*>
              (pixMapFile.Get(specialPixelHistogramsPath.str().c_str())))
             ->GetListOfKeys()->At(index)))->ReadObj());
      }

      if( m_calculateOccupancy ){

        std::ostringstream hitMapsPath;

        hitMapsPath << "hitMaps_endcap" << (*direction) << "/Disk" << layer;

        for(int index = 0; index < 48; index++) // loop in TDirectory
        {
          std::string name((static_cast<TKey*>
                ((static_cast<TDirectory*>
                  (pixMapFile.Get(hitMapsPath.str().c_str())))->
                 GetListOfKeys()->At(index)))->GetName());

          hitMaps[name] =
            static_cast<TH2D*>
            ((static_cast<TKey*>
              ((static_cast<TDirectory*>
                (pixMapFile.Get(hitMapsPath.str().c_str())))
               ->GetListOfKeys()->At(index)))->ReadObj());
        }
      } // end if (m_calculateOccupancy)
    } // end loop on layer
  } // end loop on Endcap direction


  // barrel

  std::vector<int> staves;
  if(m_isIBL) staves.push_back(14); // --- IBL --- //
  staves.push_back(22);
  staves.push_back(38);
  staves.push_back(52);

  std::vector<std::string> layers;
  if(m_isIBL) layers.push_back("IBL"); // --- IBL --- //
  layers.push_back("B-layer");
  layers.push_back("Layer1");
  layers.push_back("Layer2");


  for(unsigned int layer = 0; layer < layers.size(); layer++)
  {
    

      std::ostringstream specialPixelHistogramsPath;

      specialPixelHistogramsPath << m_pixelPropertyName + "Maps_barrel/" << layers[layer];

      int nModulesPerStave = 13;
      if (m_isIBL && layer == 0) nModulesPerStave = 20; // --- IBL --- //
      for(int module = 0; module < staves[layer] * nModulesPerStave; module++) // loop on modules
      {
        std::string name((static_cast<TKey*>
              ((static_cast<TDirectory*>
                (pixMapFile.Get(specialPixelHistogramsPath.str().c_str())))->
               GetListOfKeys()->At(module)))->GetName());

       
        specialPixelHistograms[name] =
          static_cast<TH2C*>
          ((static_cast<TKey*>
            ((static_cast<TDirectory*>
              (pixMapFile.Get(specialPixelHistogramsPath.str().c_str())))
             ->GetListOfKeys()->At(module)))->ReadObj());
        
      }

      if( m_calculateOccupancy ){

        std::ostringstream hitMapsPath;

        hitMapsPath << "hitMaps_barrel/" << layers[layer];

        for(int module = 0; module < staves[layer] * nModulesPerStave; module++) // loop on modules
        {

          std::string name((static_cast<TKey*>
                ((static_cast<TDirectory*>
                  (pixMapFile.Get(hitMapsPath.str().c_str())))->
                 GetListOfKeys()->At(module)))->GetName());


          hitMaps[name] =
            static_cast<TH2D*>
            ((static_cast<TKey*>
              ((static_cast<TDirectory*>
                (pixMapFile.Get(hitMapsPath.str().c_str())))
               ->GetListOfKeys()->At(module)))->ReadObj());
        } // end loop on modules
      } // end if ( m_calculateOccupancy )
    //} // end loop on staves
  } // end loop on layers

  ////////////////////////////////////////////////////////////////////////////////////////////
  // temporary repairs of PixelConvert::DCSID
  ////////////////////////////////////////////////////////////////////////////////////////////


  char* tmppath = std::getenv("DATAPATH");
  const unsigned int maxPathStringLength{3000};
  if((not tmppath) or (strlen(tmppath) > maxPathStringLength) ){
      ATH_MSG_FATAL( "Unable to retrieve environmental DATAPATH" );
      return StatusCode::FAILURE;
  }
  std::stringstream tmpSstr{};
  tmpSstr<<tmppath;
  std::string cmtpath(tmpSstr.str());
  std::vector<std::string> paths = splitter(cmtpath, ':');
  std::ifstream ifs;
  for (const auto& x : paths){
    if(is_file_exist((x + "/PixelMapping_Run2.dat").c_str())){
      if(m_isIBL){
        ifs.open(x + "/PixelMapping_Run2.dat");
      } else {
        ifs.open(x + "/PixelMapping_May08.dat");
      }
      int tmp_barrel_ec; int tmp_layer; int tmp_module_phi; int tmp_module_eta; std::string tmp_module_name;
      std::vector<int> tmp_position;
      tmp_position.resize(4);
      while(ifs >> tmp_barrel_ec >> tmp_layer >> tmp_module_phi >> tmp_module_eta >> tmp_module_name) {
        tmp_position[0] = tmp_barrel_ec;
        tmp_position[1] = tmp_layer;
        tmp_position[2] = tmp_module_phi;
        tmp_position[3] = tmp_module_eta;
        m_pixelMapping.push_back(std::make_pair(tmp_module_name, tmp_position));
      }
      break;
    }
  }


  ////////////////////////////////////////////////////////////////////////////////////////////
  // temporary repairs of PixelConvert::DCSID
  ////////////////////////////////////////////////////////////////////////////////////////////

  auto spm = std::make_unique<DetectorSpecialPixelMap>();
  double nHitsBeforeMask = 0.;
  double nHitsAfterMask = 0.;

  spm->resize(m_pixelID->wafer_hash_max());

  // kazuki
  InDetDD::SiDetectorElementCollection::const_iterator iter, itermin, itermax;
  itermin = m_pixman->getDetectorElementBegin();
  itermax = m_pixman->getDetectorElementEnd();
  int niterators=0; // kazuki
  if(m_pixelID->wafer_hash_max() > 1744) m_isIBL = true; // #modules only Pixel is 1744

  for( iter = itermin; iter != itermax; ++iter) {
    niterators++; // kazuki
    const InDetDD::SiDetectorElement* element = *iter;
    if(element == 0) continue;
    Identifier ident = element->identify();
    if(!m_pixelID->is_pixel(ident)) continue;  // OK this Element is included
    const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
    if(!design) continue;
    unsigned int mchips = design->numberOfCircuits();
    int mrow =design->rows();
    int barrel     = m_pixelID->barrel_ec (ident);
    int layer      = m_pixelID->layer_disk(ident); // kazuki
    int module_phi = m_pixelID->phi_module(ident);
    int module_eta = m_pixelID->eta_module(ident); // kazuki
    int moduleHash = m_pixelID->wafer_hash(ident);
    int chipType = 1; // FE-I4
    if( mchips==8 || abs(barrel)==2 || (barrel==0 && layer > 0) ){ // if FE-I3
      mchips *=2; // guess numberOfCircuits()
      mrow /=2; // number of chips in row
      chipType = 0; // FE-I3
    }
    (*spm)[moduleHash] = new ModuleSpecialPixelMap();
    unsigned int chipsPerModule = mchips * 10 + chipType;
    (*spm)[moduleHash]->setchipsPerModule(chipsPerModule);
    TString onlineID;
    if (m_isIBL) { // --- IBL --- //
      onlineID = TString( PixMapDBWriter::getDCSIDFromPosition(barrel,layer,module_phi,module_eta ) );
   
    }
    if ( specialPixelHistograms.find(std::string(onlineID)) == specialPixelHistograms.end() ) {
      ATH_MSG_DEBUG("onlineID " << onlineID << " is not found in the specialPixelHistograms" );
      continue;
    }
    TH2C* specialPixelHistogram = specialPixelHistograms[std::string(onlineID)];
    int nspecial = static_cast<int>( specialPixelHistogram->GetEntries() ); // #noisy pixels
    if( !m_calculateOccupancy ){
      if ( nspecial == 0 ) continue; // no noise hits
    }
    TH2D* hitMap = 0;

    if( m_calculateOccupancy ){
      hitMap = hitMaps[std::string(onlineID)];
      if( hitMap->GetEntries() == 0 ) continue;
    }

    nmodules++; // nmodules is total # of modules which contain noisy pixels
    npixels += nspecial; // npixels is total # of noisy pixels...

    double nHitsAfterMaskThisModule = 0.;

    for(int pixel_eta = 0; pixel_eta <= m_pixelID->eta_index_max(ident); pixel_eta++)
    {
      for(int pixel_phi = 0; pixel_phi <= m_pixelID->phi_index_max(ident); pixel_phi++)
      {

        bool special = false;

        if( nspecial != 0 && specialPixelHistogram->GetBinContent(pixel_eta + 1, pixel_phi + 1) != 0 )
        {
          special = true;
        }

        if( special ){ // noisy/dead pixel

          unsigned int pixelID =
            ModuleSpecialPixelMap::encodePixelID(barrel, module_phi, pixel_eta, pixel_phi, chipsPerModule); 
          (*spm)[moduleHash]->setPixelStatus(pixelID, m_pixelStatus);
          ModulePixelidStatus[moduleHash][pixelID] = m_pixelStatus;

          if( m_listSpecialPixels ){
            if (!(pixel_phi%1000)) { // thin out
              ATH_MSG_INFO( "moduleHash = " << moduleHash << " barrel_ec = " << barrel << " module_phi = " << module_phi << " pixel_eta = " << pixel_eta << " pixel_phi = " << pixel_phi << " mchips = " << mchips << " pixelID = " << pixelID ); // too much...
            }
          }
        } // end if special

        if( m_calculateOccupancy && !special ){

          nHitsAfterMaskThisModule += hitMap->GetBinContent( pixel_eta + 1, pixel_phi + 1 );
        }
      } // end loop on pixel_phi
    } // end loop on pixel_eta

    if( m_calculateOccupancy ){
      nHitsBeforeMask += hitMap->GetEntries();
      nHitsAfterMask += nHitsAfterMaskThisModule;
    }
  } // end loop on element

  ATH_MSG_INFO( "Total of " << npixels << " masked pixels on " << nmodules << " modules" );

  spm->markSpecialRegions();
  spm->setNeighbourFlags();


  if(m_doValidate){
    DetectorSpecialPixelMap* spmFromDB = 0;

    StatusCode sc = detStore()->retrieve(spmFromDB, m_specialPixelMapKey);

    if( !sc.isSuccess() ){
      ATH_MSG_FATAL( "Unable to retrieve reference special pixel map" );
      return StatusCode::FAILURE;
    }

    int nMaskedPixels1 = 0; // this
    int nMaskedPixels2 = 0; // reference

    int nCommonMaskedPixels = 0;

    int nModulesWithMaskedPixels1 = 0;
    int nModulesWithMaskedPixels2 = 0;

    int nCommonModulesWithMaskedPixels = 0;

    std::vector<std::pair<unsigned int, Identifier> > results;
    std::vector<std::pair<unsigned int, Identifier> > results_absnum;

    for( iter = itermin; iter != itermax; ++iter){
      const InDetDD::SiDetectorElement* element = *iter;
      if(element == 0) continue;
      Identifier ident = element->identify();
      if(!m_pixelID->is_pixel(ident)) continue;  // OK this Element is included
      const InDetDD::PixelModuleDesign* design = dynamic_cast<const InDetDD::PixelModuleDesign*>(&element->design());
      if(!design) continue;
      unsigned int mchips = design->numberOfCircuits();
      int mrow =design->rows();
      int barrel     = m_pixelID->barrel_ec (ident);
      int layer      = m_pixelID->layer_disk(ident); // kazuki
      int module_phi = m_pixelID->phi_module(ident);
      int module_eta = m_pixelID->eta_module(ident); // kazuki
      int moduleHash = m_pixelID->wafer_hash(ident);
      int chipType = 1; // FE-I4
      if( mchips==8 || abs(barrel)==2 || (barrel==0 && layer > 0) ){ // if FE-I3
        mchips *=2; // guess numberOfCircuits()
        mrow /=2; // number of chips in row
        chipType = 0; // FE-I3
      }
      unsigned int chipsPerModule = mchips * 10 + chipType;

      //for(unsigned int moduleHash = 0; moduleHash < m_pixelID->wafer_hash_max(); moduleHash++)

      int nMaskedPixelsThis1 = 0;
      int nMaskedPixelsThis2 = 0;

      int nCommonMaskedPixelsThis = 0;

      for(int pixel_eta = 0; pixel_eta <= m_pixelID->eta_index_max(ident); pixel_eta++)
      {
        //for(int pixel_phi = 0; pixel_phi <= m_pixelID->phi_index_max(moduleID); pixel_phi++)
        for(int pixel_phi = 0; pixel_phi <= m_pixelID->phi_index_max(ident); pixel_phi++)
        {

          unsigned int pixelID =
            ModuleSpecialPixelMap::encodePixelID(barrel, module_phi, pixel_eta, pixel_phi, chipsPerModule);

          unsigned int pixelStatus1 = ModulePixelidStatus[moduleHash][pixelID];
          unsigned int pixelStatus2 = (*spmFromDB)[moduleHash]->pixelStatus(pixelID);

          if (pixelStatus1 != 0)
          {

            ATH_MSG_DEBUG( "barrel, layer, module_phi, module_eta, pixel_eta, pixel_phi, mchips, pixelID\n"
                      << " = " << barrel << ", " << layer << ", " << module_phi << ", " << module_eta
                      << ", " << pixel_eta << ", " << pixel_phi << ", " << mchips << ", " << pixelID 
                      << "\npixelStatus1, pixelStatus2 = " << pixelStatus1 << ", " << pixelStatus2 );
          }

          if( (pixelStatus1 & m_pixelStatus) == m_pixelStatus ){
            nMaskedPixels1++;
            nMaskedPixelsThis1++;
          }

          if( (pixelStatus2 & m_pixelStatus) == m_pixelStatus ){
            nMaskedPixels2++;
            nMaskedPixelsThis2++;
          }

          if( (pixelStatus2 & m_pixelStatus) == m_pixelStatus &&
              (pixelStatus1 & m_pixelStatus) == m_pixelStatus ){ // both this and ref have the same status
            nCommonMaskedPixels++;
            nCommonMaskedPixelsThis++;
          }
        }
      }

      if(nMaskedPixelsThis1 != 0) nModulesWithMaskedPixels1++;
      if(nMaskedPixelsThis2 != 0) nModulesWithMaskedPixels2++;
      if(nMaskedPixelsThis1 != 0 && nMaskedPixelsThis2 != 0) nCommonModulesWithMaskedPixels++;

      results.push_back(std::pair<unsigned int, Identifier>
          (std::abs(nMaskedPixelsThis1 - nMaskedPixelsThis2), ident));
          //(std::abs(nMaskedPixelsThis1 - nMaskedPixelsThis2), moduleID));
      results_absnum.push_back(std::pair<unsigned int, Identifier>
          (std::abs(nMaskedPixelsThis1), ident));
    }

    std::sort(results.rbegin(), results.rend());
    std::sort(results_absnum.rbegin(), results_absnum.rend());

    ATH_MSG_INFO("");

    msg().precision(1);
    msg().setf(std::ios::fixed);

    ATH_MSG_INFO("");
    ATH_MSG_INFO( "Number of modules with masked pixels:" );
    ATH_MSG_INFO( "Common " << nCommonModulesWithMaskedPixels
        << ", New map " << nModulesWithMaskedPixels1 << " ("
        << static_cast<double>(nCommonModulesWithMaskedPixels) / static_cast<double>(nModulesWithMaskedPixels1 + 0.00001) * 100. //Add epsilon to avoid potential FPE
        << "% common), Reference map " << nModulesWithMaskedPixels2 << " ("
        << static_cast<double>(nCommonModulesWithMaskedPixels) / static_cast<double>(nModulesWithMaskedPixels2 + 0.00001) * 100.
        << "% common)" );

    ATH_MSG_INFO( "Changes: -" << nModulesWithMaskedPixels2 - nCommonModulesWithMaskedPixels
        << " (-" << static_cast<double>(nModulesWithMaskedPixels2 - nCommonModulesWithMaskedPixels) /
        static_cast<double>(nModulesWithMaskedPixels2 + 0.00001) * 100. << "%), +"
        << nModulesWithMaskedPixels1 - nCommonModulesWithMaskedPixels
        << " (+" << static_cast<double>(nModulesWithMaskedPixels1 - nCommonModulesWithMaskedPixels) /
        static_cast<double>(nModulesWithMaskedPixels2 + 0.00001) * 100. << "%), total difference " << std::showpos
        << nModulesWithMaskedPixels1 - nModulesWithMaskedPixels2 << " ("
        << static_cast<double>(nModulesWithMaskedPixels1 - nModulesWithMaskedPixels2) /
        static_cast<double>(nModulesWithMaskedPixels2 + 0.00001) * 100. << "%)" << std::noshowpos );

    ATH_MSG_INFO("");
    ATH_MSG_INFO( "Number of masked pixels (ganged pixels enter twice):" );
    ATH_MSG_INFO( "Common " << nCommonMaskedPixels
        << ", New map " << nMaskedPixels1 << " ("
        << static_cast<double>(nCommonMaskedPixels) / static_cast<double>(nMaskedPixels1 + 0.00001) * 100. //Add epsilon to avoid potential FPE
        << "% common), Reference map " << nMaskedPixels2 << " ("
        << static_cast<double>(nCommonMaskedPixels) / static_cast<double>(nMaskedPixels2 + 0.00001) * 100.
        << "% common)" );


    ATH_MSG_INFO( "Changes: -" << nMaskedPixels2 - nCommonMaskedPixels << " (-"
        << static_cast<double>(nMaskedPixels2 - nCommonMaskedPixels) /
        static_cast<double>(nMaskedPixels2 + 0.00001) * 100. << "%), +"
        << nMaskedPixels1 - nCommonMaskedPixels << " (+"
        << static_cast<double>(nMaskedPixels1 - nCommonMaskedPixels) /
        static_cast<double>(nMaskedPixels2 + 0.00001) * 100. << "%), total difference " << std::showpos
        << nMaskedPixels1 - nMaskedPixels2 << " ("
        << static_cast<double>(nMaskedPixels1 - nMaskedPixels2) /
        static_cast<double>(nMaskedPixels2 + 0.00001) * 100. << "%)" << std::noshowpos );


    ATH_MSG_INFO("");
    ATH_MSG_INFO( "Modules with largest differences:" );
    ATH_MSG_INFO("");

    for(int i = 0; i < 20 ; i++){

      if ( m_isIBL ) {
        int barrel     = m_pixelID->barrel_ec(results[i].second);
        int layer      = m_pixelID->layer_disk(results[i].second); // kazuki
        int module_phi = m_pixelID->phi_module(results[i].second);
        int module_eta = m_pixelID->eta_module(results[i].second); // kazuki
        TString tmp_onlineID;
        tmp_onlineID = TString( PixMapDBWriter::getDCSIDFromPosition(barrel,layer,module_phi,module_eta ) );

        ATH_MSG_INFO( results[i].first << " pixels differing on module   \t" << results[i].second
            << " \t" << tmp_onlineID );
      } else {
        unsigned int hashID = ( ((m_pixelID->barrel_ec(results[i].second) + 2) / 2) << 25 ) +
          ( m_pixelID->layer_disk(results[i].second) << 23) +
          ( m_pixelID->phi_module(results[i].second) << 17) +
          ( (m_pixelID->eta_module(results[i].second) + 6) << 13);

        ATH_MSG_INFO( results[i].first << " pixels differing on module   \t" << results[i].second
            << " \t" << PixelConvert::DCSID(PixelConvert::OnlineID(hashID)) );
      }
    }

    ATH_MSG_INFO("");

    ATH_MSG_INFO("");
    ATH_MSG_INFO( "Modules with the largest number of noisy pixels:" );
    ATH_MSG_INFO("");

    for(int i = 0; i < 20 ; i++){

      if ( m_isIBL ) {
        int barrel     = m_pixelID->barrel_ec(results_absnum[i].second);
        int layer      = m_pixelID->layer_disk(results_absnum[i].second); // kazuki
        int module_phi = m_pixelID->phi_module(results_absnum[i].second);
        int module_eta = m_pixelID->eta_module(results_absnum[i].second); // kazuki
        TString tmp_onlineID;
        tmp_onlineID = TString( PixMapDBWriter::getDCSIDFromPosition(barrel,layer,module_phi,module_eta ) );

        ATH_MSG_INFO( results_absnum[i].first << " noisy pixels on module   \t" << results_absnum[i].second
            << " \t" << tmp_onlineID );
      } else {
        unsigned int hashID = ( ((m_pixelID->barrel_ec(results_absnum[i].second) + 2) / 2) << 25 ) +
          ( m_pixelID->layer_disk(results_absnum[i].second) << 23) +
          ( m_pixelID->phi_module(results_absnum[i].second) << 17) +
          ( (m_pixelID->eta_module(results_absnum[i].second) + 6) << 13);

        ATH_MSG_INFO( results_absnum[i].first << " noisy pixels on module   \t" << results_absnum[i].second
            << " \t" << PixelConvert::DCSID(PixelConvert::OnlineID(hashID)) );
      }
    }

    ATH_MSG_INFO("");
    pixMapFile.Close();
    
    return StatusCode::SUCCESS;
  } // end if dovalidate

  if( m_calculateOccupancy ){

    double nPixels = 80363520.;
    double nModules = 1744.;
    if (m_isIBL) {
      nPixels = 2880. * 16. * 1744. // Pixel
        + 26880. * 112. + 53760. * 168.  // IBL 3D + Planar
        + 26880. * 24; // DBM
      nModules = 2048.;
    }

    TH1D* disabledModules = 0;
    TH1D* nEvents = 0;

    pixMapFile.GetObject( "DisabledModules", disabledModules );
    pixMapFile.GetObject( "NEvents", nEvents );

    double nDisabledModules = 0.;

    for( int i = 1; i < nModules + 1; i++ ){
      nDisabledModules += disabledModules->GetBinContent(i);
    }

    nDisabledModules /= nEvents->GetBinContent(1);

    double nPixelsOnDisabledModules = nDisabledModules * nPixels / nModules;

    ATH_MSG_INFO("");

    ATH_MSG_INFO("Occupancy calculated for " << m_nBCReadout << " bc readout and an average of "
        << nDisabledModules << " disabled modules." );
    ATH_MSG_INFO( "Occupancy before masking of special pixels "
        << nHitsBeforeMask / (nPixels - nPixelsOnDisabledModules) /
        (nEvents->GetBinContent(1) * m_nBCReadout) << " +- "
        << std::sqrt(nHitsBeforeMask) / (nPixels - nPixelsOnDisabledModules) /
        (nEvents->GetBinContent(1) * m_nBCReadout) );
    ATH_MSG_INFO( "Occupancy after masking of special pixels "
        << nHitsAfterMask / (nPixels - nPixelsOnDisabledModules) /
        (nEvents->GetBinContent(1) * m_nBCReadout) << " +- "
        << std::sqrt(nHitsAfterMask) / (nPixels - nPixelsOnDisabledModules) /
        (nEvents->GetBinContent(1) * m_nBCReadout) );

    ATH_MSG_INFO("");
  } // end if m_calculateOccupancy

  if( !m_specialPixelMapKey.empty() ){

    DetectorSpecialPixelMap* spmFromDB = 0;

    StatusCode sc = detStore()->retrieve(spmFromDB, m_specialPixelMapKey);

    if( !sc.isSuccess() ){
      ATH_MSG_FATAL( "Unable to retrieve special pixel map from database" );
      return StatusCode::FAILURE;
    }

    ATH_MSG_INFO( "Merging special pixel map with reference map at " << m_specialPixelMapKey );

    spm->merge(spmFromDB);
  }

  if( !m_doValidate ){
    StatusCode sc = m_specialPixelMapSvc->registerCondAttrListCollection(spm.get());

    if( !sc.isSuccess() ){
      ATH_MSG_FATAL( "Unable to construct and record CondAttrListCollection" );
      return StatusCode::FAILURE;
    }
  }

  pixMapFile.Close();

  return StatusCode::SUCCESS;
}

std::string PixMapDBWriter::getDCSIDFromPosition (int barrel_ec, int layer, int module_phi, int module_eta){
  for(unsigned int ii = 0; ii < m_pixelMapping.size(); ii++) {
    if (m_pixelMapping[ii].second.size() != 4) {
      ATH_MSG_WARNING( "getDCSIDFromPosition: Vector size is not 4!" );
      return std::string("Error!");
    }
    if (m_pixelMapping[ii].second[0] != barrel_ec) continue;
    if (m_pixelMapping[ii].second[1] != layer) continue;
    if (m_pixelMapping[ii].second[2] != module_phi) continue;
    if (m_pixelMapping[ii].second[3] != module_eta) continue;
    return m_pixelMapping[ii].first;
  }
  ATH_MSG_WARNING( "Not found!" );
  return std::string("Error!");
}

std::vector<int> PixMapDBWriter::getPositionFromDCSID (std::string DCSID){
  for(unsigned int ii = 0; ii < m_pixelMapping.size(); ii++) {
    if (m_pixelMapping[ii].first == DCSID)
    return m_pixelMapping[ii].second;
  }
  ATH_MSG_WARNING( "Not found!" );
  std::vector<int> void_vec;
  return void_vec;
}
