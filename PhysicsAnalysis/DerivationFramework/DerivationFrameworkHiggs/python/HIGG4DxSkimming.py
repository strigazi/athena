# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

#################################################
# Common code used for the HIGG4 skimming       #
# Z.Zinonos                                     #
# zenon@cern.ch                                 #
# Nov 2015                                      #
#################################################

from AthenaCommon.GlobalFlags import globalflags

def setup(HIGG4DxName, ToolSvc):

    skimmingTools = []

    #muon quality
    muonQual = "Muons.DFCommonMuonsPreselection"
    
    #electron quality
    eleLoose  = "(Electrons.DFCommonElectronsIsEMLoose || Electrons.DFCommonElectronsLHLoose)"
    eleMedium = "(Electrons.DFCommonElectronsIsEMMedium || Electrons.DFCommonElectronsLHMedium)"
    eleTight  = "(Electrons.DFCommonElectronsIsEMTight || Electrons.DFCommonElectronsLHTight)"

    # el calo crack region: 1.52 < el->caloCluster()->etaBE(2) < 1.37,  with el->eta: apply a 5% offset 
    # veto events with el->caloCluster()->etaBE(2) ) < 2.47, with el->eta: apply a 5% offset 
    #muonVeto = '(count( Muons.pt > 15.0*GeV && abs(Muons.eta) < 2.5 && '+muonTight+' ) == 0)' #not supported since 18/5/2016
    eleVeto = '(count( Electrons.pt > 15.0*GeV && (abs(Electrons.eta) < 1.30 || abs(Electrons.eta) > 1.60) && abs(Electrons.eta) < 2.35  && '+eleTight+' ) == 0)'
    lepVeto = eleVeto  #+ '&& '+ muonVeto

    #tau prongs and tracks
    tauProngs = "abs(TauJets.charge)==1.0 && (TauJets.nTracks == 1 || TauJets.nTracks == 3)"
    tauTracks = "(TauJets.nTracks == 1 || TauJets.nTracks == 3)"

    if HIGG4DxName == 'HIGG4D1':     
        muonLead = "Muons.pt > 13.0*GeV && abs(Muons.eta) < 2.5 && " + muonQual
        muonSubL = "Muons.pt > 9.0*GeV && abs(Muons.eta) < 2.5 && " + muonQual
        electron = "Electrons.pt > 13.0*GeV && " + eleLoose
        count1 = "( count("+muonLead+") + count("+electron+") ) >=1"
        count2 = "( count("+muonSubL+") + count("+electron+") ) >=2"
        skim_expression = "("+count1+") && ("+count2+")"

    elif HIGG4DxName == 'HIGG4D2':
        #MuTrig: (pT(mu)>18 && pT(tau)>18) || (pT(mu)>12 && pT(tau)>23)  EleTrig: (pT(el)>22 && pT(tau)>18) || (pT(el)>15 && pT(tau)>23)
        e15 = '(count( Electrons.pt > 15.0*GeV && '+eleMedium+') >= 1)'
        e22 = '(count( Electrons.pt > 22.0*GeV && '+eleMedium+') >= 1)'
        mu12 = '(count( Muons.pt > 12.0*GeV && abs(Muons.eta) < 2.5 && '+muonQual+' ) >= 1)'
        mu18 = '(count( Muons.pt > 18.0*GeV && abs(Muons.eta) < 2.5 && '+muonQual+' ) >= 1)'
        tau18 = '(count( TauJets.pt > 18.0*GeV && '+tauProngs+' ) >= 1)'
        tau23 = '(count( TauJets.pt > 23.0*GeV && '+tauProngs+' ) >= 1)'
        mutau = '('+mu18+' && '+tau18+') || ('+mu12+' && '+tau23+')'
        etau = '('+e22+' && '+tau18+') || ('+e15+' && '+tau23+')'
        skim_expression = '('+mutau+') || ('+etau+')'

    elif HIGG4DxName == 'HIGG4D3':
        tauTrks = '( (TauJets.nTracks + TauJets.nTracksIsolation >= 1) && (TauJets.nTracks + TauJets.nTracksIsolation <= 8) )'
        tauLead = '(TauJets.pt > 33.0*GeV)'
        tauSubl = '(TauJets.pt > 23.0*GeV)'
        tauId   = 'HIGG4DxJetBDTSigLoose'
        tauReq0 = '(count( '+tauSubl+' && '+tauTrks+' ) >= 2)'
        tauReq1 = '(count( '+tauSubl+' && '+tauTrks+' && '+tauId+' ) >= 1)'
        tauReq2 = '(count( '+tauLead+' && '+tauTrks+' ) >= 1)'
        skim_expression = tauReq0 + '&&' + tauReq1 + '&&' + tauReq2 + '&&'  + lepVeto

    elif HIGG4DxName == 'HIGG4D4':
        ditau = '(count( '+tauProngs+' && TauJets.pt > 45.0*GeV) >= 2)'
        tau1 = '(count((TauJets.pt > 100.0*GeV)) >= 1)'
        tau2 = '(count((TauJets.pt > 45.0*GeV)) >= 2)'
        tauTrack = '(count('+tauTracks+' && TauJets.pt > 45.0*GeV) >= 1)'
        trigger = '( HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 || HLT_j360 || HLT_j380 || HLT_j400 || HLT_j420 || HLT_j440 || HLT_j460 || HLT_tau80_medium1_tracktwo_L1TAU60 || HLT_tau125_medium1_tracktwo || HLT_tau160_medium1_tracktwo || HLT_tau80_medium1_tracktwo_L1TAU60_tau50_medium1_tracktwo_L1TAU12 || HLT_tau35_medium1_tracktwo_tau25_medium1_tracktwo_L1TAU20IM_2TAU12IM)'
        DFisMC = (globalflags.DataSource()=='geant4')
        if not DFisMC:
            hadhad = '(((' + ditau + ') || (' + tau1 + ' && ' + tau2 + ' && ' + tauTrack + ')) && ' + trigger + ')'
        else:
            hadhad = ditau 
        skim_expression = hadhad + "&&" + lepVeto

    elif HIGG4DxName == 'HIGG4D5':
        tauTracks123 = "(TauJets.nTracks == 1 || TauJets.nTracks == 2 || TauJets.nTracks == 3)"
        tau = '(count('+tauTracks123+' && TauJets.pt > 50.0*GeV) >= 1)'
        trigger_main = '( HLT_xe70 || HLT_xe70_mht || HLT_xe90_L1XE50 || HLT_xe90_mht_L1XE50 || HLT_xe110_L1XE50 || HLT_xe110_mht_L1XE50 || HLT_j360 || HLT_j380 || HLT_tau80_medium1_tracktwo_L1TAU60 || HLT_tau125_medium1_tracktwo || HLT_tau160_medium1_tracktwo || HLT_noalg_L1J400 )'
        trigger_aux = '( HLT_j15 || HLT_j25 || HLT_j35 || HLT_j55 || HLT_j60 || HLT_j85 || HLT_j110 || HLT_j150 || HLT_j175 || HLT_j200 || HLT_j260 || HLT_j300 || HLT_j320 )'
        DFisMC = (globalflags.DataSource()=='geant4')
        if not DFisMC:
            monotau = '('+tau+' && ('+trigger_main+' || '+trigger_aux+'))'
        else:
            monotau = '('+tau+' && '+trigger_main+')'
        skim_expression = monotau + "&&" + lepVeto
    else:
        assert False, "HIGG4DxSkimming: Unknown derivation stream '{}'".format(HIGG4DxName)

    from DerivationFrameworkTools.DerivationFrameworkToolsConf import DerivationFramework__xAODStringSkimmingTool
    HIGG4DxSkimmingTool = DerivationFramework__xAODStringSkimmingTool( name 		= HIGG4DxName+"SkimmingTool",
                                                                       expression 	= skim_expression)
    ToolSvc += HIGG4DxSkimmingTool
    skimmingTools.append(HIGG4DxSkimmingTool)

    return skimmingTools
