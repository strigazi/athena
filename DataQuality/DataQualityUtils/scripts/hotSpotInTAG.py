#!/usr/bin env python

# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

# Author : Benjamin Trocme / Summer 2012
# Make use of extractPath library to retrieve the correct castor path

# Script to browse a TAG file and extract LBs for which at least N occurences of an object is found
# in a region defined as noisy.
#Options:
#  -h, --help            show this help message and exit
#  -r RUN, --run=RUN     Run number
#  -s STREAMS, --stream=STREAMS Data stream : express/CosmicCalo/JetTauEtmiss/Egamma
#  -a AMI, --amiTag=AMI  ami Tag - Simply set x/f to choose express/bulk processing
#  -e ETA, --eta=ETA     Eta of hot spot
#  -p PHI, --phi=PHI     Phi of hot spot (or MET bump)
#  -t THRESHOLD, --treshold=THRESHOLD Et/pt threshold (in MeV)
#  -d DELTA, --delta=DELTA Distance to look around hot spot (or MET bump)
#  -o OBJECT, --object=OBJECT TopoCluster/Jet/LoosePhoton/TauJet/MET
#  -m MIN, --min=MIN     Min number of object in a LB
#  -n, --noplot          Do not plot LB map
# The -h does not work. I should move to argpars...

import os, sys  
import string
import subprocess as sp
from optparse import OptionParser

#sys.path.append("/afs/cern.ch/user/t/trocme/public/libraries")
import pathExtract         

import ROOT
from ROOT import *
from ROOT import gROOT, gDirectory
from ROOT import gStyle, TCanvas, TString
from ROOT import TFile, TTree
from ROOT import TH1F,TH2F,TBrowser
from ROOT import TPaveText

gROOT.Reset()
gStyle.SetPalette(1)
gStyle.SetOptStat("em")

def analyzeTree():
  global nbHitInHot
  global cumEnergInHot
  global nbLArNoisyRO_Std
  global nbLArNoisyRO_SatTight
  if (tree.LArFlags & 8):nbNoiseBurstVeto[tree.LumiBlockN] = nbNoiseBurstVeto[tree.LumiBlockN] + 1
  # Fill the list of coordinates depending on the object type
  if ("TopoCluster" in objectType):
    coordEnergy = [[tree.TopoClusterEt1,tree.TopoClusterEta1,tree.TopoClusterPhi1]]
  if ("Jet" in objectType):
    coordEnergy = [[tree.JetPt1,tree.JetEta1,tree.JetPhi1],[tree.JetPt2,tree.JetEta2,tree.JetPhi2],[tree.JetPt3,tree.JetEta3,tree.JetPhi3],[tree.JetPt4,tree.JetEta4,tree.JetPhi4],[tree.JetPt5,tree.JetEta5,tree.JetPhi5],[tree.JetPt6,tree.JetEta6,tree.JetPhi6]]
  if ("LoosePhoton" in objectType):
    coordEnergy = [[tree.LoosePhotonPt1,tree.LoosePhotonEta1,tree.LoosePhotonPhi1],[tree.LoosePhotonPt2,tree.LoosePhotonEta2,tree.LoosePhotonPhi2]]
  if ("Tau" in objectType):
    coordEnergy = [[tree.TauJetPt1,tree.TauJetEta1,tree.TauJetPhi1],[tree.TauJetPt2,tree.TauJetEta2,tree.TauJetPhi2]]
  if ("MET" in objectType):
    coordEnergy = [[tree.CellMissingET,0,tree.CellMissingETPhi]]
  # Loop on the list of coordinates to analyze them
  for iCE in coordEnergy:
    if (iCE[0] > thresholdE):
      if ("MET" in objectType): # Fill general map
        h0map.Fill(iCE[2])
        if not (tree.LArFlags & 8):
          h0mapClean.Fill(iCE[2])
      else:
        h0map.Fill(iCE[1],iCE[2])
        if not (tree.LArFlags & 8):
          h0mapClean.Fill(iCE[1],iCE[2])
      if not ((not options.larcleaning) and tree.LArFlags & 8): # Explicit cut LArEventInfo != ERROR to remove noise bursts is larcleaning is false
        if (fabs(iCE[1]-etaSpot)<deltaSpot and fabs(iCE[2]-phiSpot)<deltaSpot): # A candidate has been found in suspicious region - Explict cut LArEventInfo != ERROR to remove noise bursts
