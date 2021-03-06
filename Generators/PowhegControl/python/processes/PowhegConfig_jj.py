# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

## @PowhegControl PowhegConfig_jj
#  Powheg configuration for jj subprocess
#
#  Authors: James Robinson  <james.robinson@cern.ch>
#           Daniel Hayden   <danhayden0@googlemail.com>
#           Stephen Bieniek <stephen.paul.bieniek@cern.ch>

#! /usr/bin/env python
from ..PowhegConfig_base import PowhegConfig_base

## Default Powheg configuration for jj generation
#
#  Create a full configurable with all available Powheg options
class PowhegConfig_jj(PowhegConfig_base) :

  def __init__( self, runArgs=None, opts=None ) :
    ## Constructor: set process-dependent executable path here
    super(PowhegConfig_jj, self).__init__( runArgs, opts )
    self._powheg_executable += '/Dijet/pwhg_main'

    ## Decorate with generic option sets
    self.add_parameter_set( 'extra tests' )
    self.add_parameter_set( 'fixed scale' )
    self.add_parameter_set( 'jacsing' )
    self.add_parameter_set( 'LHEv3' )
    self.add_parameter_set( 'v2' )

    ## Set optimised integration parameters
    self.itmx1    = 2
    self.itmx2    = 10
    self.ncall1   = 10000
    self.ncall2   = 200000
    self.nubound  = 150000
    self.xupbound = 10
    self.foldx    = 10
    self.foldy    = 10
    self.foldphi  = 10

    ## Override defaults
    self.bornktmin = 5.0
    self.minlo     = -1
    ## Fix problem with spikes in final observables: from Paolo Nason (private communication)
    self.doublefsr  = 1
    self.par_2gsupp = 5
    self.par_diexp  = 4
    self.par_dijexp = 4

    self.populate_default_strings()
