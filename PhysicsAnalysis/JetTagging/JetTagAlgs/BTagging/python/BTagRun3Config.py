# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

from AthenaConfiguration.ComponentAccumulator import ComponentAccumulator
from AthenaConfiguration.ComponentFactory import CompFactory
from IOVDbSvc.IOVDbSvcConfig import addFolders
from BTagging.JetBTaggerAlgConfig import JetBTaggerAlgCfg
from BTagging.JetParticleAssociationAlgConfig import JetParticleAssociationAlgCfg
from BTagging.JetBTaggingAlgConfig import JetBTaggingAlgCfg
from BTagging.JetSecVertexingAlgConfig import JetSecVertexingAlgCfg
from BTagging.JetSecVtxFindingAlgConfig import JetSecVtxFindingAlgCfg
from BTagging.BTagTrackAugmenterAlgConfig import BTagTrackAugmenterAlgCfg
from BTagging.BTagHighLevelAugmenterAlgConfig import BTagHighLevelAugmenterAlgCfg
from BTagging.HighLevelBTagAlgConfig import HighLevelBTagAlgCfg

def JetTagCalibCfg(ConfigFlags, scheme="", TaggerList = [], useBTagFlagsDefaults = True, **kwargs):
    result=ComponentAccumulator()

    CalibrationChannelAliases = [ "myOwnCollection->AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt4Tower->AntiKt4Tower,AntiKt4H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt4Topo->AntiKt4Topo,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4H1Topo",
                              "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4Topo,AntiKt4H1Topo",
                              "AntiKt6Tower->AntiKt6Tower,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt6Topo->AntiKt6Topo,AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt6LCTopo->AntiKt6LCTopo,AntiKt6TopoEM,AntiKt6Topo,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt4TopoEM->AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4H1Topo,AntiKt4LCTopo",
                              "AntiKt6TopoEM->AntiKt6TopoEM,AntiKt6H1Topo,AntiKt6H1Tower,AntiKt4TopoEM,AntiKt4EMTopo",
                              #WOUTER: I added some more aliases here that were previously set up at ./python/BTagging_jobOptions.py. But it cannot
                              #stay there if we want support for JetRec to setup b-tagging from their end.
                              "AntiKt4EMTopo->AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                              "AntiKt4LCTopo->AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt4EMTopoOrigin->AntiKt4EMTopoOrigin,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                              "AntiKt4LCTopoOrigin->AntiKt4LCTopoOrigin,AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt10LCTopo->AntiKt10LCTopo,AntiKt6LCTopo,AntiKt6TopoEM,AntiKt4LCTopo,AntiKt4TopoEM,AntiKt4EMTopo",
                              "AntiKt10Truth->AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt10TruthWZ->AntiKt10TruthWZ,AntiKt6TopoEM,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt4Truth->AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt4TruthWZ->AntiKt4TruthWZ,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt4Track->AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt3Track->AntiKt3Track,AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt2Track->AntiKt2Track,AntiKt4Track,AntiKt4TopoEM,AntiKt4EMTopo,AntiKt4LCTopo",
                              "AntiKt4EMPFlow->AntiKt4EMPFlow,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo",
                              "AntiKt4HI->AntiKt4HI,AntiKt4EMTopo,AntiKt4TopoEM,AntiKt4LCTopo"]

    newChannel = kwargs.get("NewChannel")
    if newChannel:
        CalibrationChannelAliases.append(newChannel)
       
    if useBTagFlagsDefaults:
      #IP2D
      grades= ConfigFlags.BTagging.Grades

      #IP3D
      #Same as IP2D. Revisit JetTagCalibCondAlg.cxx if not.

      #RNNIP
      RNNIPConfig = {'rnnip':''}

      JetTagCalibCondAlg=CompFactory.Analysis.JetTagCalibCondAlg
      jettagcalibcondalg = "JetTagCalibCondAlg"
      readkeycalibpath = "/GLOBAL/BTagCalib/RUN12"
      connSchema = "GLOBAL_OFL"
      if not ConfigFlags.Input.isMC:
          readkeycalibpath = readkeycalibpath.replace("/GLOBAL/BTagCalib","/GLOBAL/Onl/BTagCalib")
          connSchema = "GLOBAL"
      histoskey = "JetTagCalibHistosKey"
      result.merge(addFolders(ConfigFlags,[readkeycalibpath], connSchema, className='CondAttrListCollection'))
      JetTagCalib = JetTagCalibCondAlg(jettagcalibcondalg, ReadKeyCalibPath=readkeycalibpath, HistosKey = histoskey, taggers = TaggerList, channelAliases = CalibrationChannelAliases, IP2D_TrackGradePartitions = grades, RNNIP_NetworkConfig = RNNIPConfig)
  # Maybe needed for trigger use
  #from IOVDbSvc.CondDB import conddb
  #if conddb.dbdata == 'COMP200':
  #  conddb.addFolder("GLOBAL_ONL", "/GLOBAL/Onl/BTagCalib/RUN12", className='CondAttrListCollection')
  #  if globalflags.DataSource()!='data':
  #    conddb.addFolder("GLOBAL_ONL", "/GLOBAL/Onl/TrigBTagCalib/RUN12", className='CondAttrListCollection')
  #elif conddb.isMC:
  #  conddb.addFolder("GLOBAL_OFL", "/GLOBAL/BTagCalib/RUN12", className='CondAttrListCollection')
  #  conddb.addFolder("GLOBAL_OFL", "/GLOBAL/TrigBTagCalib/RUN12", className='CondAttrListCollection')

    result.addCondAlgo(JetTagCalib)

    return result

