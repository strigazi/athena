/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef FORWARDDETECTORSD3PDMAKER_AFPSIDSIMHITCOLLECTIONFILLERTOOL_H
#define FORWARDDETECTORSD3PDMAKER_AFPSIDSIMHITCOLLECTIONFILLERTOOL_H

#include "AfpCommonHdr.h"
#include "AFP_SimEv/AFP_SIDSimHitCollection.h"

namespace D3PD
{
    typedef D3PD::BlockFillerTool<AFP_SIDSimHitCollection> AfpSIDSimHitCollectionFillerBase;

    class AfpSIDSimHitCollectionFillerTool : public AfpSIDSimHitCollectionFillerBase
    {

    public:
        AfpSIDSimHitCollectionFillerTool(const std::string& type, const std::string& name, const IInterface* parent);

        virtual StatusCode initialize();
        virtual StatusCode book();
        virtual StatusCode fill(const AFP_SIDSimHitCollection& DataCollection);

    private:
        void ClearData(const AFP_SIDSimHitCollection& DataCollection);

    private:
		int* m_pnTotHitsCnt;
		std::vector<int>* m_pvecHitID;
		std::vector<int>* m_pvecTrackID;
		std::vector<int>* m_pvecParticleEncoding;
		std::vector<float>* m_pvecKineticEnergy;
		std::vector<float>* m_pvecEnergyDeposit;
		std::vector<float>* m_pvecPreStepX;
		std::vector<float>* m_pvecPreStepY;
		std::vector<float>* m_pvecPreStepZ;
		std::vector<float>* m_pvecPostStepX;
		std::vector<float>* m_pvecPostStepY;
		std::vector<float>* m_pvecPostStepZ;
		std::vector<float>* m_pvecGlobalTime;
		std::vector<int>* m_pvecStationID;
		std::vector<int>* m_pvecDetectorID;
		std::vector<int>* m_pvecIsAuxVSID;
		std::vector<int>* m_pvecPixelRow;
		std::vector<int>* m_pvecPixelCol;
    };
}

#endif // FORWARDDETECTORSD3PDMAKER_AFPSIDSIMHITCOLLECTIONFILLERTOOL_H
