#include "../TrigHIJetBaseSubtraction.h"
#include "../TrigHIEFTrackHypo.h"
#include "../ETSumHypo.h"
#include "../TrigHIL2VtxMultHypo.h"
#include "../TrigEstimateUE.h"
#include "../VnHypo.h"
#include "../CreateHIUEEstimate.h"
#include "../HICaloCellCorectionTool.h"

#include "GaudiKernel/DeclareFactoryEntries.h"

DECLARE_ALGORITHM_FACTORY( TrigHIJetBaseSubtraction )
DECLARE_ALGORITHM_FACTORY( TrigHIEFTrackHypo )
DECLARE_ALGORITHM_FACTORY( ETSumHypo )
DECLARE_ALGORITHM_FACTORY( TrigHIL2VtxMultHypo )
DECLARE_ALGORITHM_FACTORY( TrigEstimateUE )
DECLARE_ALGORITHM_FACTORY( VnHypo )
DECLARE_ALGORITHM_FACTORY( CreateHIUEEstimate )
DECLARE_TOOL_FACTORY( HICaloCellCorectionTool )

DECLARE_FACTORY_ENTRIES( TrigHIHypo ) {
  DECLARE_ALGORITHM( TrigHIJetBaseSubtraction )
  DECLARE_ALGORITHM( TrigHIEFTrackHypo )
  DECLARE_ALGORITHM( ETSumHypo )
  DECLARE_ALGORITHM( TrigHIL2VtxMultHypo )
  DECLARE_ALGORITHM( TrigEstimateUE )
  DECLARE_ALGORITHM( VnHypo )
  DECLARE_ALGORITHM( CreateHIUEEstimate )
  DECLARE_TOOL( HICaloCellCorectionTool )
}