def registerJetCollectionEL(flags, JetCollection, TimeStamp):
    ItemList = []
    # btaggingLink
    suffix = ".".join(['btaggingLink'+ ts for ts in TimeStamp])
    ItemList.append('xAOD::JetContainer#'+JetCollection+'Jets')
    ItemList.append('xAOD::JetAuxContainer#'+JetCollection+'JetsAux.'+ suffix)

    return ItemList

def registerOutputBTaggingContainers(flags, JetCollection, suffix = ''):
      """Registers the jet collection to various containers in BTaggingFlags which govern which
      containers will be parsed to the output xAOD and ESD files. This used to happen in
      ./share/BTagging_jobOptions.py.

      input: JetCollection:       The name of the jet collection."""
      ItemList = []

      OutputFilesSVname = "SecVtx"
      OutputFilesJFVxname = "JFVtx"
      OutputFilesBaseName = "xAOD::BTaggingContainer#"
      OutputFilesBaseAuxName = "xAOD::BTaggingAuxContainer#"
      OutputFilesBaseNameSecVtx = "xAOD::VertexContainer#"
      OutputFilesBaseAuxNameSecVtx = "xAOD::VertexAuxContainer#"
      OutputFilesBaseNameJFSecVtx = "xAOD::BTagVertexContainer#"
      OutputFilesBaseAuxNameJFSecVtx= "xAOD::BTagVertexAuxContainer#"

      author = flags.BTagging.OutputFiles.Prefix + JetCollection
      # SecVert
      ItemList.append(OutputFilesBaseNameSecVtx + author + OutputFilesSVname)
      ItemList.append(OutputFilesBaseAuxNameSecVtx + author + OutputFilesSVname + 'Aux.-vxTrackAtVertex')
      # JFSeCVert
      ItemList.append(OutputFilesBaseNameJFSecVtx + author + OutputFilesJFVxname)
      ItemList.append(OutputFilesBaseAuxNameJFSecVtx + author + OutputFilesJFVxname + 'Aux.')

      if suffix:
          author += '_' + suffix

      ItemList.append(OutputFilesBaseName + author)
      ItemList.append(OutputFilesBaseAuxName + author + 'Aux.*')
      #ItemList.append(OutputFilesBaseAuxName + author + 'Aux.-BTagTrackToJetAssociatorBB')

      return ItemList

def BTagRedoESDCfg(flags, jet, extraContainers=[]):
    acc=ComponentAccumulator()

    acc.merge(RenameInputContainerCfg("old"))

    #Register input ESD container in output
    ESDItemList = registerOutputBTaggingContainers(flags, jet)
    ESDItemList += registerJetCollectionEL(flags, jet, [''])
    print(ESDItemList)
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags,"ESD", ItemList=ESDItemList+extraContainers))

    return acc

