# Copyright (C) 1995-2017 CERN for the benefit of the ATLAS collaboration
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from TrigFTKHypo.TrigFTKHypoConf import TrigFTKAvailableHypo

class TrigFTKAvailable(TrigFTKAvailableHypo):
  def __init__(self, name="TrigFTKAvailable_Muon_FTK", type="muon"):

    TrigFTKAvailableHypo.__init__(self, name)
