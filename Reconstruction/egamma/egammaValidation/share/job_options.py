from AthenaCommon.AppMgr                    import theApp
from AthenaCommon.AppMgr                    import ServiceMgr as svcMgr
from AthenaCommon.AlgSequence               import AlgSequence
from AthenaMonitoring.GenericMonitoringTool import GenericMonitoringTool, defineHistogram
from GaudiSvc.GaudiSvcConf                  import THistSvc

import AthenaRootComps.ReadAthenaxAODHybrid

job = CfgMgr.AthSequencer("AthAlgSeq")
job += CfgMgr.ArtTest()

monTool = GenericMonitoringTool('MonTool')

monTool.Histograms = [defineHistogram('runNmb'      , path='EXPERT', type='TH1F', title='Run   Number'         , xbins=250, xmin=284000  , xmax=285000  ),
                      defineHistogram('evtNmb'      , path='EXPERT', type='TH1F', title='Event Number'         , xbins=250, xmin=33894000, xmax=33896000),
                      defineHistogram('TIME_execute', path='EXPERT', type='TH1F', title='Exec    Time'         , xbins=100, xmin=0       , xmax=100     ), 
                      defineHistogram('pT_ElTrk_All', path='EXPERT', type='TH1F', title='Electron Pt Track All', xbins=200, xmin=0       , xmax=200     ),
                      defineHistogram('pT_ElTrk_LLH', path='EXPERT', type='TH1F', title='Electron Pt Track LLH', xbins=200, xmin=0       , xmax=200     ),
                      defineHistogram('pT_ElTrk_MLH', path='EXPERT', type='TH1F', title='Electron Pt Track MLH', xbins=200, xmin=0       , xmax=200     ),
                      defineHistogram('pT_ElTrk_TLH', path='EXPERT', type='TH1F', title='Electron Pt Track TLH', xbins=200, xmin=0       , xmax=200     )]

job.ArtTest.MonTool = monTool

svcMgr += THistSvc(Output = ["EXPERT DATAFILE='ART-monitoring.root', OPT='RECREATE'"])

theApp.EvtMax = 2000

print ( "\n\nALL OK\n\n" )
