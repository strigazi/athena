##Creation: George Redlinger & David Cote, September 2010 
##This file defines the SUSYEGAMMA stream and filters instances
from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#====================================================================
# Define the particle and event selections
#====================================================================
# from D2PDMaker.SUSYFilters import SetupEGammaStreamFilters
# acceptAlgs=SetupEGammaStreamFilters(topSequence)
from SUSYD3PDMaker.SUSYTriggerFilters import SetupSUSYTriggerFilters
acceptAlgs=SetupSUSYTriggerFilters(topSequence)


#====================================================================
# Define the test DPD output stream
#====================================================================
from OutputStreamAthenaPool.MultipleStreamManager import MSMgr 
from D2PDMaker.D2PDHelpers import buildFileName
from D2PDMaker.D2PDFlags import D2PDFlags
# This stream HAS TO start with "StreamD2AODM_"! If the input was an (D)ESD(M), this should start with "StreamD2ESD(M)_".
# See this twiki for more information: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DPDNamingConvention
fileName   = buildFileName( D2PDFlags.WriteDAOD_SUSYEGammaStream )
streamName = D2PDFlags.WriteDAOD_SUSYEGammaStream.StreamName
SUSYEGammaStream = MSMgr.NewPoolStream( streamName, fileName )

# Only events that pass the filters listed below are written out
# AcceptAlgs  = logical OR of filters
# RequireAlgs = logical AND of filters
SUSYEGammaStream.AcceptAlgs( acceptAlgs )


#---------------------------------------------------
# Add the containers to the output stream
#---------------------------------------------------
from PrimaryDPDMaker import PrimaryDPD_OutputDefinitions as dpdOutput
# Take all items from the input, except for the ones listed in the excludeList
# If the excludeList is empty, all containers from the input file (e.g. AOD)
# are copied to the output file.
excludeList = []
excludeList = list(set(excludeList)) # This removes dublicates from the list
dpdOutput.addAllItemsFromInputExceptExcludeList( streamName, excludeList )

# You need to add your newly created output containers from above to the output stream
SUSYEGammaStream.AddItem( ['CompositeParticleContainer#*'] )
SUSYEGammaStream.AddItem( ['INav4MomLinkContainer#*'] )




