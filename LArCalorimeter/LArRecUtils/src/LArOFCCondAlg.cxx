/*
   Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
   */

#include <cmath>

//FIXME
//are these threadsafe?
#include "CaloDetDescr/CaloDetDescrManager.h"
#include "CaloDetDescr/CaloDetDescrElement.h"
#include "CaloIdentifier/CaloCell_ID.h"

#include <Eigen/Dense>

#include "LArOFCCondAlg.h"

#include "LArElecCalib/LArConditionsException.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArIdentifier/LArOnline_SuperCellID.h"

#include "GaudiKernel/EventIDRange.h"

LArOFCCondAlg::LArOFCCondAlg(const std::string &name,
        ISvcLocator *pSvcLocator)
    : ::AthAlgorithm(name, pSvcLocator),
    m_LArOnOffIdMappingObjKey("LArOnOffIdMap"),
    m_LArShapeObjKey("LArShape"),
    m_LArNoiseObjKey("LArNoise"),
    m_LArPedestalObjKey("LArPedestal"),
    m_LArAutoCorrTotalObjKey("LArAutoCorrTotal"),
    m_LArOFCObjKey("LArOFC"),
    m_condSvc("CondSvc", name), m_Nminbias(0), m_isMC(true),
    m_isSuperCell(false), m_firstSample(0), m_useDelta(0),
    m_deltaBunch(1), m_useHighestGainAutoCorr(false), m_Dump(false) {
        declareProperty("LArOnOffIdMappingObjKey", m_LArOnOffIdMappingObjKey,
                "Key to read LArOnOffIdMapping object");
        declareProperty("LArShapeObjKey", m_LArShapeObjKey,
                "Key to read LArShape object");
        declareProperty("LArNoiseObjKey", m_LArNoiseObjKey,
                "Key to read LArNoise object");
        declareProperty("LArPedestalObjKey", m_LArPedestalObjKey,
                "Key to read LArPedestal object");
        declareProperty("LArAutoCorrTotalObjKey", m_LArAutoCorrTotalObjKey,
                "Key to read LArAutoCorrTotal object");
        declareProperty("LArOFCObjKey", m_LArOFCObjKey,
                "Key to write LArOFC object");
        declareProperty("Nminbias", m_Nminbias);
        declareProperty("isMC", m_isMC);
        declareProperty("isSuperCell", m_isSuperCell);
        declareProperty(
                "firstSample", m_firstSample,
                "First sample to use for in-time event on the full pulse shape");
        declareProperty("UseDelta",
                m_useDelta,
                "0 = not use Delta, 1 = only EMECIW/HEC/FCAL, 2 = all , 3 = only EMECIW/HEC/FCAL1+high eta FCAL2-3");
        declareProperty("deltaBunch", m_deltaBunch,
                "Delta between filled bunches in 25 ns units");
        declareProperty("useHighestGainAutoCorr",m_useHighestGainAutoCorr);
        declareProperty("DumpOFCCondAlg",m_Dump);
    }

LArOFCCondAlg::~LArOFCCondAlg() {}

StatusCode LArOFCCondAlg::initialize() {
    ATH_MSG_DEBUG("initialize " << name());

    // CondSvc
    ATH_CHECK(m_condSvc.retrieve());

    // ReadCondHandle initialization
    ATH_CHECK(m_LArShapeObjKey.initialize());

    ATH_CHECK(m_LArAutoCorrTotalObjKey.initialize());
    ATH_CHECK(m_LArOnOffIdMappingObjKey.initialize());

    //WriteHandle initialization 
    ATH_CHECK(m_LArOFCObjKey.initialize());

    if (m_isMC) {
        ATH_CHECK(m_LArNoiseObjKey.initialize());
    } else {
        ATH_CHECK(m_LArPedestalObjKey.initialize());
    }

    // WriteCondHandle initialization
    if (m_condSvc->regHandle(this, m_LArOFCObjKey).isFailure()) {
        ATH_MSG_ERROR("Unable to register WriteCondHandle "
                << m_LArOFCObjKey.fullKey() << " with CondSvc");
        return StatusCode::FAILURE;
    }

    // Number of gains (does this have to be in initialize now b/c of AthenaMT?)
    // Copied from LArADC2MeVCondAlg.cxx
    if (m_isSuperCell) {
        m_nGains = 1;
    } else {
        m_nGains = 3;
    }

    return StatusCode::SUCCESS;
}

