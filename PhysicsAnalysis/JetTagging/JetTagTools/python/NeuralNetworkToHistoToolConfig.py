# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator

# import the NeuralNetworkToHistoTool configurable
from TrkNeuralNetworkUtils.TrkNeuralNetworkUtilsConf import Trk__NeuralNetworkToHistoTool

def NeuralNetworkToHistoToolCfg( name = 'NeuralNetworkToHistoToolNN', useBTagFlagsDefaults = True, **options ):
    """Sets up a NeuralNetworkToHistoTool tool and returns it.
    input:             name: The name of the tool (should be unique).
                  **options: Python dictionary with options for the tool.
    output: The actual tool."""
    acc = ComponentAccumulator()
    options['name'] = name
    acc.setPrivateTools(Trk__NeuralNetworkToHistoTool( **options))

    return acc
