# needed to get	the right dictionary from IdDictParser  
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00-NSW"

if 'ReadAGDD' in ToolSvc.NewSmallWheel.__slots__ : ToolSvc.NewSmallWheel.ReadAGDD=False
