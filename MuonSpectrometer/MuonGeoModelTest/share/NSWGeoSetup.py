# Select geometry version
if not 'DetDescrVersion' in dir():
    DetDescrVersion="ATLAS-R2-2015-03-01-00"
    #DetDescrVersion="ATLAS-GEOTEST"
    printfunc ('MuonGeoModelTest/NSWGeoSetup DetDescrVersion now set to ', DetDescrVersion)
else:
    printfunc ('MuonGeoModelTest/NSWGeoSetup DetDescrVersion is already ', DetDescrVersion)
 

from AthenaCommon.JobProperties import jobproperties
jobproperties.Global.DetDescrVersion = DetDescrVersion

from AtlasGeoModel import SetGeometryVersion
from AtlasGeoModel import GeoModelInit
######################################################################## end setup GeoModel

#***************************************************** HERE override MuonSpectrometer tag
from AtlasGeoModel import SetGeometryVersion
from GeoModelSvc.GeoModelSvcConf import GeoModelSvc
GeoModelSvc = GeoModelSvc()
if DetDescrVersion=="ATLAS-GEOTEST" or DetDescrVersion=="ATLAS-GEO-21-00-01":
    # do nothing
    printfunc ('NSWGeoSetup: no need to override tag on the MuonSpectrometer Node with ATLAS tag ', DetDescrVersion )
    printfunc ('Be aware that the job is running with NSW TDR setup!')
else:
    GeoModelSvc.MuonVersionOverride="MuonSpectrometer-R.07.00-NSW"
printfunc (GeoModelSvc)

#***************************************************** HERE setup MuonDetectorManager
from MuonGeoModel.MuonGeoModelConf import MuonDetectorTool
MuonDetectorTool = MuonDetectorTool()
MuonDetectorTool.StationSelection  = 2
MuonDetectorTool.SelectedStations  = [ "EIL1" ]
MuonDetectorTool.SelectedStations  += [ "EIL2" ]
MuonDetectorTool.SelectedStations  += [ "EIS*" ]
MuonDetectorTool.SelectedStations  += [ "EIL10" ]
MuonDetectorTool.SelectedStations  += [ "EIL11" ]
MuonDetectorTool.SelectedStations  += [ "CSS*" ]
MuonDetectorTool.SelectedStations  += [ "CSL*" ]
MuonDetectorTool.SelectedStations  += [ "T4E*" ]
MuonDetectorTool.SelectedStations  += [ "T4F*" ]

from AGDD2GeoSvc.AGDD2GeoSvcConf import AGDDtoGeoSvc
Agdd2GeoSvc = AGDDtoGeoSvc()

if not "NSWAGDDTool/NewSmallWheel" in Agdd2GeoSvc.Builders.__str__():
    from AthenaCommon import CfgGetter
    Agdd2GeoSvc.Builders += [CfgGetter.getPrivateTool("NewSmallWheel", checkType=True)]

theApp.CreateSvc += ["AGDDtoGeoSvc"]
ServiceMgr += Agdd2GeoSvc