def BTagESDtoESDCfg(flags, jet, timestamp):
    acc=ComponentAccumulator()

    ESDItemList = []
    #Register new ouput ESD container
    for ts in timestamp:
        ESDItemList += registerOutputBTaggingContainers(flags, jet, ts)

    ESDItemList += registerJetCollectionEL(flags, jet, timestamp)

    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(flags,"ESD", ItemList=ESDItemList))

    return acc

def RenameInputContainerCfg(suffix):
    acc=ComponentAccumulator()

    #Delete BTagging container read from input ESD
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::JetAuxContainer#AntiKt4EMTopoJets.btaggingLink->AntiKt4EMTopoJets.btaggingLink_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingContainer#BTagging_AntiKt4EMTopo->BTagging_AntiKt4EMTopo_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_AntiKt4EMTopoAux.->BTagging_AntiKt4EMTopo_' + suffix+"Aux."]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::VertexContainer#BTagging_AntiKt4EMTopoSecVtx->BTagging_AntiKt4EMTopoSecVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::VertexAuxContainer#BTagging_AntiKt4EMTopoSecVtxAux.->BTagging_AntiKt4EMTopoSecVtx_' + suffix+"Aux."]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTagVertexContainer#BTagging_AntiKt4EMTopoJFVtx->BTagging_AntiKt4EMTopoJFVtx_' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTagVertexAuxContainer#BTagging_AntiKt4EMTopoJFVtxAux.->BTagging_AntiKt4EMTopoJFVtx_' + suffix+"Aux."]
    acc.addService(AddressRemappingSvc)
    acc.addService(ProxyProviderSvc(ProviderNames = [ "AddressRemappingSvc" ]))

    return acc

def RenameHLTaggerCfg(JetCollection, Tagger, suffix):
    acc=ComponentAccumulator()
    AddressRemappingSvc, ProxyProviderSvc=CompFactory.getComps("AddressRemappingSvc","ProxyProviderSvc",)
    AddressRemappingSvc = AddressRemappingSvc("AddressRemappingSvc")
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_' + JetCollection + '.' + Tagger + '_pu->BTagging_' + JetCollection + '.'+ Tagger + '_pu' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_' + JetCollection + '.' + Tagger + '_pc->BTagging_' + JetCollection + '.'+ Tagger + '_pc' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_' + JetCollection + '.' + Tagger + '_pb->BTagging_' + JetCollection + '.'+ Tagger + '_pb' + suffix]
    AddressRemappingSvc.TypeKeyRenameMaps += ['xAOD::BTaggingAuxContainer#BTagging_' + JetCollection + '.' + Tagger + '_ptau->BTagging_' + JetCollection + '.'+ Tagger + '_ptau' + suffix]
    acc.addService(AddressRemappingSvc)
    acc.addService(ProxyProviderSvc(ProviderNames = [ "AddressRemappingSvc" ]))

    return acc
def PrepareStandAloneBTagCfg(inputFlags):
    result=ComponentAccumulator()

    from AthenaPoolCnvSvc.PoolReadConfig import PoolReadCfg
    result.merge(PoolReadCfg(inputFlags))

    from TrkConfig.AtlasTrackingGeometrySvcConfig import TrackingGeometrySvcCfg
    acc = TrackingGeometrySvcCfg(inputFlags)
    result.merge(acc)

    from MuonConfig.MuonGeometryConfig import MuonGeoModelCfg
    result.merge(MuonGeoModelCfg(inputFlags))    

    GeometryDBSvc=CompFactory.GeometryDBSvc
    result.addService(GeometryDBSvc("InDetGeometryDBSvc"))
    
    from PixelGeoModel.PixelGeoModelConfig import PixelGeometryCfg
    result.merge(PixelGeometryCfg( inputFlags ))

    # get standard config for magnetic field - map and cache
    from MagFieldServices.MagFieldServicesConfig import MagneticFieldSvcCfg
    result.merge(MagneticFieldSvcCfg( inputFlags ))

    from IOVDbSvc.IOVDbSvcConfig import addFolders, addFoldersSplitOnline
    
    #load folders needed for Run2 ID alignment
    result.merge(addFoldersSplitOnline(inputFlags,"INDET","/Indet/Onl/Align","/Indet/Align",className="AlignableTransformContainer"))
    result.merge(addFolders(inputFlags,['/TRT/Align'],'TRT_OFL'))

    #load folders needed for IBL
    result.merge(addFolders(inputFlags,['/Indet/IBLDist'],'INDET_OFL'))

    return result

