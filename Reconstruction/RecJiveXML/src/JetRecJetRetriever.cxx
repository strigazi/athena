/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "RecJiveXML/JetRecJetRetriever.h"

#include "JetEvent/JetCollection.h"
#include "JetEvent/Jet.h"

#include "Navigation/NavigationToken.h"  
#include "CaloEvent/CaloCell.h"

#include "JiveXML/DataType.h"

#include "JetTagEvent/TrackAssociation.h"
#include "TrkTrack/TrackCollection.h"


namespace JiveXML {

  /**
   * This is the standard AthAlgTool constructor
   * @param type   AlgTool type name
   * @param name   AlgTool instance name
   * @param parent AlgTools parent owning this tool
   **/
  JetRecJetRetriever::JetRecJetRetriever(const std::string& type,const std::string& name,const IInterface* parent):
    AthAlgTool(type,name,parent),
    m_typeName("Jet"){

    //Only declare the interface
    declareInterface<IDataRetriever>(this);
    
    declareProperty("FavouriteJetCollection" ,m_sgKeyFavourite = "AntiKt4TopoEMJets" );
    declareProperty("OtherJetCollections" ,m_otherKeys);
    declareProperty("DoWriteHLT", m_doWriteHLT = false); // ignore HLTAutoKey objects
    declareProperty("TrackCollection", m_trackCollection = "TrackParticleCandidate");
    declareProperty("WriteJetQuality", m_writeJetQuality = false,"Don't write extended jet quality details by default.");
  }

  /**
   * Manually match measured perigee d0,z0,phi0 of
   * Trk::Track and Rec::TrackParticle, some problem with direct
   * match depite identical values (maybe worth trying again).
  **/

  void manualPerigeeMatch(const Trk::Perigee* per1, 
                          const Trk::Perigee* per2, bool& matched){

    double diff1 = ( per1->parameters()[Trk::d0] -
                     per2->parameters()[Trk::d0] );
    double diff2 = ( per1->parameters()[Trk::z0] -
                     per2->parameters()[Trk::z0] );
    double diff3 = ( per1->parameters()[Trk::phi0] -
                     per2->parameters()[Trk::phi0] );
    double diff4 = ( per1->charge() -
                     per2->charge() );
    double diff5 = ( per1->pT() -
                     per2->pT() );

    if (diff1+diff2+diff3+diff4+diff5 == 0.){
       matched = true;
    }else{
       matched = false;
    }
  }
 
  /**
   * Retrieve measured perigee, automatically switch between
   * Trk::Track and Rec::TrackParticle depending on selected input collection
   * for tracks
  **/

  StatusCode JetRecJetRetriever::fillPerigeeList() {

/// read TrackParticleCollection to compare Perigee and ordering
// from:
//   http://atlas-sw.cern.ch/cgi-bin/viewcvs-all.cgi/users/jsvirzi/PhysicsAnalysis/Atlas_ESD_Analysis/src/Atlas_ESD_Analysis.cxx?root=atlas&view=co
///// example:
//  InnerDetector/InDetMonitoring/InDetAlignmentMonitoring/TrackSplitterTool.cxx
// (this class has now been deleted !)
/////
//    const Trk::Perigee* trackPerigee = inputTrack->perigeeParameters();
//    const Trk::MeasuredPerigee* measuredperigee = dynamic_cast<const Trk::MeasuredPerigee*>( trackPerigee );
//    double m_d0 = measuredperigee->parameters()[Trk::d0];
//
///// Note: replace all 'measuredPerigee' with just 'perigee'. Migration Dec'13
/////    https://twiki.cern.ch/twiki/bin/viewauth/Atlas/MigrationCLHEPtoEigen#Removed_Classes
//
    const Rec::TrackParticleContainer* tracks = NULL ;
    const TrackCollection* trktracks = NULL ;
 
    size_t found;    
    std::string searchStr = "TrackParticle";
    found=m_trackCollection.find(searchStr);

    m_perigeeVector.clear(); // need to clear, otherwise accumulates over events
    if (found!=std::string::npos){ // User selected a Rec::TrackParticle Collection
      if (evtStore()->retrieve(tracks, m_trackCollection).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to retrieve track collection"
                                                  << m_trackCollection << " for association "<< endmsg;
      } else {
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved " << 
	     m_trackCollection << endmsg;

         Rec::TrackParticleContainer::const_iterator track;
         for(track=tracks->begin();track!=tracks->end();++track) {
	    const Trk::Perigee *perigee = (*track)->perigee();
//          if(perigee == 0) continue; // not skip ! need to keep order for index !
            m_perigeeVector.push_back( perigee ); // this perigee match works !
         }
      }
    }else{ // it's a Trk::Tracks collection
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " User selected a Trk::Track collection ! " << endmsg; 

      if (evtStore()->retrieve(trktracks, m_trackCollection).isFailure()){
        if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Unable to retrieve track collection"
                                                  << m_trackCollection << " for association "<< endmsg;
      } else {
        if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Retrieved " << 
	    m_trackCollection << endmsg;
        TrackCollection::const_iterator track;
        for(track=trktracks->begin();track!=trktracks->end();++track) {
          const Trk::Perigee* trackPerigee = (*track)->perigeeParameters();
          const Trk::Perigee *perigee = dynamic_cast<const Trk::Perigee*>( trackPerigee );
//          if(perigee == 0) continue;
          m_perigeeVector.push_back( perigee ); 
        }
      }
    }
    return StatusCode::SUCCESS;
  }
   
