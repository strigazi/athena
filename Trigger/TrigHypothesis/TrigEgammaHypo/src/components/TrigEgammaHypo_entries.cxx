#include "TrigEgammaHypo/TrigL2CaloHypo.h"
#include "TrigEgammaHypo/TrigL2ElectronFex.h"
#include "TrigEgammaHypo/TrigL2ElectronHypo.h"
#include "TrigEgammaHypo/TrigL2ElectronHypoMoni.h"
#include "TrigEgammaHypo/TrigL2PhotonFex.h"
#include "TrigEgammaHypo/TrigL2PhotonHypo.h"
#include "TrigEgammaHypo/TrigL2DielectronMassHypo.h"
#include "TrigEgammaHypo/TrigL2DielectronMassFex.h"
#include "TrigEgammaHypo/TrigEFDielectronMassHypo.h"
#include "TrigEgammaHypo/TrigEFDielectronMassFex.h"
#include "TrigEgammaHypo/TrigEFHadronicRecoil.h"
#include "TrigEgammaHypo/TrigEFCaloHypo.h"
#include "TrigEgammaHypo/TrigEFElectronHypo.h"
#include "TrigEgammaHypo/TrigEFPhotonHypo.h"
#include "TrigEgammaHypo/TrigEFTrackHypo.h"
#include "TrigEgammaHypo/TrigL2PhotonFex.h"
#include "TrigEgammaHypo/TrigL2CaloLayersHypo.h"

#include "GaudiKernel/DeclareFactoryEntries.h"


DECLARE_ALGORITHM_FACTORY( TrigL2CaloHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronFex )
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2ElectronHypoMoni )
DECLARE_ALGORITHM_FACTORY( TrigL2PhotonFex )
DECLARE_ALGORITHM_FACTORY( TrigL2PhotonHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DielectronMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2DielectronMassFex )
DECLARE_ALGORITHM_FACTORY( TrigEFDielectronMassHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFDielectronMassFex )
DECLARE_ALGORITHM_FACTORY( TrigEFHadronicRecoil )
DECLARE_ALGORITHM_FACTORY( TrigEFCaloHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFElectronHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFPhotonHypo )
DECLARE_ALGORITHM_FACTORY( TrigEFTrackHypo )
DECLARE_ALGORITHM_FACTORY( TrigL2CaloLayersHypo )

DECLARE_FACTORY_ENTRIES( TrigEgammaHypo ) {
    DECLARE_ALGORITHM( TrigL2CaloHypo )
    DECLARE_ALGORITHM( TrigL2ElectronFex )
    DECLARE_ALGORITHM( TrigL2ElectronHypo )
    DECLARE_ALGORITHM( TrigL2ElectronHypoMoni )
    DECLARE_ALGORITHM( TrigL2PhotonFex )
    DECLARE_ALGORITHM( TrigL2PhotonHypo )
    DECLARE_ALGORITHM( TrigL2DielectronMassHypo )
    DECLARE_ALGORITHM( TrigL2DielectronMassFex )
    DECLARE_ALGORITHM( TrigEFDielectronMassHypo )
    DECLARE_ALGORITHM( TrigEFDielectronMassFex )
    DECLARE_ALGORITHM( TrigEFHadronicRecoil )
    DECLARE_ALGORITHM( TrigEFCaloHypo )
    DECLARE_ALGORITHM( TrigEFElectronHypo )
    DECLARE_ALGORITHM( TrigEFPhotonHypo )
    DECLARE_ALGORITHM( TrigEFTrackHypo )
    DECLARE_ALGORITHM( TrigL2CaloLayersHypo )
}