def BTagCfg(inputFlags, JetCollection = [], **kwargs):

    #This is monolithic for now. 
    #Once a first complete example runs, this will be split into small modular chunks.
    #Some such items may be best placed elsewehere (e.g. put magnetic field setup in magnetic field git folder etc)
    result=ComponentAccumulator()
    timestamp = kwargs.get('TimeStamp', None)
    if timestamp: del kwargs['TimeStamp']
    splitAlg = kwargs.get('SplitAlg', None)
    del kwargs['SplitAlg']


    TrainedTaggers = inputFlags.BTagging.run2TaggersList + ['MultiSVbb1','MultiSVbb2']
    result.merge(JetTagCalibCfg(inputFlags, TaggerList = TrainedTaggers, **kwargs))

    for jet in JetCollection:
        taggerList = inputFlags.BTagging.run2TaggersList
        taggerList += ['MultiSVbb1','MultiSVbb2']
        if timestamp:
            #Time-stamped BTagging container (21.2)
            result.merge(BTagESDtoESDCfg(inputFlags, jet, timestamp))
            kwargs['TimeStamp'] = timestamp
            result.merge(RenameInputContainerCfg("old"))
        else:
            extraCont = []
            extraCont.append("xAOD::VertexContainer#PrimaryVertices")
            extraCont.append("xAOD::VertexAuxContainer#PrimaryVerticesAux.")
            extraCont.append("xAOD::TrackParticleContainer#InDetTrackParticles")
            extraCont.append("xAOD::TrackParticleAuxContainer#InDetTrackParticlesAux.")
            if splitAlg:
                JFSecVtx = "xAOD::BTagVertexContainer#"
                AuxJFSecVtx= "xAOD::BTagVertexAuxContainer#"
                SecVtx = "xAOD::VertexContainer#"
                AuxSecVtx = "xAOD::VertexAuxContainer#"
                author = inputFlags.BTagging.OutputFiles.Prefix + jet
                extraCont.append(JFSecVtx + author + 'JFVtxMT')
                extraCont.append(AuxJFSecVtx + author + 'JFVtxMTAux.')
                extraCont.append(SecVtx + author + 'SecVtxMT')
                extraCont.append(AuxSecVtx + author + 'SecVtxMTAux.-vxTrackAtVertex')
                extraCont.append(SecVtx + author + 'MSVMT')
                extraCont.append(AuxSecVtx + author + 'MSVMTAux.-vxTrackAtVertex')
                extraCont.append("xAOD::BTaggingContainer#" + author + 'MT')
                extraCont.append("xAOD::BTaggingAuxContainer#" + author + 'MTAux.')
            result.merge(BTagRedoESDCfg(inputFlags, jet, extraCont))

        if splitAlg:
            secVertexingAndAssociators = {'JetFitter':'BTagTrackToJetAssociator','SV1':'BTagTrackToJetAssociator', 'MSV':'BTagTrackToJetAssociatorBB'}
            result.merge(JetBTaggerSplitAlgsCfg(inputFlags, JetCollection = jet, TaggerList = taggerList, SecVertexingAndAssociators = secVertexingAndAssociators, **kwargs))
        else:
            result.merge(JetBTaggerAlgCfg(inputFlags, JetCollection = jet, TaggerList = taggerList, **kwargs))

    return result

