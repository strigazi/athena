# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

""" Missing ET slice signatures """

__author__  = "E.Pianori, C.Bernius"
__version__ = "" 
__doc__="Implementation of Missing ET slice in new TM framework "
##########################################################

from AthenaCommon.SystemOfUnits import GeV
from AthenaCommon.Logging import logging
logging.getLogger().info("Importing %s",__name__)
log = logging.getLogger("TriggerMenu.met.MissingETDef")

def trace(frame, event, arg):
    if event == "call":
        filename = frame.f_code.co_filename
        lineno = frame.f_lineno
        print ("%s @ %s" % (filename, lineno))
        return trace
        


from TrigEFMissingET.TrigEFMissingETConfig import (EFMissingET_Fex_2sidednoiseSupp,
                                                   EFMissingET_Fex_Jets,
                                                   EFMissingET_Fex_TrackAndJets,
                                                   EFMissingET_Fex_TrackAndClusters,
                                                   EFMissingET_Fex_topoClusters,
                                                   EFMissingET_Fex_topoClustersPS, 
                                                   EFMissingET_Fex_topoClustersPUC,
                                                   EFMissingET_Fex_topoClustersTracksPUC)

from TrigL2MissingET.TrigL2MissingETConfig import L2MissingET_Fex

from TrigMissingETHypo.TrigMissingETHypoConfig import (EFMetHypoJetsXE,
                                                       EFMetHypoTrackAndJetsXE,
                                                       EFMetHypoTrackAndClustersXE,
                                                       EFMetHypoTCPSXE,
                                                       EFMetHypoTCPUCXE, 
                                                       EFMetHypoTCTrkPUCXE,
                                                       EFMetHypoTCXE,
                                                       EFMetHypoTE,
                                                       EFMetHypoXE, 
                                                       EFMetHypoXS_2sided)

from TrigMissingETMuon.TrigMissingETMuonConfig import (EFTrigMissingETMuon_Fex,
                                                       EFTrigMissingETMuon_Fex_Jets,
                                                       EFTrigMissingETMuon_Fex_topocl,
                                                       EFTrigMissingETMuon_Fex_topoclPS,
                                                       EFTrigMissingETMuon_Fex_topoclPUC,
                                                       L2TrigMissingETMuon_Fex)

from TrigGenericAlgs.TrigGenericAlgsConf import PESA__DummyUnseededAllTEAlgo

from TriggerMenu.jet.JetDef import generateHLTChainDef
from TriggerMenu.menu import DictFromChainName
from TriggerMenu.menu.HltConfig import L2EFChainDef, mergeRemovingOverlap
from TriggerMenu.menu.MenuUtils import splitChainDict
from TriggerMenu.muon.MuonDef import L2EFChain_mu
from TriggerMenu.menu.SignatureDicts import METChainParts_Default

#############################################################################
class L2EFChain_met(L2EFChainDef):
     
    def __init__(self, chainDict):
        self.L2sequenceList   = []
        self.EFsequenceList   = []
        self.L2signatureList  = []
        self.EFsignatureList  = []
        self.TErenamingDict   = []
    
        # May as well give the methods easy access to all the input information
        self.chainDict = chainDict
        l1_item_name = chainDict['L1item']        
        self.l2_input_tes = ''    ##MET is un-seeded
        chain_counter = chainDict['chainCounter']
        self.chainPart = chainDict['chainParts']
        l2_name = 'L2_'+self.chainPart['chainPartName']
        ef_name = 'EF_'+self.chainPart['chainPartName']
        self.mult = int(self.chainPart['multiplicity'])

        chainName = chainDict['chainName']
        sig_id = self.chainPart['chainPartName']
        self.sig_id_noMult = sig_id[1:] if self.mult > 1 else sig_id
               
        self.setup_xeXX()
            
        L2EFChainDef.__init__(self, chainName, l2_name, chain_counter, l1_item_name, ef_name, chain_counter, self.l2_input_tes)
        
    def defineSequences(self):
        for sequence in self.L2sequenceList:
            self.addL2Sequence(*sequence)
        for sequence in self.EFsequenceList:
            self.addEFSequence(*sequence)
                
    def defineSignatures(self):       
        for signature in self.L2signatureList:
            self.addL2Signature(*signature)
        for signature in self.EFsignatureList:
            self.addEFSignature(*signature)
 
    def defineTErenaming(self):
        self.TErenamingMap = self.TErenamingDict