StatusCode LArOFCCondAlg::execute() {

    // WriteHandle setup
    SG::WriteCondHandle<LArOFC> writeHandle(m_LArOFCObjKey);
    // So the following should not be called usually?!
    if (writeHandle.isValid()) {
        ATH_MSG_DEBUG(
                "CondHandle "
                << writeHandle.fullKey() << " is already valid.");
        return StatusCode::SUCCESS;
    }

    // Identifier helper
    // Copied from LArADC2MeVCondAlg.cxx
    const LArOnlineID_Base *larOnlineID = nullptr;
    if (m_isSuperCell) {
        const LArOnline_SuperCellID *scidhelper;
        ATH_CHECK(detStore()->retrieve(scidhelper, "LArOnline_SuperCellID"));
        larOnlineID = scidhelper; // cast to base-class
    } else {                    // regular cells
        const LArOnlineID *idhelper;
        ATH_CHECK(detStore()->retrieve(idhelper, "LArOnlineID"));
        larOnlineID = idhelper; // cast to base-class
    }

    // retrieve CaloDetDescrManager only for m_delta=3
    const CaloDetDescrManager_Base* caloDetDescrMan = nullptr;
    if (m_useDelta == 3 ){
        if ( m_isSuperCell ){
            const CaloSuperCellDetDescrManager* cc;
            ATH_CHECK(  detStore()->retrieve(cc) );
            caloDetDescrMan = (const CaloDetDescrManager_Base*) cc;
        }else{
            const CaloDetDescrManager* cc;
            ATH_CHECK(  detStore()->retrieve(cc) );
            caloDetDescrMan = (const CaloDetDescrManager_Base*) cc;
        }
    }  


    EventIDRange rangeMapping;
    // Mapping helper
    const LArOnOffIdMapping *larOnOffIdMapping = nullptr;
    SG::ReadCondHandle<LArOnOffIdMapping> larOnOffIdMappingHdl{
        m_LArOnOffIdMappingObjKey
    };
    larOnOffIdMapping = *larOnOffIdMappingHdl;
    if (larOnOffIdMapping == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArOnOffIdMapping object");
    } else if (!larOnOffIdMappingHdl.range(rangeMapping)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for LArOnOffIdMapping object with " <<  larOnOffIdMappingHdl.key());
        return StatusCode::FAILURE;
    }

    // Get pointers to inputs
    // Retrieve validity ranges and determine their intersection
    EventIDRange rangeShape, rangeAutoCorrTotal;

    SG::ReadCondHandle<ILArShape> ShapeHdl{ m_LArShapeObjKey };
    // FIXME: should check if handle is properly created and/or check if handle is
    // properly retrieved
    // operator star of a ReadCondHandle returns a const pointer to type T
    const ILArShape *larShape{ *ShapeHdl };
    if (larShape == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArShape object");
    } else if (!ShapeHdl.range(rangeShape)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << ShapeHdl.key());
    }

    SG::ReadCondHandle<LArAutoCorrTotal> AutoCorrTotalHdl{ m_LArAutoCorrTotalObjKey };
    const LArAutoCorrTotal *larAutoCorrTotal = nullptr;
    larAutoCorrTotal= *AutoCorrTotalHdl;
    if (larAutoCorrTotal == nullptr) {
        ATH_MSG_ERROR("Failed to retrieve LArADC2MeV object");
    } else if (!AutoCorrTotalHdl.range(rangeAutoCorrTotal)) {
        ATH_MSG_ERROR("Failed to retrieve validity range for " << AutoCorrTotalHdl.key());
    }

    // Determine intersection of the two required objects
    EventIDRange rangeIntersection =
        EventIDRange::intersect(rangeShape, rangeAutoCorrTotal);
    // if ( rangeIntersection.start() > rangeIntersection.stop() ) {
    // ATH_MSG_ERROR( "Invalid intersection range: " << rangeIntersection);
    // return StatusCode::FAILURE;
    //}

    // Consider the determinstic objects
    const ILArNoise *larNoise = nullptr;
    const ILArPedestal *larPedestal = nullptr;

    if (m_isMC) {
        EventIDRange rangeNoise;
        SG::ReadCondHandle<ILArNoise> NoiseHdl{ m_LArNoiseObjKey };
        larNoise = *NoiseHdl;
        if (larNoise == nullptr) {
            ATH_MSG_ERROR("Failed to retrieve object LArNoise");
        } else if (!NoiseHdl.range(rangeNoise)) {
            ATH_MSG_ERROR("Failed to retrieve validity range for "
                    << NoiseHdl.key());
        }
        rangeIntersection.intersect(rangeIntersection, rangeNoise);
    } else {
        EventIDRange rangePedestal;
        SG::ReadCondHandle<ILArPedestal> PedestalHdl{ m_LArPedestalObjKey };
        larPedestal = *PedestalHdl;
        if (larPedestal == nullptr) {
            ATH_MSG_ERROR("Failed to retrieve object LArPedestal");
        } else if (!PedestalHdl.range(rangePedestal)) {
            ATH_MSG_ERROR("Failed to retrieve validity range for "
                    << PedestalHdl.key());
        }
        rangeIntersection.intersect(rangeIntersection, rangePedestal);
    }

    // Check sanity of final range
    if (rangeIntersection.start() > rangeIntersection.stop()) {
        ATH_MSG_ERROR("Invalid intersection range: " << rangeIntersection);
        return StatusCode::FAILURE;
    }

    ATH_MSG_INFO("IOV found from intersection for AutoCorrTotal object: "
            << rangeIntersection);

    // make output object
    // dimensions: number of gains x number of channel IDs x elements of
    // OFC
    std::unique_ptr<LArOFC> larOFC =
        std::make_unique<LArOFC>(larOnlineID, larOnOffIdMapping, m_nGains);


    ///////////////////////////////////////////////////
    std::vector<HWIdentifier>::const_iterator it = larOnlineID->channel_begin();
    std::vector<HWIdentifier>::const_iterator it_e = larOnlineID->channel_end();
    int count = 0;
    int count2 = 0;

    for (; it != it_e; ++it) {
        count++;
        const HWIdentifier chid = *it;
        const IdentifierHash hid = larOnlineID->channel_Hash(chid);

        //if (!(larOnOffIdMapping->isOnlineConnected(chid))) continue;
        if (larOnOffIdMapping->isOnlineConnected(chid)) {
            count2++;
            for (size_t igain = 0; igain < m_nGains; igain++) {

                bool thisChan_useDelta = false;

                std::vector<float> OFCa_tmp, OFCb_tmp;

                if (m_useDelta==2) {
                    thisChan_useDelta = true; 
                }
                else if (m_useDelta==1) { // only HEC/EMECIW/FCAL
                    if (larOnlineID->isEMECIW(chid) || larOnlineID->isFCALchannel(chid) || larOnlineID->isHECchannel(chid)) {
                        thisChan_useDelta = true; 
                    }
                }
                else if (m_useDelta==3) { // only HEC/EMECIW/FCAL1 and high eta FCAL2-3 
                    if (larOnlineID->isEMECIW(chid) ||  larOnlineID->isHECchannel(chid)) {
                        thisChan_useDelta = true; 
                    }
                    else if (larOnlineID->isFCALchannel(chid) && caloDetDescrMan) {       
                        Identifier ofl_id = larOnOffIdMapping->cnvToIdentifier(chid);
                        const CaloDetDescrElement* dde = caloDetDescrMan->get_element(ofl_id);
                        if (!dde) {
                            ATH_MSG_ERROR( " dde = 0 , onl_id, ofl_id= "<< chid << " "<< ofl_id  );
                            //return (m_OFCtmp);
                        }
                        CaloCell_ID::CaloSample sampling = dde->getSampling();
                        float eta = dde->eta();
                        if (sampling==CaloCell_ID::FCAL0){
                            thisChan_useDelta = true;
                        } else {
                            if (fabs(eta)>4.0) {
                                thisChan_useDelta = true;
                            }
                        }    
                    }     
                }

                //:::::::::::::::::::::::::::::::
                //retrieve the data
                //:::::::::::::::::::::::::::::::
                ILArShape::ShapeRef_t Shape = larShape->Shape(chid,igain);
                unsigned int nsamples_shape = Shape.size();
                ILArShape::ShapeRef_t ShapeDer = larShape->ShapeDer(chid,igain);
                //:::::::::::::::::::::::::::::::

                // get Noise autocorrelation for gain
                int igain_autocorr=igain;
                // to use only Autocorr fro highest gain in optimization: HEC/FCAL=> medium gain    EM=>high gain
                if (m_useHighestGainAutoCorr) {
                    if  (larOnlineID->isHECchannel(chid) || larOnlineID->isFCALchannel(chid) ) igain_autocorr=1;
                    else igain_autocorr=0;
                }

                const std::vector<double> AutoCorr = 
                    larAutoCorrTotal->autoCorrTotal(chid,igain_autocorr,m_Nminbias);
                //unsigned int nsamples_AC_OFC=AutoCorr.size()+1;
                unsigned int nsamples_AC_OFC = (1+((int)(sqrt(1+8*AutoCorr.size()))))/2;

                const std::vector<double>& rmsSampl =
                    larAutoCorrTotal->samplRMS(chid,igain_autocorr,m_Nminbias);
                unsigned int nsamples2 = rmsSampl.size();
                if (nsamples2 != nsamples_AC_OFC) {
                    ATH_MSG_WARNING( " bad size for rmsSampl "  );
                    //return (m_OFCtmp);  // return empty vector
                }
                //:::::::::::::::::::::::::::::::
                //unsigned int iBeginOfNSamples=findTheNSamples(Shape,
                //						    nsamples_AC_OFC,
                //						    nsamples_shape); 
                unsigned int firstSample = m_firstSample; 
                if(larOnlineID->isHECchannel(chid) && m_firstSample == 0 && nsamples_AC_OFC==4){ 
                    firstSample=1; 
                } 
                unsigned int iBeginOfNSamples = firstSample; 
                if(nsamples_AC_OFC + iBeginOfNSamples > nsamples_shape) 
                    iBeginOfNSamples=0;      
                //:::::::::::::::::::::::::::::::

                if(m_isMC) {
                }
                else          
                {	 
                    float RMSpedestal = larPedestal->pedestalRMS(chid,igain);
                    if(RMSpedestal>= (1.0+LArElecCalib::ERRORCODE))
                        ;
                    else
                    {
                        ATH_MSG_WARNING(" PedestalRMS vector empty for "
                                <<chid<<" at gain "<<igain );
                    }	
                }
                //:::::::::::::::::::::::::::::::
                //protection against missing data
                //:::::::::::::::::::::::::::::::
                if(Shape.size()==0 || ShapeDer.size()==0 || AutoCorr.size()==0)
                {
                    ATH_MSG_WARNING("Some data are missing -> OFC will be empty for "
                            <<chid<<" at gain "<<igain );
                    //return (m_OFCtmp);
                    //returns an empty vector
                }
                //:::::::::::::::::::::::::::::::
                unsigned int l,c,i; 
                //:::::::::::::::::::::::::::::::
                //calculations
                //:::::::::::::::::::::::::::::::
                // fill and inverrt AC matrix
                //HepMatrix AC(nsamples_AC_OFC,nsamples_AC_OFC),
                          //ACinv(nsamples_AC_OFC,nsamples_AC_OFC);    
                Eigen::MatrixXf AC = Eigen::MatrixXf::Zero(nsamples_AC_OFC,nsamples_AC_OFC);
                Eigen::MatrixXf ACinv = Eigen::MatrixXf::Zero(nsamples_AC_OFC,nsamples_AC_OFC);
                for(l=0;l<nsamples_AC_OFC;++l) {  //  l=line c=column      	
                    for(c=0;c<nsamples_AC_OFC;++c) {
                        if (l==c) {
                            AC(l,c)=1.;
                        }
                        else {
                            int i1=std::min(l,c);
                            int i2=std::max(l,c);
                            int index = i1*nsamples_AC_OFC - i1*(i1+1)/2 -(i1+1) + i2;
                            AC(l,c)=AutoCorr[index];
                        }
                        AC(l,c) = AC(l,c)*rmsSampl[l]*rmsSampl[c];
                    }
                }
                ACinv=AC.inverse();
                //:::::::::::::::::::::::::::::::           

                if (!thisChan_useDelta) { // STANDARD CALCULATION

                    float ACinv_PS[32];//ACinv_PS
                    float ACinv_PSD[32]; //ACinv_PSD
                    //Q1 Q2 Q3 DELTA
                    float Q1=0.;
                    float Q2=0.;
                    float Q3=0.;

                    for(l=0;l<nsamples_AC_OFC;++l)
                    {
                        ACinv_PS[l]=0.; ACinv_PSD[l]=0.;
                        for(c=0;c<nsamples_AC_OFC;++c){
                            ACinv_PS[l]+=ACinv(l,c)*Shape[c+iBeginOfNSamples];
                            ACinv_PSD[l]+=ACinv(l,c)*ShapeDer[c+iBeginOfNSamples];
                        }
                        Q1+=Shape[l+iBeginOfNSamples]*ACinv_PS[l];
                        Q2+=ShapeDer[l+iBeginOfNSamples]*ACinv_PSD[l];
                        Q3+=ShapeDer[l+iBeginOfNSamples]*ACinv_PS[l];
                    } 
                    float DELTA=Q1*Q2-Q3*Q3;  
                    //:::::::::::::::::::::::::::::::
                    //OFCa  
                    for(i=0;i<nsamples_AC_OFC;++i) 
                        OFCa_tmp.push_back( (ACinv_PS[i]*Q2-ACinv_PSD[i]*Q3)/DELTA );
                    //OFCb  
                    for(i=0;i<nsamples_AC_OFC;++i) 
                        OFCb_tmp.push_back( (ACinv_PS[i]*Q3-ACinv_PSD[i]*Q1)/DELTA ); 

                    //for debugging only
                    if(m_Dump)
                    { 
                        std::cout<<larOnlineID
                            ->show_to_string(larOnOffIdMapping->cnvToIdentifier(chid))
                            <<" gain="<<igain<<" Nminbias="<<m_Nminbias<<std::endl;
                        std::cout<<"Shape: ";
                        for(c=0;c<nsamples_shape;++c)
                            std::cout<<Shape[c]<<" ";
                        std::cout<<std::endl;
                        std::cout<<"ShapeDer: ";
                        for(c=0;c<nsamples_shape;++c)
                            std::cout<<ShapeDer[c]<<" ";
                        std::cout<<std::endl;
                        for(c=0;c<nsamples_AC_OFC;++c)
                            std::cout<<Shape[c+iBeginOfNSamples]<<" ";
                        std::cout<<" <- "<<iBeginOfNSamples<<std::endl;
                        for(i=0;i<nsamples_AC_OFC;++i) std::cout<<ACinv_PS[i]<<" ";
                        std::cout<<std::endl;
                        for(i=0;i<nsamples_AC_OFC;++i) std::cout<<ACinv_PSD[i]<<" ";
                        std::cout<<std::endl;
                        std::cout<<" Q1="<<Q1<<" Q2="<<Q2<<" Q3="<<Q3
                            <<" DELTA="<<DELTA<<std::endl;
                        std::cout << " OFCa: ";
                        for(i=0;i<nsamples_AC_OFC;++i) 
                            std::cout<<(ACinv_PS[i]*Q2-ACinv_PSD[i]*Q3)/DELTA<<" ";
                        std::cout<<std::endl;
                    }
                } else { // OPTIMIZATION WRT NOISE AND PEDESTAL SHIFTS
                    ATH_MSG_DEBUG( " Computing pulse averages for " 
                            << chid << " at gain " << igain );

                    std::vector<float> averages = getShapeAverages(nsamples_AC_OFC,m_deltaBunch,Shape.asVector(),firstSample);

                    // Fill shape, derivative and delta vectors as HepVector
                    //HepVector gResp(nsamples_AC_OFC);
                    //HepVector gDerivResp(nsamples_AC_OFC);
                    //HepVector gDelta(nsamples_AC_OFC);
                    Eigen::VectorXf gResp = Eigen::VectorXf::Zero(nsamples_AC_OFC);
                    Eigen::VectorXf gDerivResp = Eigen::VectorXf::Zero(nsamples_AC_OFC);
                    Eigen::VectorXf gDelta = Eigen::VectorXf::Zero(nsamples_AC_OFC);
                    for(c=0;c<nsamples_AC_OFC;++c){
                        gResp(c)      = Shape[c+iBeginOfNSamples];
                        gDerivResp(c) = ShapeDer[c+iBeginOfNSamples];
                        gDelta(c)     = averages[c];
                    }

                    ATH_MSG_DEBUG( " Computing OFC optimized for noise and offsets for " 
                            << chid << " at gain " << igain );

                    //HepMatrix isol(3,3); 
                    Eigen::Matrix3f isol = Eigen::Matrix3f::Zero(); 

                    isol(0,0) = (gResp.transpose()*ACinv*gResp)(0);
                    isol(0,1) = (gResp.transpose()*ACinv*gDerivResp)(0);
                    isol(0,2) = (gResp.transpose()*ACinv*gDelta)(0);

                    isol(1,0) = (gDerivResp.transpose()*ACinv*gResp)(0);
                    isol(1,1) = (gDerivResp.transpose()*ACinv*gDerivResp)(0);
                    isol(1,2) = (gDerivResp.transpose()*ACinv*gDelta)(0);

                    isol(2,0) = (gDelta.transpose()*ACinv*gResp)(0);
                    isol(2,1) = (gDelta.transpose()*ACinv*gDerivResp)(0);
                    isol(2,2) = (gDelta.transpose()*ACinv*gDelta)(0);

                    //int ifail;
                    //HepMatrix isolInv = isol.inverse(ifail);
                    Eigen::Matrix3f isolInv = isol.inverse();
                    //if(ifail != 0) {
                    // do something 
                    //} 

                    //for debugging only
                    if(m_Dump)
                    {
                        std::cout<<larOnlineID
                            ->show_to_string(larOnOffIdMapping->cnvToIdentifier(chid))
                            <<" gain="<<igain<<" Nminbias="<<m_Nminbias<<std::endl;
                        std::cout<<"Shape: ";
                        for(c=0;c<nsamples_shape;++c)
                            std::cout<<Shape[c]<<" ";
                        std::cout<<std::endl;
                        std::cout<<"ShapeDer: ";
                        for(c=0;c<nsamples_shape;++c)
                            std::cout<<ShapeDer[c]<<" ";
                        std::cout<<std::endl;
                        std::cout << " Shape for the n samples ";
                        for(c=0;c<nsamples_AC_OFC;++c)
                            std::cout<<Shape[c+iBeginOfNSamples]<<" ";
                        std::cout<<" <- "<<iBeginOfNSamples<<std::endl;
                        std::cout << " averages: ";
                        for(i=0;i<nsamples_AC_OFC;++i)
                            std::cout<<averages[i]<< " ";
                        std::cout<<std::endl;
                    }


                    // OFCa 
                    {
                        //HepVector Amp(3); 
                        Eigen::Vector3f Amp = Eigen::Vector3f::Zero(); 
                        //HepVector Ktemp(3);
                        Eigen::Vector3f Ktemp = Eigen::Vector3f::Zero();
                        Ktemp(0) = 1.;
                        Ktemp(1) = 0.;
                        Ktemp(2) = 0.;
                        Amp = isolInv*Ktemp;
                        //HepVector OFCa_vec(nsamples_AC_OFC);
                        Eigen::VectorXf OFCa_vec = Eigen::VectorXf::Zero(nsamples_AC_OFC);
                        OFCa_vec = Amp(0)*ACinv*gResp + Amp(1)*ACinv*gDerivResp + Amp(2)*ACinv * gDelta;
                        for(i=0;i<nsamples_AC_OFC;++i) {
                            OFCa_tmp.push_back( OFCa_vec(i) );
                        }
                        if (m_Dump) {
                            std::cout << "OFCa: ";
                            for(i=0;i<nsamples_AC_OFC;++i) std::cout << OFCa_vec(i) << " ";
                            std::cout << std::endl;
                        }
                    }

                    // OFCb
                    {
                        //HepVector Atau(3);
                        Eigen::Vector3f Atau = Eigen::Vector3f::Zero();
                        //HepVector Ktemp(3);
                        Eigen::Vector3f Ktemp = Eigen::Vector3f::Zero();
                        Ktemp(0) = 0.; 
                        Ktemp(1) = -1.;
                        Ktemp(2) = 0.;
                        Atau = isolInv*Ktemp;
                        //HepVector OFCb_vec(nsamples_AC_OFC);
                        Eigen::VectorXf OFCb_vec = Eigen::VectorXf::Zero(nsamples_AC_OFC);
                        OFCb_vec = Atau(0)*ACinv*gResp + Atau(1)*ACinv*gDerivResp + Atau(2)*ACinv * gDelta  ;
                        for(i=0;i<nsamples_AC_OFC;++i) 
                            OFCb_tmp.push_back( OFCb_vec(i) );
                    }
                } // finish optimization wrt pedestal and noise
                bool stat = larOFC->setOFC(hid, igain, std::make_pair(OFCa_tmp, OFCb_tmp));
            	if (!stat) {
	                msg(MSG::ERROR) << "LArOFC::setOFC fails for gain " << igain << ", hash " << hid << endmsg;
                }
	     } // end loop over gains
        } else { // end loop over connected channels -- now, set empty for disc. chnanels
            for (unsigned igain=0;igain<m_nGains;++igain) {
                std::vector<float> empty;
                bool stat = larOFC->setOFC(hid,igain, std::make_pair(empty, empty));
            	if (!stat) {
	                msg(MSG::ERROR) << "LArOFC::setOFC fails for gain " << igain << ", hash " << hid << endmsg;
                }
            } // end loop over gains of disconnected channels
        } // end loop over disconnected channels

    } // end loop over all channels


    ATH_CHECK(writeHandle.record(rangeIntersection,larOFC.release()));
    ATH_MSG_INFO("Wrote LArOFC obj to CondStore");
    return StatusCode::SUCCESS;
}