def JetBTaggerSplitAlgsCfg(inputFlags, JetCollection="", TaggerList=[], SecVertexingAndAssociators = {}, SetupScheme="", **kwargs):

    result=ComponentAccumulator()
    jet = JetCollection

    timestamp = kwargs.get('TimeStamp', None)
    if not timestamp:
        timestamp = ['']
    else:
        del kwargs['TimeStamp']

    postTagDL2JetToTrainingMap={
        'AntiKt4EMPFlow': [
        #'BTagging/201903/smt/antikt4empflow/network.json',
        'BTagging/201903/rnnip/antikt4empflow/network.json',
        'BTagging/201903/dl1r/antikt4empflow/network.json',
        'BTagging/201903/dl1/antikt4empflow/network.json',
        #'BTagging/201903/dl1rmu/antikt4empflow/network.json',
        ]
    }

    if jet in postTagDL2JetToTrainingMap:
        #Remove DL1 and RNNIP from taggers list, those taggers are run with HighLevelBTagAlg
        TaggerList.remove('RNNIP')
        TaggerList.remove('DL1')

    #Track Association
    TrackToJetAssociators = list(set(SecVertexingAndAssociators.values()))
    if kwargs.get('Release', None) == '22':
        #JetParticleAssociationAlg not implemented for BTagTrackToJetAssociatorBB in rel 22
        TrackToJetAssociators.remove('BTagTrackToJetAssociatorBB')

    for assoc in TrackToJetAssociators:
        result.merge(JetParticleAssociationAlgCfg(inputFlags, jet, "InDetTrackParticles", assoc, **kwargs))
        
    del kwargs['Release']

    #Sec vertex finding
    for k, v in SecVertexingAndAssociators.items():
        if v not in TrackToJetAssociators:
            print( v + ' is not configured, Sec vertex finding skipped')
            del SecVertexingAndAssociators[k]
        else:
            result.merge(JetSecVtxFindingAlgCfg(inputFlags, jet, "InDetTrackParticles", k, v))
            #Sec vertexing
            result.merge(JetSecVertexingAlgCfg(inputFlags, jet, "InDetTrackParticles", k, v))

    #result.merge(JetSecVertexingAlgCfg(inputFlags, jet, "InDetTrackParticles", 'MSV', 'BTagTrackToJetAssociatorBB'))

    #BTagging
    for ts in timestamp:
        result.merge(JetBTaggingAlgCfg(inputFlags, JetCollection = jet, TaggerList = TaggerList, SVandAssoc = SecVertexingAndAssociators, TimeStamp = ts, **kwargs))

    if jet in postTagDL2JetToTrainingMap:
        #Track Augmenter
        result.merge(BTagTrackAugmenterAlgCfg(inputFlags))

        for ts in timestamp:
            #HighLevel taggers can not be run with time stamped containers
            if ts == "":
                result.merge(RunHighLevelTaggersCfg(inputFlags, jet, 'BTagTrackToJetAssociator', postTagDL2JetToTrainingMap[jet], ts))

    return result

def RunHighLevelTaggersCfg(inputFlags, JetCollection, Associator, TrainingMaps, TimeStamp):
    result = ComponentAccumulator()

    AthSequencer=CompFactory.AthSequencer

    BTagCollection = 'BTagging_'+JetCollection
    sequenceName = BTagCollection + "_HLTaggers"
    if TimeStamp:
            BTagCollection += '_' + TimeStamp
            sequenceName += '_' + TimeStamp

    HLBTagSeq = AthSequencer(sequenceName, Sequential = True)
    result.addSequence(HLBTagSeq)

    result.merge(BTagHighLevelAugmenterAlgCfg(inputFlags, sequenceName, JetCollection = JetCollection, BTagCollection = BTagCollection, Associator = Associator) )
    for dl2 in TrainingMaps:
        result.merge(HighLevelBTagAlgCfg(inputFlags, sequenceName, BTagCollection, dl2) )

    return result