#        if (fabs(iCE[1]-etaSpot)<deltaSpot and fabs(iCE[2]-phiSpot)<deltaSpot and not (tree.LArFlags & 8)): # A candidate has been found in suspicious region - Explict cut LArEventInfo != ERROR to remove noise bursts
          nbHitInHot[tree.LumiBlockN]=nbHitInHot[tree.LumiBlockN] + 1
          cumEnergInHot[tree.LumiBlockN]=cumEnergInHot[tree.LumiBlockN]+iCE[0]
          if (tree.LArFlags & 1): nbLArNoisyRO_Std[tree.LumiBlockN] = nbLArNoisyRO_Std[tree.LumiBlockN] + 1
          if (tree.LArFlags & 4): nbLArNoisyRO_SatTight[tree.LumiBlockN] = nbLArNoisyRO_SatTight[tree.LumiBlockN] + 1

  return
  

# Main===========================================================================================================
usage = 'usage: %prog [options] arg'
parser = OptionParser(usage)
parser.add_option('-r','--run',type='str',dest='run',default='',help='Run number',action='store')
parser.add_option('-s','--stream',type='str',dest='streams',default='express',help='Data stream : express/CosmicCalo/JetTauEtmiss/Egamma',action='store')
parser.add_option('-a','--amiTag',type='str',dest='ami',default='f',help='ami Tag - Simply set x/f to choose express/bulk processing',action='store')
parser.add_option('-e','--eta',type='float',dest='eta',default='0.',help='Eta of hot spot',action='store')
parser.add_option('-p','--phi',type='float',dest='phi',default='0.',help='Phi of hot spot (or MET bump)',action='store')
parser.add_option('-t','--treshold',type='int',dest='threshold',default='1000',help='Et/pt threshold (in MeV)',action='store')
parser.add_option('-d','--delta',type='float',dest='delta',default='0.1',help='Distance to look around hot spot (or MET bump)',action='store')
parser.add_option('-o','--object',type='string',dest='object',default='TopoCluster',help='TopoCluster/Jet/LoosePhoton/TauJet/MET',action='store')
parser.add_option('-m','--min',type='int',dest='min',default='5',help='Min number of object in a LB',action='store')
parser.add_option('-n','--noplot',dest='noplot',help='Do not plot LB map',action='store_true')
parser.add_option('-l','--larcleaning',dest='larcleaning',help='Ignore LAr cleaning to find hot spot',action='store_true')

parser.print_help()

usage = 'usage: %prog [options] arg'
(options, args) = parser.parse_args()
run = options.run
stream = options.streams
amiTag = options.ami
etaSpot = options.eta
phiSpot = options.phi
deltaSpot = options.delta
objectType = options.object
minInLB = options.min
thresholdE = options.threshold

if ("MET" in objectType):
  etaSpot=0

print '\n'
print '---------------------------------'
print "Investigation on run "+str(run)+"/"+stream+" stream with ami TAG "+amiTag
listOfFiles = pathExtract.returnEosTagPath(run,stream,amiTag)

#listOfFiles=["root://eosatlas//eos/atlas/atlastier0/rucio/data15_comm/express_express/00265573/data15_comm.00265573.express_express.merge.TAG.f581_m1423_m1425/data15_comm.00265573.express_express.merge.TAG.f581_m1423_m1425._0001.1"]

tree = TChain("POOLCollectionTree")

for files in listOfFiles:
  tree.AddFile("root://eosatlas/%s"%(files))

entries = tree.GetEntries()
#entries = 100

nLB=2000
nbHitInHot = [0] * nLB
cumEnergInHot = [0] * nLB
nbNoiseBurstVeto = [0] * nLB
nbLArNoisyRO_Std = [0] * nLB
nbLArNoisyRO_SatTight = [0] * nLB

if ("MET" in objectType):
  h0map = TH1D("map","General map of %s with MET > %d MeV"%(objectType,thresholdE),64,-3.14,3.14)
  h0mapClean = TH1D("mapClean","General map of %s with MET > %d MeV - LArFlags != ERROR"%(objectType,thresholdE),64,-3.14,3.14)
else:
  h0map = TH2D("map","General map of %s with Et/Pt > %d MeV"%(objectType,thresholdE),90,-4.5,4.5,64,-3.14,3.14)
  h0mapClean = TH2D("mapClean","General map of %s with MET > %d MeV - LArFlags != ERROR"%(objectType,thresholdE),90,-4.5,4.5,64,-3.14,3.14)

for jentry in xrange( entries ): # Loop on all events
  if (jentry % 100000 == 0):
    print "%d / %d evnt processed"%(jentry,entries)
  nb = tree.GetEntry( jentry )
  analyzeTree()     

