# all these tests, except egamma, should be in egammaGetter
from CaloRec.CaloRecFlags import jobproperties
from egammaRec.egammaRecFlags import jobproperties
from RecExConfig.RecFlags  import rec
from RecExConfig.RecAlgsFlags  import recAlgs
from AthenaCommon.Resilience import treatException
from AthenaCommon.DetFlags import DetFlags
from egammaRec import egammaKeys


from AthenaCommon.AlgSequence import AlgSequence
topSequence = AlgSequence()

#####################################################################
# Run calo-based (egamma) algorithm, includes topo-seeded
# It requires either to read ESD
# either that Emcluster have been build
if jobproperties.egammaRecFlags.doEgammaCaloSeeded()  and ( rec.readESD() or jobproperties.CaloRecFlags.doEmCluster()) :
    # 
    try:
        if jobproperties.egammaRecFlags.doEgammaCaloSeeded:
            from egammaRec.egammaGetter import egammaGetter
            egammaGetter(ignoreExistingDataObject=True)
    except Exception:
        treatException("Could not set up egammaGetter. Switch it off !")
        jobproperties.egammaRecFlags.doEgammaCaloSeeded=False    
        egammaGetter(disable=True)
else:
    jobproperties.egammaRecFlags.doEgammaCaloSeeded=False
    try:
        from egammaRec.egammaGetter import egammaGetter
        egammaGetter(disable=True)
    except Exception:
        pass

####################################################################
# Run forward electrons algorithm
# It requires either to read ESD
# either that CaloTopoCluster have been build
# It is not run if scoping level is greater or equal to 5 (so a priori always)
if jobproperties.egammaRecFlags.doEgammaForwardSeeded and ( rec.readESD() or jobproperties.CaloRecFlags.doCaloTopoCluster()):
    try:
        from egammaRec.egammaForwardGetter import egammaForwardGetter
        # descoping of track-seeded algorithm
        if rec.ScopingLevel()>=5:
            egammaForwardGetter(disable=True)
        else:
            egammaForwardGetter(ignoreExistingDataObject=True)
    except Exception: 
        jobproperties.egammaRecFlags.doEgammaForwardSeeded=False    
        treatException("Could not set up egammaForwardGetter. Switch it off !")
        egammaForwardGetter(disable=True)
else:
    jobproperties.egammaRecFlags.doEgammaForwardSeeded=False    
    try:
        from egammaRec.egammaForwardGetter import egammaForwardGetter
        egammaForwardGetter(disable=True)
    except Exception:
        pass

####################################################################
# Run association between true and reco objects
# It requires Truth and some of the above to be actually there

if not rec.doTruth(): 
  jobproperties.egammaRecFlags.doEgammaTruthAssociation = False

if jobproperties.egammaRecFlags.doEgammaTruthAssociation() and jobproperties.egammaRecFlags.doEgammaCaloSeeded() :
    try:
        from egammaRec.egammaTruthAssociationAlg import egammaTruthAssociationAlg
        egammaTruthAssociationAlg(MatchForwardElectrons= jobproperties.egammaRecFlags.doEgammaForwardSeeded(),
                                  MatchClusters=False)
    except Exception:
        treatException("Could not set up egammaTruthAssociationAlg. Switched off !")

####################################################################
# Lock egamma containers
if rec.doESD():
    try:
        from egammaRec.egammaLocker import egammaLocker
        topSequence +=egammaLocker(name= "egLocker",
                                   doTruth=rec.doTruth(),
                                   doFinalizeClusters =jobproperties.egammaRecFlags.doEgammaCaloSeeded(),
                                   doEgammaForwardSeeded=jobproperties.egammaRecFlags.doEgammaForwardSeeded(),
                                   doEgammaCaloSeeded=jobproperties.egammaRecFlags.doEgammaCaloSeeded(),
                                   outputClusterKey=egammaKeys.outputClusterKey(),
                                   egammakeys=egammaKeys.outputs.items())
    except:
        treatException("Could not set up egammaLocker. Switched off !")

