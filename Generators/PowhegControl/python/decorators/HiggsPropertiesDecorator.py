# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl HiggsPropertiesDecorator
#  Powheg runcard decorator for Higgs mass/width
#
#  Authors: James Robinson  <james.robinson@cern.ch>

#! /usr/bin/env python
from .. import ATLASCommonParameters

class HiggsPropertiesDecorator(object) :

  ## Define decorator name string
  name = 'Higgs properties'

  def __init__( self, decorated ) :
    ## Attach decorations to Powheg configurable
    decorated.run_card_decorators.append( self )
    self.decorated = decorated

    self.decorated.add_parameter( 'mass_H', ATLASCommonParameters.mass_H,   default='{0}', desc='mass of Higgs boson in GeV', parameter='hmass' )
    self.decorated.add_parameter( 'width_H', ATLASCommonParameters.width_H, default='{0}', desc='width of Higgs boson in GeV', parameter='hwidth' )