############################### DEFINE GROUPS OF CHAINS HERE ##############################

    def setup_xeXX(self):

        ##EF only chain, run FEB or topo_cluster
        ##NoAlg at L2
        ##if at a certain point different steps are used at EF, 
        ## we need a way to encode the information of the threshold at both the two steps
        ##could be a dict here, or adding something to the SignatureDictionary
        
        threshold   = int(self.chainPart['threshold'])
        calibration = self.chainPart['calib']
        jetCalib    = self.chainPart['jetCalib']
        L2recoAlg   = self.chainPart['L2recoAlg']
        EFrecoAlg   = self.chainPart['EFrecoAlg']
        EFmuon      = self.chainPart['EFmuonCorr']
        addInfo     = self.chainPart["addInfo"]

        # tcpufit is the run 3 name for pufit
        if EFrecoAlg == "tcpufit":
            EFrecoAlg = "pufit"

        #--------------------------------------
        #obtaining the muon sequences & signature:
        #--------------------------------------
        log.debug("Creating muon sequence")
        
        chain = ['mu8', 'L1_MU6',  [], ["Main"], ['RATE:SingleMuon', 'BW:Muon'], -1]

        theDictFromChainName = DictFromChainName.DictFromChainName()
        muonChainDict = theDictFromChainName.getChainDict(chain)
        listOfChainDicts = splitChainDict(muonChainDict)
        muDict = listOfChainDicts[0]
        muDict['chainCounter'] = 9150
        muonthing = L2EFChain_mu(muDict, False, ['8GeV']) 
        muonSeed = muonthing.EFsignatureList[-1][0][0]

        for seq in muonthing.L2sequenceList:
             self.L2sequenceList += [seq]
        for seq in muonthing.EFsequenceList:
             self.EFsequenceList += [seq]
 

        ########### Imports for hypos and fexes  ###########
        ##L1 MET 
        theL2Fex     = L2MissingET_Fex()
        theL2MuonFex = L2TrigMissingETMuon_Fex()

        mucorr=  '_wMu' if EFmuon else ''          
        ##MET with topo-cluster
        if EFrecoAlg=='tc' or EFrecoAlg=='pueta' or EFrecoAlg=='pufit' or EFrecoAlg=='mht' or EFrecoAlg=='trkmht' or EFrecoAlg=='pufittrack' or EFrecoAlg=='trktc':

            ##Topo-cluster
            if EFrecoAlg=='tc':
                #MET fex
                theEFMETFex     = EFMissingET_Fex_topoClusters()                         
                #Muon correction fex
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topocl()        

                #TC hypo
                #mucorr=  '_wMu' if EFmuon else ''      

                if self.chainPart['trigType'] == "xs":
                    theEFMETHypo = EFMetHypoXS_2sided('EFMetHypo_xs_2sided_%i%s' % (threshold, mucorr),ef_thr=float(threshold)*0.1)
                elif  self.chainPart['trigType'] == "te":
                    theEFMETHypo = EFMetHypoTE('EFMetHypo_te%d' % threshold,ef_thr=float(threshold)*GeV)
                else:               
                    theEFMETHypo = EFMetHypoTCXE('EFMetHypo_TC_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)  
                
            if EFrecoAlg=='pufit':

                doLArH11off=False
                doLArH12off=False
                LArTag=''
                if "LArH11off" in addInfo: 
                    doLArH11off = True
                    LArTag += '_LArH11off'
                if "LArH12off" in addInfo:
                    doLArH12off = True
                    LArTag += '_LArH12off'
                if "METphi" in addInfo: 
                    LArTag += '_METphi'

                jpt_thr = '-1'
                if len(addInfo.split('Jpt'))==2: jpt_thr = addInfo.split('Jpt')[1] 

                if "Jpt" in addInfo:
                    LArTag += '_Jpt'+jpt_thr

                #MET fex
                theEFMETFex = EFMissingET_Fex_topoClustersPUC("EFMissingET_Fex_topoClustersPUC%s"%(addInfo),doLArH11off,doLArH12off,float(jpt_thr)) 
                #Muon correction fex
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topoclPUC()
                mucorr= '_wMu' if EFmuon else '' 

                theEFMETHypo = EFMetHypoTCPUCXE('EFMetHypo_TCPUC'+LArTag+'_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV,labelMET=addInfo)


            if EFrecoAlg=='pufittrack':
                calibCorr = ('_{0}'.format(calibration) if calibration != METChainParts_Default['calib'] else '') + ('_{0}'.format(jetCalib) if jetCalib != METChainParts_Default['jetCalib'] else '')
                #MET fex
                #print ("PUFITTRACK XXXXXXXXXX")
                #print (calibCorr)
                theEFMETFex = EFMissingET_Fex_topoClustersTracksPUC("EFMissingET_Fex_topoClustersTracksPUC{0}".format(calibCorr), extraCalib=calibCorr)
                #print ("PUFITTRACK XXXXXXXXXX")
                #print (theEFMETFex)
                #Muon correction fex
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topoclPUC()
                #print (theEFMETMuonFex)
                mucorr= '_wMu' if EFmuon else ''
                #theEFMETHypo = EFMetHypoTCTrkPUCXE('EFMetHypo_TCTrkPUC_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)
                theEFMETHypo = EFMetHypoTCTrkPUCXE('EFMetHypo_TCTrkPUC_xe%s_tc%s%s%s'%(threshold,jetCalib,calibration,mucorr),ef_thr=float(threshold)*GeV, extraCalib=calibCorr)

            ##MET based on trigger jets
            if EFrecoAlg=='mht':
                calibCorr = ('_{0}'.format(calibration) if calibration != METChainParts_Default['calib'] else '') + ('_{0}'.format(jetCalib) if jetCalib != METChainParts_Default['jetCalib'] else '')

                #MET fex
                theEFMETFex = EFMissingET_Fex_Jets("EFMissingET_Fex_Jets{0}".format(calibCorr), extraCalib=calibCorr )
                #Muon correction fex
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_Jets("EFTrigMissingETMuon_Fex_Jets{0}".format(calibCorr) )
                #mucorr= '_wMu' if EFmuon else ''
                theEFMETHypo = EFMetHypoJetsXE('EFMetHypo_Jets_xe%s_tc%s%s%s'%(threshold,jetCalib,calibration,mucorr),ef_thr=float(threshold)*GeV, extraCalib=calibCorr)


             ##MET based on trigger jets
            if EFrecoAlg=='trkmht':
                #MET fex                                                                                                                                                    
                theEFMETFex = EFMissingET_Fex_TrackAndJets()                                                                                                                
                #Muon correction fex                                                                                                                                        
                ## this will be added later                                                                                                                                 
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_Jets()                                                                                                            
                #mucorr= '_wMu' if EFmuon else ''                                                                                                                           
                theEFMETHypo = EFMetHypoTrackAndJetsXE('EFMetHypo_TrackAndJets_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)

                
            if EFrecoAlg=='trktc':
                #MET fex                                                                                                                                                    
                theEFMETFex = EFMissingET_Fex_TrackAndClusters()                                                                                                                
                #Muon correction fex                                                                                                                                        
                ## this will never be used                                                                                                                                 
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topocl()                                                                                                            
                #mucorr= '_wMu' if EFmuon else ''                                                                                                                           
                theEFMETHypo = EFMetHypoTrackAndClustersXE('EFMetHypo_TrackAndClusters_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)

       
            ##Topo-cluster with Pile-up suppression
            if EFrecoAlg=='pueta':
                #MET fex
                theEFMETFex = EFMissingET_Fex_topoClustersPS() 
                #Muon correction fex
                theEFMETMuonFex = EFTrigMissingETMuon_Fex_topoclPS()        

                theEFMETHypo = EFMetHypoTCPSXE('EFMetHypo_TCPS_xe%s_tc%s%s'%(threshold,calibration,mucorr),ef_thr=float(threshold)*GeV)



        ##2-SidedNoise Cell
        elif EFrecoAlg=='cell':
            #MET fex
            theEFMETFex = EFMissingET_Fex_2sidednoiseSupp()

            #Muon correction fex
            theEFMETMuonFex = EFTrigMissingETMuon_Fex()        
            
            #Hypo
            if self.chainPart['trigType'] == "xs":
                theEFMETHypo = EFMetHypoXS_2sided('EFMetHypo_xs_2sided_%d%s' % (threshold, mucorr),ef_thr=float(threshold)*0.1)                    
            elif  self.chainPart['trigType'] == "te":
                theEFMETHypo = EFMetHypoTE('EFMetHypo_te%d'% threshold,ef_thr=threshold*GeV)
            else:               
                LArTag=''
                if "LArH11off" in addInfo: LArTag += '_LArH11off'
                if "LArH12off" in addInfo: LArTag += '_LArH12off'
                if "METphi" in addInfo: LArTag += '_METphi'
                theEFMETHypo = EFMetHypoXE('EFMetHypo'+LArTag+'_xe%s%s'%(threshold,mucorr),ef_thr=float(threshold)*GeV)  

        else:
            log.warning("MET EF algorithm not recognised")
        
        #----------------------------------------------------
        # Obtaining the needed jet TEs from the jet code and b-jet code 
        #----------------------------------------------------
        from TriggerJobOpts.TriggerFlags import TriggerFlags
        
        from TriggerMenu.bjet.generateBjetChainDefs import generateChainDefs as generateBjetChainDefs
    
        chain = ['j0_{0}_{1}'.format(calibration, jetCalib), '', [], ["Main"], ['RATE:SingleJet', 'BW:Jet'], -1]

        theDictFromChainName = DictFromChainName.DictFromChainName()
        jetChainDict = theDictFromChainName.getChainDict(chain)
        
        jetChainDict['chainCounter'] = 9151
        jetChainDef = generateHLTChainDef(jetChainDict)
        #This is a dummy b-jet chain, with a threshold at 20 GeV at the uncalibrated scale. It computes the tracks within each jet RoI. 
        #Change the calibration by changing 'nojcalib' to the desired calibration scale. 
        #For pufittrack, we found that the performance was superior using uncalibrated jets. 
        dummy_bjet_chain = ['j20_{0}_{1}_boffperf_split'.format(calibration, jetCalib),  '', [], ["Main"], ['RATE:SingleBJet', 'BW:BJet'], -1]
        bjet_chain_dict = theDictFromChainName.getChainDict(dummy_bjet_chain)
        bjet_chain_dict["chainCounter"] = 9152
        bjet_chain_dict['topoThreshold'] = None
        bjet_chain_def = generateBjetChainDefs(bjet_chain_dict)
       
        #for i in range(3):
        #    m_input[i] = jetChainDef.sequenceList[i]['input']
        #    m_output[i]= jetChainDef.sequenceList[i]['output']
        #    m_algo[i] =jetChainDef.sequenceList[i]['algorithm']

        #obtaining DummyUnseededAllTEAlgo/RoiCreator 
        input0=jetChainDef.sequenceList[0]['input']
        output0 =jetChainDef.sequenceList[0]['output']
        algo0 =jetChainDef.sequenceList[0]['algorithm']
        
        #obtaing TrigCaloCellMaker/FS 
        input1=jetChainDef.sequenceList[1]['input']
        output1 =jetChainDef.sequenceList[1]['output']
        algo1 =jetChainDef.sequenceList[1]['algorithm']
        
        #obtaining TrigCaloClusterMaker
        input2=jetChainDef.sequenceList[2]['input']
        output2 =jetChainDef.sequenceList[2]['output']
        algo2 =jetChainDef.sequenceList[2]['algorithm']

        #obtaining TrigHLTEnergyDensity
        input3=jetChainDef.sequenceList[3]['input']
        output3 =jetChainDef.sequenceList[3]['output']
        algo3 =jetChainDef.sequenceList[3]['algorithm']

        #obtaining TrigHLTJetRecFromCluster
        input4=jetChainDef.sequenceList[4]['input']
        output4 =jetChainDef.sequenceList[4]['output']
        algo4 =jetChainDef.sequenceList[4]['algorithm']

        #---End of obtaining jet TEs------------------------------
                   
        ########### Sequences ###########
        
        #Run L2-like algos only for l2fsperf and L2FS chains
        if L2recoAlg=="l2fsperf" or L2recoAlg=="L2FS":

            ##L1 MET
            self.L2sequenceList += [[ self.l2_input_tes,              [theL2Fex],                      'L2_xe_step1']]
            ##Moun Correction to L1 MET
            self.L2sequenceList += [[ ['L2_xe_step1', muonSeed],  [theL2MuonFex],                  'L2_xe_step2']]

        # --- EF ---                

        # cell preselection (v7 onwards only)
        # First check if we're in a multipart chain (for now assume that we don't apply the preselection for these)
        isMulitpartChain = self.chainPart['chainPartName'] != self.chainDict['chainName']
        if EFrecoAlg != 'cell' and 'v6' not in TriggerFlags.triggerMenuSetup() and 'v5' not in TriggerFlags.triggerMenuSetup() and not isMulitpartChain:
        # if EFrecoAlg != 'cell' and TriggerFlags.run2Config() != '2016' and not isMulitpartChain:
          # a few parameters
          cellPresel_minL1Threshold = 50
          cellPresel_threshold = 50
          # work out what the L1 threshold is
          import re
          match = re.match(r"L1_XE(\d+)", self.chainDict['L1item'])
          if match:
            if int(match.group(1) ) >= cellPresel_minL1Threshold:
              cellPreselectionFex = EFMissingET_Fex_2sidednoiseSupp()
              cellPreselectionMuonFex = EFTrigMissingETMuon_Fex()
              cellPreselectionHypo = EFMetHypoXE('EFMetHypo_xe{0}_presel'.format(cellPresel_threshold), ef_thr = float(cellPresel_threshold) * GeV)

              self.EFsequenceList += [[ [''], [cellPreselectionFex], 'EF_xe{0}_step1'.format(cellPresel_threshold) ]]
              self.EFsequenceList += [[ ['EF_xe{0}_step1'.format(cellPresel_threshold), muonSeed], [cellPreselectionMuonFex, cellPreselectionHypo], 'EF_xe{0}_step2'.format(cellPresel_threshold) ]]
              
              self.EFsignatureList += [ [['EF_xe{0}_step1'.format(cellPresel_threshold)]] ]
              self.EFsignatureList += [ [['EF_xe{0}_step2'.format(cellPresel_threshold)]] ]
          else:
            log.info("Pure MET chain doesn't have an L1_XE seed! Will not apply the cell preselection")


        #topocluster
        if EFrecoAlg=='tc' or EFrecoAlg=='pueta' or EFrecoAlg=='pufit':
            self.EFsequenceList +=[[ input0,algo0,  output0 ]]            
            self.EFsequenceList +=[[ input1,algo1,  output1 ]]            
            self.EFsequenceList +=[[ input2,algo2,  output2 ]]           
            self.EFsequenceList +=[[ input3,algo3,  output3 ]]            
            self.EFsequenceList +=[[ input4,algo4,  output4 ]]
            self.EFsequenceList +=[[ [output3,output4],          [theEFMETFex],  'EF_xe_step1' ]]            
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed],     [theEFMETMuonFex, theEFMETHypo],  'EF_xe_step2' ]]


        elif EFrecoAlg=='pufittrack':
            def makelist(x):
                return x if isinstance(x, list) else [x]
            self.EFsequenceList += [ [ x['input'], makelist(x['algorithm']), x['output'] ] for x in bjet_chain_def.sequenceList[:-2] ]
#            for x in bjet_chain_def.sequenceList[:-2]:
#                print (x)
#            print (self.EFsequenceList[4][2], "Clusters, output EF_FSTopoClusters")
#            print (self.EFsequenceList[6][2], "Jets, output EF_8389636500743033767_jetrec_a4tclcwnojcalibFS")
#            print (self.EFsequenceList[-1][2], "Tracks, output HLT_j20_eta_jsplit_IDTrig")
#            print (self.EFsequenceList[-5][2], "Vertex, output HLT_superIDTrig_prmVtx")
#            Fill the sequence with clusters, jets, tracks, vertices 
            self.EFsequenceList += [[ [self.EFsequenceList[4][2],self.EFsequenceList[6][2],self.EFsequenceList[-1][2],self.EFsequenceList[-5][2]], [theEFMETFex], 'EF_xe_step1' ]]  
            self.EFsequenceList += [[ ['EF_xe_step1',muonSeed], [theEFMETMuonFex, theEFMETHypo], 'EF_xe_step2' ]]

        #trigger-jet based MET
        elif EFrecoAlg=='mht': 
            self.EFsequenceList +=[[ input0,algo0,  output0 ]]            
            self.EFsequenceList +=[[ input1,algo1,  output1 ]]            
            self.EFsequenceList +=[[ input2,algo2,  output2 ]]           
            self.EFsequenceList +=[[ input3,algo3,  output3 ]]            
            self.EFsequenceList +=[[ input4,algo4,  output4 ]]            
            self.EFsequenceList +=[[ [output4], [theEFMETFex], 'EF_xe_step1' ]]
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed], [theEFMETMuonFex, theEFMETHypo], 'EF_xe_step2' ]]
            if "FStracks" in addInfo:
                from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
                trk_algs = TrigInDetSequence("FullScan", "fullScan", "IDTrig", sequenceFlavour=["FTF"]).getSequence()
                print (trk_algs[0])
                dummyAlg = PESA__DummyUnseededAllTEAlgo("EF_DummyFEX_xe")
                self.EFsequenceList +=[[ [''], [dummyAlg]+trk_algs[0], 'EF_xe_step3' ]]

        elif EFrecoAlg=='trkmht':
            self.EFsequenceList +=[[ input0,algo0,  output0 ]]
            self.EFsequenceList +=[[ input1,algo1,  output1 ]]
            self.EFsequenceList +=[[ input2,algo2,  output2 ]]
            self.EFsequenceList +=[[ input3,algo3,  output3 ]]
            self.EFsequenceList +=[[ input4,algo4,  output4 ]]

            #adding FullScan tracks
            from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
            trk_algs = TrigInDetSequence("FullScan", "fullScan", "IDTrig", sequenceFlavour=["FTF"]).getSequence()
            dummyAlg = PESA__DummyUnseededAllTEAlgo("EF_DummyFEX_xe")
            self.EFsequenceList +=[[ [''], [dummyAlg]+trk_algs[0], 'EF_xe_step0' ]]

            self.EFsequenceList +=[[ [output4,'EF_xe_step0',muonSeed], [theEFMETFex], 'EF_xe_step1' ]]                                                                               
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed], [theEFMETMuonFex, theEFMETHypo], 'EF_xe_step2' ]]   


        elif EFrecoAlg=='trktc':
            self.EFsequenceList +=[[ input0,algo0,  output0 ]]
            self.EFsequenceList +=[[ input1,algo1,  output1 ]]
            self.EFsequenceList +=[[ input2,algo2,  output2 ]]
            self.EFsequenceList +=[[ input3,algo3,  output3 ]]
            #self.EFsequenceList +=[[ input4,algo4,  output4 ]]

            ##adding FullScan tracks
            from TrigInDetConf.TrigInDetSequence import TrigInDetSequence
            trk_algs = TrigInDetSequence("FullScan", "fullScan", "IDTrig", sequenceFlavour=["FTF"]).getSequence()
            dummyAlg = PESA__DummyUnseededAllTEAlgo("EF_DummyFEX_xe")
            self.EFsequenceList +=[[ [''], [dummyAlg]+trk_algs[0], 'EF_xe_step0' ]]

            self.EFsequenceList +=[[ [output3,'EF_xe_step0',muonSeed], [theEFMETFex], 'EF_xe_step1' ]]                                                                               
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed], [theEFMETMuonFex, theEFMETHypo], 'EF_xe_step2' ]]   


        #cell based MET
        elif EFrecoAlg=='cell':
            self.EFsequenceList +=[[ [''],          [theEFMETFex],  'EF_xe_step1' ]]  
            self.EFsequenceList +=[[ ['EF_xe_step1',muonSeed],     [theEFMETMuonFex, theEFMETHypo],  'EF_xe_step2' ]]
            
            
        ########### Signatures ###########
        if L2recoAlg=="l2fsperf" or  L2recoAlg=="L2FS" :
            self.L2signatureList += [ [['L2_xe_step1']] ]
            self.L2signatureList += [ [['L2_xe_step2']] ]

        if EFrecoAlg=="trkmht" or EFrecoAlg=="trktc" :
            self.EFsignatureList += [ [['EF_xe_step0']] ]   

        self.EFsignatureList += [ [['EF_xe_step1']] ]
        self.EFsignatureList += [ [['EF_xe_step2']] ]
        if "FStracks" in addInfo:
            self.EFsignatureList += [ [['EF_xe_step3']] ]
        

        ########### TE renaming ###########
        self.TErenamingDict = {}
            
        if L2recoAlg=="l2fsperf" or  L2recoAlg=="L2FS" :
            self.TErenamingDict['L2_xe_step1']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step1')
            self.TErenamingDict['L2_xe_step2']= mergeRemovingOverlap('L2_', self.sig_id_noMult+'_step2')

        if EFrecoAlg=='trkmht' or EFrecoAlg=='trktc':
            self.TErenamingDict['EF_xe_step0']= mergeRemovingOverlap('EF_', self.sig_id_noMult+"_step0")                                                                    
        #if EFrecoAlg=='pufittrack':
        #    self.TErenamingDict['EF_xe_step0']= mergeRemovingOverlap('EF_', self.sig_id_noMult+"_step0")
            
        self.TErenamingDict['EF_xe_step1']= mergeRemovingOverlap('EF_', self.sig_id_noMult+'_step1')
        if "FStracks" in addInfo:
            self.TErenamingDict['EF_xe_step2']= mergeRemovingOverlap('EF_', self.sig_id_noMult+"_step2")
            self.TErenamingDict['EF_xe_step3']= mergeRemovingOverlap('EF_', self.sig_id_noMult)
        else:
            self.TErenamingDict['EF_xe_step2']= mergeRemovingOverlap('EF_', self.sig_id_noMult)

        