std::vector<float> LArOFCCondAlg::getShapeAverages( const unsigned n_samples, 
						 const unsigned n_deltaBunch, // in unit of 25 ns
						 const std::vector<float>& shape, unsigned int firstSample ) const
{
  std::vector<float> averages(n_samples);
  for (unsigned int i=0;i<n_samples;++i) {
    float sumShape = 0.;
    for (unsigned int j=0;j<shape.size();++j) {
      int k=i-j+firstSample;
      if (k%n_deltaBunch == 0 ) sumShape += shape[j];
    }
    averages[i] = sumShape;
  }
  return averages;
}

unsigned int LArOFCCondAlg::findTheNSamples(ILArShape::ShapeRef_t Shape,
					 unsigned int nsamples_AC_OFC,
					 unsigned int nsamples_shape) const
{
  unsigned int  i_ShapeMax=0;
  double ShapeMax=0;
  for(unsigned int i=0;i<nsamples_shape;++i)
  {
    double value=Shape[i];
    if(value>ShapeMax) { ShapeMax=value; i_ShapeMax=i; }
    else if(value<0 && i>3) break;//after the peak  
  }
  
  unsigned int tmp=int(nsamples_AC_OFC/2.);
  if(tmp>i_ShapeMax) return 0;
  else               return i_ShapeMax-tmp;
}
