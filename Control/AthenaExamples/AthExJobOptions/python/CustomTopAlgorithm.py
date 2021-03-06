# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

## @file CustomTopAlgorithm.py

# File: AthExJobOptions/CustomTopAlgorithm.py
# Author: Wim Lavrijsen (WLavrijsen@lbl.gov)

__version__ = '1.0.0'
__author__  = 'Wim Lavrijsen (WLavrijsen@lbl.gov)'

__all__ = [ 'CustomTopAlgorithm' ]


from .AthExJobOptionsConf import TopAlgorithm

## @class CustomTopAlgorithm
#  @brief Customizer class for TopAlgorithm, enabled through derivation
# bla bla bla
class CustomTopAlgorithm( TopAlgorithm ):
   __slots__ = []                  # enforce no new properties

   def __init__( self, name = 'CustomTopAlgorithm' ):       # sets default name
      super( CustomTopAlgorithm, self ).__init__( name )    # have to call base init

      self.RndmFactor = 101.       # allowed, NOT considered default

 # setDefaults is enforced to be a class method; because it is a class method,
 # the defaults can be savely queried by other tools, without touching any part
 # of the configuration as would happen otherwise
   def setDefaults( cls, handle ):
    # these values will override the C++ (i.e. developer) default values, not
    # any python (i.e. user) values
      handle.RndmFactor = 42.      # best, is considered default

    # make sure one tool is always selected
      if not hasattr( handle, 'TheTool' ):
         from .AthExJobOptionsConf import ConcreteTool
         handle.TheTool = ConcreteTool()