print "I have looked for LBs with at least %d %s in a region of %.2f around (%.2f,%.2f) and Et/Pt > %d MeV"%(minInLB,objectType,deltaSpot,etaSpot,phiSpot,thresholdE)
if (options.larcleaning):
  print "WARNING : The LArCleaning for noise bursts (LArEventInfo != ERROR) has been DEACTIVATED!!!"
else:
  print "The LArCleaning (LArEventInfo != ERROR) for noise bursts has been activated"

nLB_offending = []
lowerLB = 2000
upperLB = 0
for i in range(nLB):
  if nbHitInHot[i]>=minInLB:
    print "LB: %d -> %d hits (LAr flag in this LB : %d veto / In these events : %d Std / %d SatTight)"%(i,nbHitInHot[i],nbNoiseBurstVeto[i],nbLArNoisyRO_Std[i],nbLArNoisyRO_SatTight[i])
    nLB_offending.append(i)
    if i<lowerLB : lowerLB = i
    if i>upperLB : upperLB = i


if (options.larcleaning):
  suffix = "NO LArFlags cut"
else:
  suffix = "LArFlags != ERROR"

# Plot evolution vs LB
if (upperLB>lowerLB): # check that at least one noisy LB was found
  c0 = TCanvas()
  h0Evol = TH1I("h0Evol","Nb of hits in a region of %.2f around (%.2f,%.2f) and Et/Pt > %d MeV - %s"%(deltaSpot,etaSpot,phiSpot,thresholdE,suffix),upperLB-lowerLB+1,lowerLB-0.5,upperLB+0.5)
  h0Evol.SetXTitle("LumiBlock")
  h0Evol_E = TH1I("h0Evol_E","Mean E/pt in a region of %.2f around (%.2f,%.2f) and Et/Pt > %d MeV - %s"%(deltaSpot,etaSpot,phiSpot,thresholdE,suffix),upperLB-lowerLB+1,lowerLB-0.5,upperLB+0.5)
  h0Evol_E.SetXTitle("LumiBlock")
  if ("MET" in objectType):
    h0Evol.SetTitle("Nb of events with MET in a phi region of %.2f around %.2f and MET > %d MeV - %s"%(deltaSpot,phiSpot,thresholdE,suffix))
    h0Evol_E.SetTitle("Mean MET in a phi region of %.2f around %.2f and MET > %d MeV - %s"%(deltaSpot,phiSpot,thresholdE,suffix))  
  for i in range(lowerLB,upperLB+1):
    h0Evol.Fill(i,nbHitInHot[i])
    if (nbHitInHot[i]>0):
      h0Evol_E.Fill(i,cumEnergInHot[i]/nbHitInHot[i])
  c0.Divide(1,2)
  c0.cd(1)
  h0Evol.Draw()
  c0.cd(2)
  h0Evol_E.Draw()
  c0.Update()

c0_2 = TCanvas()
if (objectType == "MET"):
  h0map.Draw()
else:
  h0map.Draw("COLZ")

c0_3 = TCanvas()
if (objectType == "MET"):
  h0mapClean.Draw()
else:
  h0mapClean.Draw("COLZ")

# Plot individual map for each LB. Can be switched off with "-n"
if (not options.noplot):
  canvas = []
  for i in range(min(len(nLB_offending),2)):
    canvas.append(TCanvas())
    iCurrent = len(canvas)-1

    if (options.larcleaning):
      cutC = "1" # Accept all events
    else:
      cutC = "!(LArFlags & 8)" # Reject events flagged with LArEventInfo::ERROR

    if ("TopoCluster" in objectType):
