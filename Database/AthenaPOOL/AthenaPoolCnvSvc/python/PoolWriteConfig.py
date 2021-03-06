"""Configuration for POOL file writing

Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
"""
from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
AthenaPoolCnvSvc=CompFactory.AthenaPoolCnvSvc


def PoolWriteCfg(flags, **kwargs):
    """Return ComponentAccumulator configured to Write POOL files"""
    # based on WriteAthenaPool._configureWriteAthenaPool
    acc = ComponentAccumulator()

    PoolAttributes = []
    # Switch off splitting by setting default SplitLevel to 0
    PoolAttributes += ["DEFAULT_SPLITLEVEL ='0'"]
    # Set as default the member-wise streaming, ROOT default
    PoolAttributes += ["STREAM_MEMBER_WISE = '1'"]

    # Increase default BasketSize to 32K, ROOT default (but overwritten by POOL)
    PoolAttributes += ["DEFAULT_BUFFERSIZE = '32000'"]

    # Turn off auto_flush for DataHeader container to avoid basket optimization
    PoolAttributes += ["ContainerName = 'POOLContainer(DataHeader)'; BRANCH_BASKET_SIZE = '256000'"]
    PoolAttributes += ["ContainerName = 'POOLContainerForm(DataHeaderForm)'; BRANCH_BASKET_SIZE = '1024000'"]
    PoolAttributes += ["ContainerName = 'TTree=POOLContainerForm(DataHeaderForm)'; CONTAINER_SPLITLEVEL = '99'"]
    
    # based on RecoUtils.py#0145
    if flags.Output.RDOFileName:
        PoolAttributes += ["DatabaseName = '" + flags.Output.RDOFileName + "'; COMPRESSION_ALGORITHM = '2'"]
        PoolAttributes += ["DatabaseName = '" + flags.Output.RDOFileName + "'; COMPRESSION_LEVEL = '1'"]
        PoolAttributes += ["DatabaseName = '" + flags.Output.RDOFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '1'"]

    if flags.Output.ESDFileName:
        PoolAttributes += ["DatabaseName = '" + flags.Output.ESDFileName + "'; COMPRESSION_ALGORITHM = '2'"]
        PoolAttributes += ["DatabaseName = '" + flags.Output.ESDFileName + "'; COMPRESSION_LEVEL = '1'"]
        # Optimize Basket Sizes to store data for 10 entries/events
        PoolAttributes += ["DatabaseName = '" + flags.Output.ESDFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '10'"]

    if flags.Output.AODFileName:
        PoolAttributes += ["DatabaseName = '" + flags.Output.AODFileName + "'; COMPRESSION_ALGORITHM = '2'"]
        PoolAttributes += ["DatabaseName = '" + flags.Output.AODFileName + "'; COMPRESSION_LEVEL = '1'"]
        # Optimize Basket Sizes to store data for 100 entries/events
        PoolAttributes += ["DatabaseName = '" + flags.Output.AODFileName + "'; ContainerName = 'TTree=CollectionTree'; TREE_AUTO_FLUSH = '100'"]
        
    kwargs.setdefault("PoolAttributes", PoolAttributes)

    acc.addService(AthenaPoolCnvSvc(**kwargs))
    return acc
