# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from TriggerMenuMT.HLTMenuConfig.Menu.ChainDictTools import splitChainDict
from TriggerMenuMT.HLTMenuConfig.MinBias.MinBiasChainConfiguration import MinBiasChainConfig as MinBiasChainConfig
from TriggerMenuMT.HLTMenuConfig.Menu.ChainMerging import mergeChainDefs


from AthenaCommon.Logging import logging
log = logging.getLogger( __name__ )
log.info("Importing %s",__name__)



def generateChainConfigs( chainDict ):
    
    listOfChainDicts = splitChainDict(chainDict)
    log.debug("Implement case for minbias chain with %d legs ",len(listOfChainDicts))

    listOfChainDefs = []

    for subChainDict in listOfChainDicts:

        MinBias = MinBiasChainConfig(subChainDict).assembleChain()

        listOfChainDefs += [MinBias]
        log.debug('length of chaindefs %s', len(listOfChainDefs) )


    if len(listOfChainDefs)>1:
        log.debug("Implement case for mulit-leg minbias chain")
        theChainDef = mergeChainDefs(listOfChainDefs, chainDict)
    else:
        theChainDef = listOfChainDefs[0]

    log.debug("theChainDef %s" , theChainDef)

    return theChainDef