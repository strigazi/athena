"""Construct Overlay configuration flags

Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
"""

from AthenaConfiguration.AthConfigFlags import AthConfigFlags

def createOverlayConfigFlags():
    """Return an AthConfigFlags object with required flags"""
    flags = AthConfigFlags()
    # Data overlay flag
    flags.addFlag("Overlay.DataOverlay", False)
    # Overlay background StoreGate key prefix
    flags.addFlag("Overlay.BkgPrefix", "Bkg_")
    # Overlay signal StoreGate key prefix
    flags.addFlag("Overlay.SigPrefix", "Sig_")
    # Name of MC EvtStore to overlay
    flags.addFlag("Overlay.Legacy.EventStore", "BkgEvent_0_SG")
    return flags