#      tree.Draw("TopoClusterPhi1:TopoClusterEta1 >> h1_%d"%(nLB_offending[i]),"TopoClusterEt1 > %d && LumiBlockN==%d && !(LArFlags & 8)"%(thresholdE,nLB_offending[i]),"COLZ")
      tree.Draw("TopoClusterPhi1:TopoClusterEta1 >> h1_%d"%(nLB_offending[i]),"TopoClusterEt1 > %d && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
    if ("Jet" in objectType):     
      tree.Draw("JetPhi1:JetEta1 >> h1_%d"%(nLB_offending[i]),"JetPt1 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
      tree.Draw("JetPhi2:JetEta2 >> +h1_%d"%(nLB_offending[i]),"JetPt2 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
      tree.Draw("JetPhi3:JetEta3 >> +h1_%d"%(nLB_offending[i]),"JetPt3 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
      tree.Draw("JetPhi4:JetEta4 >> +h1_%d"%(nLB_offending[i]),"JetPt4 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
      tree.Draw("JetPhi5:JetEta5 >> +h1_%d"%(nLB_offending[i]),"JetPt5 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
      tree.Draw("JetPhi6:JetEta6 >> +h1_%d"%(nLB_offending[i]),"JetPt6 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
    if ("MET" in objectType):     
      tree.Draw("CellMissingETPhi >> h1_%d"%(nLB_offending[i]),"CellMissingET > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC))
    if ("LoosePhoton" in objectType):     
      tree.Draw("LoosePhotonPhi1:LoosePhotonEta1 >> h1_%d"%(nLB_offending[i]),"LoosePhotonPt1 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
      tree.Draw("LoosePhotonPhi2:LoosePhotonEta2 >> +h1_%d"%(nLB_offending[i]),"LoosePhotonPt2 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
    if ("Tau" in objectType):     
      tree.Draw("TauJetPhi1:TauJetEta1 >> h1_%d"%(nLB_offending[i]),"TauJetPt1 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")
      tree.Draw("TauJetPhi2:TauJetEta2 >> +h1_%d"%(nLB_offending[i]),"TauJetPt2 > %d  && LumiBlockN==%d && %s"%(thresholdE,nLB_offending[i],cutC),"COLZ")

    canvas[iCurrent].SetGridx(1)
    canvas[iCurrent].SetGridy(1)
#    canvas[iCurrent].SetLogz(1)

    canvas.append(TCanvas())

    if ("TopoCluster" in objectType):
#      tree.Draw("TopoClusterEt1 >> h1Et_%d"%(nLB_offending[i]),"abs(TopoClusterEta1-%.3f) < %.3f && abs(TopoClusterPhi1-%.3f) < %.3f  && LumiBlockN==%d && !(LArFlags & 8)"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i]))
      tree.Draw("TopoClusterEt1 >> h1Et_%d"%(nLB_offending[i]),"abs(TopoClusterEta1-%.3f) < %.3f && abs(TopoClusterPhi1-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
    if ("Jet" in objectType):     
      tree.Draw("JetPt1 >> h1Pt_%d"%(nLB_offending[i]),"abs(JetEta1-%.3f) < %.3f && abs(JetPhi1-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
      tree.Draw("JetPt2 >> +h1Pt_%d"%(nLB_offending[i]),"abs(JetEta2-%.3f) < %.3f && abs(JetPhi2-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
      tree.Draw("JetPt3 >> +h1Pt_%d"%(nLB_offending[i]),"abs(JetEta3-%.3f) < %.3f && abs(JetPhi3-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
      tree.Draw("JetPt4 >> +h1Pt_%d"%(nLB_offending[i]),"abs(JetEta4-%.3f) < %.3f && abs(JetPhi4-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
      tree.Draw("JetPt5 >> +h1Pt_%d"%(nLB_offending[i]),"abs(JetEta5-%.3f) < %.3f && abs(JetPhi5-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
      tree.Draw("JetPt6 >> +h1Pt_%d"%(nLB_offending[i]),"abs(JetEta6-%.3f) < %.3f && abs(JetPhi6-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
    if ("MET" in objectType):     
      tree.Draw("CellMissingET >> h1Pt_%d"%(nLB_offending[i]),"abs(CellMissingETPhi-%.3f) < %.3f  && LumiBlockN==%d && %s"%(phiSpot,deltaSpot,nLB_offending[i],cutC))
    if ("LoosePhoton" in objectType):     
      tree.Draw("LoosePhotonPt1 >> h1Pt_%d"%(nLB_offending[i]),"abs(LoosePhotonEta1-%.3f) < %.3f && abs(LoosePhotonPhi1-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
      tree.Draw("LoosePhotonPt2 >> +h1Pt_%d"%(nLB_offending[i]),"abs(LoosePhotonEta2-%.3f) < %.3f && abs(LoosePhotonPhi2-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
    if ("Tau" in objectType):     
      tree.Draw("TauJetPt1 >> h1Pt_%d"%(nLB_offending[i]),"abs(TauJetEta1-%.3f) < %.3f && abs(TauJetPhi1-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
      tree.Draw("TauJetPt2 >> +h1Pt_%d"%(nLB_offending[i]),"abs(TauJetEta2-%.3f) < %.3f && abs(TauJetPhi2-%.3f) < %.3f  && LumiBlockN==%d && %s"%(etaSpot,deltaSpot,phiSpot,deltaSpot,nLB_offending[i],cutC))
    
if ("Tau" in objectType):     
  print 'WARNING : in recent TAGs, the TauJet were not filled - A double check is welcome: tree.Draw(\"TauJetEta1\")'