  /**
   * For each segement collections retrieve all data
   * - loop over segments in all collections
   * - for each segment get basic coordinates
   * - for each segment try to obtain hits and add as multiple collection
   * @param FormatTool the tool that will create formated output from the DataMap
   */
  StatusCode JetRecJetRetriever::retrieve(ToolHandle<IFormatTool> &FormatTool) {
    
    ATH_MSG_DEBUG( "in retrieveAll()" );
    
    const DataHandle<JetCollection> iterator, end;
    const JetCollection* jets;

    //obtain the default collection first
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << m_sgKeyFavourite << ")" << endmsg;
    StatusCode sc = evtStore()->retrieve(jets, m_sgKeyFavourite);
    if (sc.isFailure() ) {
      if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg; 
    }else{
      DataMap data = getData(jets);
      if ( FormatTool->AddToEvent(dataTypeName(), m_sgKeyFavourite, &data).isFailure()){
       if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << m_sgKeyFavourite << " not found in SG " << endmsg;
      }else{
         if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << dataTypeName() << " (" << m_sgKeyFavourite << ") retrieved" << endmsg;
      }
    }

    if ( m_otherKeys.empty() ) {
      //obtain all other collections from StoreGate
      if (( evtStore()->retrieve(iterator, end)).isFailure()){
         if (msgLvl(MSG::WARNING)) msg(MSG::WARNING)  << "Unable to retrieve iterator for Jet collection" << endmsg;
//        return false;
      }
      
      for (; iterator!=end; iterator++) {

        std::string::size_type position = iterator.key().find("HLTAutoKey",0);
        if ( m_doWriteHLT ){ position = 99; } // override SG key find


        if ( position != 0 ){  // SG key doesn't contain HLTAutoKey         
         if (iterator.key()!=m_sgKeyFavourite) {
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG)  << "Trying to retrieve " << dataTypeName() << " (" << iterator.key() << ")" << endmsg;
             DataMap data = getData(iterator);
             if ( FormatTool->AddToEvent(dataTypeName(), iterator.key(), &data).isFailure()){
              if (msgLvl(MSG::WARNING)) msg(MSG::WARNING) << "Collection " << iterator.key() << " not found in SG " << endmsg;
           }else{
             if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << dataTypeName() << " (" << iterator.key() << ") retrieved" << endmsg;
           }
          }
       }
      }
    }else {
      //obtain all collections with the given keys
      for (const auto & thisKey:m_otherKeys){
       if ( !evtStore()->contains<JetCollection>( thisKey ) ){ continue; } // skip if not in SG
       StatusCode sc = evtStore()->retrieve( jets, thisKey );
       if (!sc.isFailure()) {
          ATH_MSG_DEBUG( "Trying to retrieve " << dataTypeName() << " (" << thisKey << ")" );
          DataMap data = getData(jets);
          if ( FormatTool->AddToEvent(dataTypeName(), thisKey, &data).isFailure()){
           ATH_MSG_WARNING( "Collection " << thisKey << " not found in SG " );
         }else{
            ATH_MSG_DEBUG( dataTypeName() << " (" << thisKey << ") retrieved" );
         }
       }
      }
    }
    //All collections retrieved okay
    return StatusCode::SUCCESS;
  }

  //--------------------------------------------------------------------------

  const DataMap JetRecJetRetriever::getData(const JetCollection* jets) {
    
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "retrieve()" << endmsg;

    DataMap dataMap;

    DataVect phi; phi.reserve(jets->size());
    DataVect eta; eta.reserve(jets->size());
    DataVect et; et.reserve(jets->size());
    DataVect mass; mass.reserve(jets->size());

    DataVect energy; energy.reserve(jets->size());
    DataVect px; px.reserve(jets->size());
    DataVect py; py.reserve(jets->size());
    DataVect pz; pz.reserve(jets->size());

    DataVect trackKeyVec; trackKeyVec.reserve(jets->size());
    DataVect trackIndexVec; trackIndexVec.reserve(jets->size());
    DataVect trackLinkCount; trackLinkCount.reserve(jets->size());

    DataVect bTagName; bTagName.reserve(jets->size());
    DataVect bTagValue; bTagValue.reserve(jets->size());
    
    DataVect numCells; numCells.reserve(jets->size());

    DataVect quality; quality.reserve(jets->size());
    DataVect qualityLAr; qualityLAr.reserve(jets->size());
    DataVect qualityTile; qualityTile.reserve(jets->size());
    DataVect time; time.reserve(jets->size());
    DataVect timeClusters; timeClusters.reserve(jets->size());
    DataVect n90cells; n90cells.reserve(jets->size());
    DataVect n90const; n90const.reserve(jets->size());
    DataVect hecf; hecf.reserve(jets->size());
    DataVect emfrac; emfrac.reserve(jets->size());
    DataVect tileGap3f; tileGap3f.reserve(jets->size());
    DataVect fcorCell; fcorCell.reserve(jets->size());
    DataVect fcorDotx; fcorDotx.reserve(jets->size());
    DataVect fcorJet; fcorJet.reserve(jets->size());
    DataVect fcorJetForCell; fcorJetForCell.reserve(jets->size());
    DataVect nbadcells; nbadcells.reserve(jets->size());
    DataVect fracSamplingMax; fracSamplingMax.reserve(jets->size());
    DataVect sMax; sMax.reserve(jets->size());
    DataVect OutOfTimeEfrac; OutOfTimeEfrac.reserve(jets->size());
    DataVect isGood; isGood.reserve(jets->size());
    DataVect isBad; isBad.reserve(jets->size());
    DataVect isUgly; isUgly.reserve(jets->size());
    DataVect jvf; jvf.reserve(jets->size());

    JetCollection::const_iterator itr = jets->begin();
    DataVect cellVec;





    StatusCode sc = fillPerigeeList();
    if (!sc.isFailure()) {
       if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Perigee list filled with " << m_perigeeVector.size()
                        << " entries " << endmsg;
    }    

    for (; itr != jets->end(); ++itr) {

/// jet to track association, 
///    Reconstruction/Jet/JetExamples/JetTests.cxx  from l.411

    int countPerigee=0, countMatches=0;
    const Analysis::TrackAssociation* trackAssoc = (*itr)->getAssociation<Analysis::TrackAssociation>("Tracks");
    if ( trackAssoc ){
     std::vector<const Rec::TrackParticle*>* trackVector = trackAssoc->tracks();
     if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << " tracks vector size: " <<  trackVector->size() << endmsg;
     for(std::vector<const Rec::TrackParticle*>::iterator trkItr = trackVector->begin();
      trkItr !=trackVector->end(); ++trkItr) {
       const Rec::TrackParticle* aTemp = *trkItr;      
       const Trk::Perigee* allPer = NULL;
       double pt=0.;
       if ( !aTemp ) continue; // skip to next if track not valid 

       if ((aTemp->perigee()->parameters())[Trk::qOverP]==0) {
           pt=9999.;
       } else {
           pt=(aTemp->perigee()->charge() * aTemp->perigee()->pT()/1000.);
       }

       if (msgLvl(MSG::DEBUG)){ msg(MSG::DEBUG) << "associated track d0= " << 
        aTemp->perigee()->parameters()[Trk::d0]/CLHEP::cm << ", z0= " <<
        aTemp->perigee()->parameters()[Trk::z0]/CLHEP::cm << ", phi0= " <<
        aTemp->perigee()->parameters()[Trk::phi0/CLHEP::cm] << ", pt= " <<
        pt << endmsg; }

       for (unsigned int i=0; i<m_perigeeVector.size(); i++) {

       if ( m_perigeeVector[i] ){
// manual match works for both track types !
         bool permatched = false;
         allPer = m_perigeeVector[i];
         manualPerigeeMatch(allPer,aTemp->perigee(), permatched);
       
         if ( permatched ){
           countPerigee++;
           countMatches++;
           if (msgLvl(MSG::DEBUG)){ msg(MSG::DEBUG) << " Track perigee match at index " << i 
	      << ", count is " << countPerigee << ", collection: " << m_trackCollection << endmsg; }
           trackIndexVec.push_back(DataType( i ));
           trackKeyVec.push_back(DataType( m_trackCollection ));
	 } //perigee match
       }// perigee exists condition
      } // perigeevector loop
    } //end trackVector loop
   }//end trackAssoc valid

// end of pasted snippet

      trackLinkCount.push_back(DataType( countMatches ));
      phi.push_back(DataType((*itr)->phi()));
      eta.push_back(DataType((*itr)->eta()));
      et.push_back(DataType((*itr)->et() /1000.));
      mass.push_back(DataType((*itr)->m()/1000.0));

      energy.push_back( DataType((*itr)->e()/1000.0 ) );
      px.push_back( DataType((*itr)->px()/1000.0 ) );
      py.push_back( DataType((*itr)->py()/1000.0 ) );
      pz.push_back( DataType((*itr)->pz()/1000.0 ) );

//from: http://alxr.usatlas.bnl.gov/lxr/source/atlas/PhysicsAnalysis/AnalysisCommon/AnalysisExamples/src/JetTagAna.cxx
//
      // bjet tagger values
      bTagName.push_back( DataType( "JetFitterCOMBNN" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("JetFitterCOMBNN") ));
      bTagName.push_back( DataType( "JetFitterTagNN" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("JetFitterTagNN") ));
      bTagName.push_back( DataType( "IP3D+SV1" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight() ));
      bTagName.push_back( DataType( "IP2D" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("IP2D") ));
      bTagName.push_back( DataType( "IP3D" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("IP3D") ));
      bTagName.push_back( DataType( "SV1" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("SV1") ));
      bTagName.push_back( DataType( "SV2" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("SV2") ));
      bTagName.push_back( DataType( "MV1" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("MV1") ));
      bTagName.push_back( DataType( "MV2" ));
      bTagValue.push_back( DataType( (*itr)->getFlavourTagWeight("MV2") ));
      
      NavigationToken<CaloCell>* cellToken = new NavigationToken<CaloCell>();
      (*itr)->fillToken(*cellToken);
      NavigationToken<CaloCell>::tokenConstIterator 
       cbeg = cellToken->firstMappedObject();
      NavigationToken<CaloCell>::tokenConstIterator 
       cend = cellToken->lastMappedObject();
      
      // Loop over cells
      int noCell = 0;
      for(; cbeg != cend; ++cbeg)
       {
         // pick up cell and kinematic weight
         const CaloCell* thisCell = (*cbeg).first;
	 // for 64bit identifiers:
	 cellVec.push_back( DataType( thisCell->ID().get_identifier32().get_compact() ));
         ++noCell;
       }
      numCells.push_back(DataType(noCell));
      if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Number of cells: " << noCell << endmsg;

      // basic jet quality 
      quality.push_back(DataType((*itr)->getMoment("LArQuality")));

      isGood.push_back(DataType( 1 ));  // placeholder
      isBad.push_back(DataType( 0 ));  // placeholder
      isUgly.push_back(DataType( 0 ));  // placeholder
      emfrac.push_back(DataType( 0.5 )); // placeholder



     jvf.push_back(DataType((*itr)->getMoment("JVF")));

     m_writeJetQuality = false; // over-write flag. JetQuality broken after xAOD migration


    } // end jet loop

    // calculate cell multiple and create multiple string
    double cellMult = 0.;
    std::string cellMultStr; 
    if ( phi.size()){
      cellMult = (cellVec.size()/( phi.size()*1.0));       
      cellMultStr = DataType( cellMult ).toString();
    }else{
      cellMultStr = "1.0";
    }
    dataMap["cells multiple=\""+cellMultStr+"\""] = cellVec;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "Cells multiple: " << cellMultStr << endmsg;

    // calculate track multiple and create multiple string
    double trackMult = 0.;
    std::string trackMultStr; 
    if ( phi.size()){
      trackMult = (trackIndexVec.size()/( phi.size()*1.0));       
      trackMultStr = DataType( trackMult ).toString();
    }else{
      trackMultStr = "1.0";
    }
    dataMap["trackIndex multiple=\""+trackMultStr+"\""] = trackIndexVec;
    dataMap["trackKey multiple=\""+trackMultStr+"\""] = trackKeyVec;
    if (msgLvl(MSG::DEBUG)) msg(MSG::DEBUG) << "tracks multiple: " << trackMultStr << endmsg;


    // Start with mandatory entries
    dataMap["phi"] = phi;
    dataMap["eta"] = eta;
    dataMap["et"] = et;    
    dataMap["trackLinkCount"] = trackLinkCount;

    dataMap["bTagName multiple=\"9\""] = bTagName; // assigned by hand !
    dataMap["bTagValue multiple=\"9\""] = bTagValue;
    
    // energy not yet in AtlantisJava, jpt 3Sep11
    //dataMap["energy"] = energy;
    dataMap["mass"] = mass;
    dataMap["px"] = px;
    dataMap["py"] = py;
    dataMap["pz"] = pz;

    // basic jet quality
    dataMap["quality"] = quality;
    dataMap["isGood"] = isGood;
    dataMap["isBad"] = isBad;
    dataMap["isUgly"] = isUgly;
    dataMap["emfrac"] = emfrac;
    dataMap["jvf"] = jvf;
    dataMap["numCells"] = numCells;

    if ( m_writeJetQuality ){ // extended jet quality
      dataMap["qualityLAr"] = qualityLAr;
      dataMap["qualityTile"] = qualityTile;
      dataMap["time"] = time;
      dataMap["timeClusters"] = timeClusters;
      dataMap["n90cells"] = n90cells;
      dataMap["n90const"] = n90const;
      dataMap["hecf"] = hecf;
      dataMap["tileGap3f"] = tileGap3f;
      dataMap["fcorCell"] = fcorCell;
      dataMap["fcorDotx"] = fcorDotx;
      dataMap["fcorJet"] = fcorJet;
      dataMap["fcorJetForCell"] = fcorJetForCell;
      dataMap["nbadcells"] = nbadcells;
      dataMap["fracSamplingMax"] = fracSamplingMax;
      dataMap["sMax"] = sMax;
      dataMap["OutOfTimeEfrac"] = OutOfTimeEfrac;
    } // extended jet quality

    //Be verbose
    if (msgLvl(MSG::DEBUG)) {
      msg(MSG::DEBUG) << dataTypeName() << " (ESD, with cells), collection: " << dataTypeName();
      msg(MSG::DEBUG) << " retrieved with " << phi.size() << " entries"<< endmsg;
    }

    //All collections retrieved okay
    return dataMap;

  } // end getData

  //--------------------------------------------------------------------------

} // JiveXML namespace