def BTagHLTaggersCfg(inputFlags, JetCollection = []):
    acc = ComponentAccumulator()

    # Nothing written out
    from OutputStreamAthenaPool.OutputStreamConfig import OutputStreamCfg
    acc.merge(OutputStreamCfg(cfgFlags,"ESD", ItemList=[]))

    DeprecatedHLTaggers = ['rnnip', 'DL1']
    postTagDL2JetToTrainingMap={
        'AntiKt4EMPFlow': [
        #'BTagging/201903/smt/antikt4empflow/network.json',
        'BTagging/201903/rnnip/antikt4empflow/network.json',
        'BTagging/201903/dl1r/antikt4empflow/network.json',
        'BTagging/201903/dl1/antikt4empflow/network.json',
        #'BTagging/201903/dl1rmu/antikt4empflow/network.json',
        ]
    }
    for jet in JetCollection:
        if jet in postTagDL2JetToTrainingMap:
            for tagger in DeprecatedHLTaggers:
                acc.merge(RenameHLTaggerCfg(jet, tagger, '_old'))
            #Track Augmenter
            acc.merge(BTagTrackAugmenterAlgCfg(cfgFlags))

            #HighLevel taggers can not be run with time stamped containers
            acc.merge(RunHighLevelTaggersCfg(cfgFlags, jet, 'BTagTrackToJetAssociator', postTagDL2JetToTrainingMap[jet], ""))

    return acc

def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')

if __name__=="__main__":

    inputESD = "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecExRecoTest/mc16_13TeV.361022.Pythia8EvtGen_A14NNPDF23LO_jetjet_JZ2W.recon.ESD.e3668_s3170_r10572_homeMade.pool.root"
    import argparse
    parser = argparse.ArgumentParser(prog="BTagRun3Config: An example configuration module for btagging reconstruction reading an ESD",
                            usage="Call with an input file, pass -n=0 to skip execution, -t 0 for serial or 1 for threaded execution.")
    parser.add_argument("-f", "--filesIn", default = inputESD, type=str, help="Comma-separated list of input files")
    parser.add_argument("-t", "--nThreads", default=1, type=int, help="The number of concurrent threads to run. 0 uses serial Athena.")
    parser.add_argument("-r", "--release", default="22", type=str, help="Release number to test different scenarii.")
    parser.add_argument("-l", "--highlevel", type=str2bool, default=False, help="Run only high level taggers.")
    parser.add_argument('-s', '--splitAlg', type=str2bool, default=False, help="Split JetBTaggerAlg.")

    args = parser.parse_args()

    from AthenaCommon.Configurable import Configurable
    Configurable.configurableRun3Behavior=1

    from AthenaConfiguration.AllConfigFlags import ConfigFlags as cfgFlags

    cfgFlags.Input.isMC=True
    cfgFlags.Input.Files= args.filesIn.split(",")
    #cfgFlags.Input.isMC=False
    #cfgFlags.Input.Files=["/atlas/guirriec/git-athena/q431_2019-03-02T2147/myESD_2019.pool.root"]

    cfgFlags.Output.ESDFileName="esdOut.pool.root"

    # Flags relating to multithreaded execution
    cfgFlags.Concurrency.NumThreads = args.nThreads
    if args.nThreads>0:
        cfgFlags.Scheduler.ShowDataDeps = True
        cfgFlags.Scheduler.ShowDataFlow = True
        cfgFlags.Scheduler.ShowControlFlow = True
        cfgFlags.Concurrency.NumConcurrentEvents = args.nThreads
    
    from AthenaConfiguration.MainServicesConfig import MainServicesCfg 
    acc=MainServicesCfg(cfgFlags)
    acc.getService("MessageSvc").Format = "% F%80W%S%7W%R%T %0W%M"

    # Prevent the flags from being modified
    cfgFlags.lock()

    acc.merge(PrepareStandAloneBTagCfg(cfgFlags))

    JetCollection = ['AntiKt4EMTopo', 'AntiKt4EMPFlow']

    if args.highlevel:
        acc.merge(BTagHLTaggersCfg(cfgFlags, JetCollection = JetCollection))
    else:
        kwargs = {}
        kwargs['Release'] = args.release
        if args.release == "21.2":
            kwargs["TimeStamp"] = ['201810','201903']
            kwargs['Release'] = '21'
        kwargs["SplitAlg"] = args.splitAlg

        acc.merge(BTagCfg(cfgFlags, JetCollection = JetCollection, **kwargs))

    acc.setAppProperty("EvtMax",-1)

    acc.run()
    f=open("BTag.pkl","wb")
    acc.store(f)
    f.close()
