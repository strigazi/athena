# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration



def defineMenu():
    """
    Defines the following LlTopoFlags:
    
    algos   .... list of all algo names in the menu 

    """
    
    from TriggerMenu.l1topo.L1TopoFlags import L1TopoFlags
    from TriggerMenu.l1topo.TopoOutput import TopoOutput

    from . import Menu_MC_pp_v7
    Menu_MC_pp_v7.defineMenu()
    

    
    
