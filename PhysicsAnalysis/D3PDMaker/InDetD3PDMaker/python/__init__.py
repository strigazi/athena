# Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration

import InDetD3PDMakerConf
for k, v in InDetD3PDMakerConf.__dict__.items():
    if k.startswith ('D3PD__'):
        globals()[k[6:]] = v

