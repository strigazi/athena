# -*- coding: utf-8 -*-
############################################################
# Additional generation of W'->tb samples with MadGraph5+Pythia8+EvtGen
# Héctor de la Torre hector.de.la.torre.perez@cern.ch
# Model available here: http://feynrules.irmp.ucl.ac.be/wiki/Wprime
############################################################

############################################################
### List of DSIDs (Total of 30)
############################################################
### Semi-leptonic top decays
#    * 310503-310504: W'R, g'/g=1, m(W')=6000-7000GeV per step of 1000GeV (2 DSID)
#    * 306123-306126: W'R, g'/g=1, m(W')=3500-5000GeV per step of 500GeV (4 DSID)
#    * 306127-306132: W'L, g'/g=1, m(W')=2250-3000GeV per step of 250GeV and m(W')=3500-5000GeV per step of 500GeV (2500GeV  and 3000GeV excluded, previously generated) (6 DSID)
############################################################
### Fully-hadronic top decays
#    * 306133-306142: W'R, g'/g=1, m(W')=1000-3000GeV per step of 250GeV and m(W')=3500-5000GeV per step of 500GeV (1500GeV, 2500GeV and 3500GeV excluded, previously generated) (10 DSID)
#    * 306143-306152: W'L, g'/g=1, m(W')=1000-3000GeV per step of 250GeV m(W')=3500-5000GeV per step of 500GeV (1500GeV, 2500GeV and 3500GeV excluded, previously generated) (10 DSID)
############################################################

from MadGraphControl.MadGraphUtils import *

### Number of events generated by MadGraph
# to cope with efficiencies, must be slightly higher than 5000
nevents=6000

mode=0

### Choice of pdf set
# can be "cteq6l1" "nnpdf23lo" "nnpdf30lo" "mstw2008lo" "mmht2014lo"
# default is "nnpdf23lo", as recommended for Madgraph+Pythia8 with A14 tune
PdfSet = "nnpdf23lo"

# Use of more recent pdf sets needs special LHAPDF configuration
if PdfSet=="mmht2014lo" or PdfSet=="nnpdf30lo":
  import os
  os.environ["LHAPATH"]=os.environ["LHAPDF_DATA_PATH"]=os.environ['LHAPATH'].split(':')[0]+":/cvmfs/sft.cern.ch/lcg/external/lhapdfsets/current/"

### Define channel, process, mass
DSID = runArgs.runNumber
massList_lep_WpR = [3500,4000,4500,5000]
massList_lep_WpR_ext = [6000,7000]
massList_lep_WpL = [2250,2750,3500,4000,4500,5000]
massList_had_WpR = [1000,1250,1750,2000,2250,2750,3000,4000,4500,5000]
massList_had_WpL = massList_had_WpR
gList_M3000 = [2,3,4]
gList_M4000 = [4]
if (DSID>=302713 and DSID<=302723):
  channel="lep" # top decay channel
  process="Wp" # generated process
  chirname="R" # "chirality" of the Wprime
  MWp=500+(DSID%302713)*250 # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=302724 and DSID<=302730):
  channel="lep" # top decay channel
  process="WpSTs" # generated process
  chirname="L" # "chirality" of the Wprime
  MWp=500+(DSID%302724)*250 # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=302731 and DSID<=302733):
  channel="lep" # top decay channel
  process="Wp" # generated process
  chirname="L" # "chirality" of the Wprime
  MWp=2000+(DSID%302731)*500 # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=302734 and DSID<=302736):
  channel="had" # top decay channel
  process="Wp" # generated process
  chirname="R" # "chirality" of the Wprime
  MWp=1500+(DSID%302734)*1000 # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=302737 and DSID<=302739):
  channel="had" # top decay channel
  process="Wp" # generated process
  chirname="L" # "chirality" of the Wprime
  MWp=1500+(DSID%302737)*1000 # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=306123 and DSID<=306126):
  channel="lep" # top decay channel
  process="Wp" # generated process
  chirname="R" # "chirality" of the Wprime
  MWp=massList_lep_WpR[DSID%306123] # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=306127 and DSID<=306132):
  channel="lep" # top decay channel
  process="Wp" # generated process
  chirname="L" # "chirality" of the Wprime
  MWp=massList_lep_WpL[DSID%306127] # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=306133 and DSID<=306142):
  channel="had" # top decay channel
  process="Wp" # generated process
  chirname="R" # "chirality" of the Wprime
  MWp=massList_had_WpR[DSID%306133] # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=306143 and DSID<=306152):
  channel="had" # top decay channel
  process="Wp" # generated process
  chirname="L" # "chirality" of the Wprime
  MWp=massList_had_WpL[DSID%306143] # mass of the Wprime
  gSF=1 # g'/g
elif (DSID>=308088 and DSID<=308090):
  channel="lep" # top decay channel
  process="Wp" # generated process
  chirname="R" # "chirality" of the Wprime
  MWp=3000 # mass of the Wprime
  gSF=gList_M3000[DSID%308088] # g'/g
elif (DSID>=308091 and DSID<=308091):
  channel="lep" # top decay channel
  process="Wp" # generated process
  chirname="R" # "chirality" of the Wprime
  MWp=4000 # mass of the Wprime
  gSF=gList_M4000[DSID%308091] # g'/g
elif (DSID>=310503 and DSID<=310504):
  channel="lep" # top decay channel
  process="Wp" # generated process
  chirname="R" # "chirality" of the Wprime
  MWp=massList_lep_WpR_ext[DSID%310503] # mass of the Wprime
  gSF=1 # g'/g
else:
  raise RuntimeError("runNumber %i not recognised in this jobOptions."%DSID)

### Define the complete process name, without top decay channel
if (process=="WpSTs"):
  fullprocname = "Wp" + chirname + "STs_tb"
elif (process=="Wp"):
  fullprocname = "Wp" + chirname + "_tb"
else:
  raise RuntimeError("process %s not recognised in this jobOptions."%process)

### Set proc_card
# 4 cases: semi-leptonic or fully-hadronic top decay, including or not SM s-channel single-top
# top-quark always decaying into tb
if (process=="Wp") and (channel=="lep"): # W' only, leptonic
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
    import model WEff_UFO
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    generate p p > t b~ QED=0 NP=2, (t > b W+, W+ > l+ vl)
    add process p p > t~ b QED=0 NP=2, (t~ > b~ W-, W- > l- vl~)
    output -f
    """)
    fcard.close()
elif (process=="WpSTs") and (channel=="lep"): # W'L+ST s-chan, leptonic
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
    import model WEff_UFO
    define p = g u c d s u~ c~ d~ s~
    define j = g u c d s u~ c~ d~ s~
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    generate p p > t b~ QED=2 NP=2, (t > b W+, W+ > l+ vl)
    add process p p > t~ b QED=2 NP=2, (t~ > b~ W-, W- > l- vl~)
    output -f
    """)
    fcard.close()
elif (process=="Wp") and (channel=="had"): # W' only, hadronic
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
    import model WEff_UFO
    define p = g u c d s u~ c~ d~ s~
    define j = u c d s u~ c~ d~ s~
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    generate p p > t b~ QED=0 NP=2, (t > b W+, W+ > j j)
    add process p p > t~ b QED=0 NP=2, (t~ > b~ W-, W- > j j)
    output -f
    """)
    fcard.close()
elif (process=="WpSTs") and (channel=="had"): # W'L+ST s-chan, hadronic
    fcard = open('proc_card_mg5.dat','w')
    fcard.write("""
    import model WEff_UFO
    define p = g u c d s u~ c~ d~ s~
    define j = u c d s u~ c~ d~ s~
    define l+ = e+ mu+ ta+
    define l- = e- mu- ta-
    define vl = ve vm vt
    define vl~ = ve~ vm~ vt~
    generate p p > t b~ QED=2 NP=2, (t > b W+, W+ > j j)
    add process p p > t~ b QED=2 NP=2, (t~ > b~ W-, W- > j j)
    output -f
    """)
    fcard.close()
else:
    raise RuntimeError("process %s and/or channel %s recognised in these jobOptions."%process%channel)

### Setting beam energy from centre-of-mass energy
beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else:
    raise RuntimeError("No centre-of-mass energy found.")

### Set parameters different from the ones in default run_card
extras = {  'lhe_version':'2.0',
	    'cut_decays' :'F',
	    'parton_shower':'PYTHIA8',
	    'xqcut':0.,
	    'bwcutoff':25,
	    'ptj':0.,
	    'ptl':0.,
	    'etaj':-1,
	    'etab':-1,
	    'etal':-1,
	    'drjj':0.,
	    'drll':0.,
	    'drjl':0.,
	    'auto_ptj_mjj':'T',
	    }
if PdfSet=="cteq6l1": #CTEQ6L1
  extras['pdlabel']="'cteq6l1'"
elif PdfSet=="nnpdf23lo": #NNPDF23LO
  extras['pdlabel']="'nn23lo'"
elif PdfSet=="mstw2008lo": #MSTW2008lo68cl
  extras['pdlabel']="'lhapdf'"
  extras['lhaid']=21000
elif PdfSet=="mmht2014lo": #MMHT2014lo68cl
  extras['pdlabel']="'lhapdf'"
  extras['lhaid']=25000
elif PdfSet=="nnpdf30lo": #NNPDF30_lo_as_0118
  extras['pdlabel']="'lhapdf'"
  extras['lhaid']=262000
else: # raise error if no pdf was provided
  raise RuntimeError("ERROR: No PDF has been choosen! Please pick one among those available!")

### Building run_card
build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat',
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,extras=extras)

### W masses and width (NLO)
if (gSF == 1):
  if (MWp == 500):
      WWpL = 17
      WWpR = 12
  elif (MWp == 750):
      WWpL = 26
      WWpR = 19
  elif (MWp == 1000):
      WWpL = 34
      WWpR = 26
  elif (MWp == 1250):
      WWpL = 43
      WWpR = 32
  elif (MWp == 1500):
      WWpL = 52
      WWpR = 39
  elif (MWp == 1750):
      WWpL = 60
      WWpR = 46
  elif (MWp == 2000):
      WWpL = 69
      WWpR = 52
  elif (MWp == 2250):
      WWpL = 78
      WWpR = 59
  elif (MWp == 2500):
      WWpL = 86
      WWpR = 65
  elif (MWp == 2750):
      WWpL = 95
      WWpR = 72
  elif (MWp == 3000):
      WWpL = 104
      WWpR = 78
  elif (MWp == 3250):
      WWpL = 112
      WWpR = 85
  elif (MWp == 3500):
      WWpL = 121
      WWpR = 91
  elif (MWp == 3750):
      WWpL = 130
      WWpR = 98
  elif (MWp == 4000):
      WWpL = 138
      WWpR = 104
  elif (MWp == 4500):
      WWpL = 155
      WWpR = 117
  elif (MWp == 5000):
      WWpL = 173
      WWpR = 130
  elif (MWp == 6000):
      WWpL = 207
      WWpR = 156
  elif (MWp == 7000):
      WWpL = 242
      WWpR = 182
  else:
      raise RuntimeError("mass m(W')=%i not recognised for coupling g'/g=%i in these jobOptions."%(MWp,gSF))
elif (gSF == 2):
  if (MWp == 1750):
      WWpL = 60
      WWpR = 46
  elif (MWp == 2000):
      WWpL = 69
      WWpR = 52
  elif (MWp == 2250):
      WWpL = 78
      WWpR = 59
  if (MWp == 2500):
      WWpL = 86
      WWpR = 65
  elif (MWp == 2750):
      WWpL = 95
      WWpR = 72
  elif (MWp == 3000):
      WWpL = 104
      WWpR = 78
  else:
      raise RuntimeError("mass m(W')=%i not recognised for coupling g'/g=%i in these jobOptions."%(MWp,gSF))
elif (gSF == 3):
  if (MWp == 500):
      WWpL = 17
      WWpR = 12
  elif (MWp == 1500):
      WWpL = 52
      WWpR = 39
  elif (MWp == 2000):
      WWpL = 69
      WWpR = 52
  elif (MWp == 2500):
      WWpL = 86
      WWpR = 65
  if (MWp == 2750):
      WWpL = 95
      WWpR = 72
  elif (MWp == 3000):
      WWpL = 104
      WWpR = 78
  else:
      raise RuntimeError("mass m(W')=%i not recognised for coupling g'/g=%i in these jobOptions."%(MWp,gSF))
elif (gSF == 4):
  if (MWp == 2250):
      WWpL = 78
      WWpR = 59
  elif (MWp == 2500):
      WWpL = 86
      WWpR = 65
  elif (MWp == 2750):
      WWpL = 95
      WWpR = 72
  if (MWp == 3000):
      WWpL = 104
      WWpR = 78
  elif (MWp == 3500):
      WWpL = 121
      WWpR = 91
  elif (MWp == 4000):
      WWpL = 138
      WWpR = 104
  else:
      raise RuntimeError("mass m(W')=%i not recognised for coupling g'/g=%i in these jobOptions."%(MWp,gSF))
elif (gSF == 5):
  if (MWp == 500):
      WWpL = 17
      WWpR = 12
  elif (MWp == 1500):
      WWpL = 52
      WWpR = 39
  elif (MWp == 2500):
      WWpL = 86
      WWpR = 65
  elif (MWp == 2750):
      WWpL = 95
      WWpR = 72
  elif (MWp == 3000):
      WWpL = 104
      WWpR = 78
  if (MWp == 3500):
      WWpL = 121
      WWpR = 91
  elif (MWp == 4000):
      WWpL = 138
      WWpR = 104
  else:
      raise RuntimeError("mass m(W')=%i not recognised for coupling g'/g=%i in these jobOptions."%(MWp,gSF))
else:
  raise RuntimeError("coupling g'/g=%i not recognised in this jobOptions."%gSF)

### SM coupling strength
gSM = (6.483972e-01)

### "chirality" of W'
if (chirname=="R"):
    gL = 0
    gR = gSM*gSF
    WWp = WWpR*(gR/gSM)**2
elif (chirname=="L"):
    gL = gSM*gSF
    gR = 0
    WWp = WWpL*(gL/gSM)**2
else:
  raise RuntimeError("'chirality' %s not recognised in this jobOptions."%chirname)

### Grab default param_card and update its parameters with the ones prepared above
paramcard = subprocess.Popen(['get_files','-data','param_card.Wprime_tb.dat'])
paramcard.wait()
if not os.access('param_card.Wprime_tb.dat',os.R_OK):
    raise RuntimeError("ERROR: Could not get param_card.Wprime_tb.dat")
else:
    oldcard = open('param_card.Wprime_tb.dat','r')
    newcard = open('param_card.dat','w')
    for line in oldcard:
        if ' MWp' in line:
            newcard.write('    34 %f # MWp \n'%(MWp))
        elif ' WWp' in line:
            newcard.write('DECAY  34 %f # WWp \n'%(WWp))
        elif ' gL' in line:
            newcard.write('    1 %f # gL \n'%(gL))
        elif ' gR' in line:
            newcard.write('    2 %f # gR \n'%(gR))
        else:
            newcard.write(line)
    oldcard.close()
    newcard.close()

### Printing a summary of the chosen parameters
summary="INFO: Events will be generated for "
if (process=="Wp"):
  summary+="W'->tb, "
elif (process=="WpSTs"):
  summary+="W'->tb + s-channel single-top, "
else:
  raise RuntimeError("process %s not recognised in this jobOptions."%process)
if (chirname=="R"):
  summary+="'right' Wprime, "
elif (chirname=="L"):
  summary+="'left' Wprime, "
else:
  raise RuntimeError("'chirality' %s not recognised in this jobOptions."%chirname)
if (channel=="lep"):
  summary += "leptonic channel"
elif (channel=="had"):
  summary += "hadronic channel"
else:
  raise RuntimeError("channel %s not recognised in this jobOptions."%channel)
summary+=", MWp="+str(MWp)+" GeV"
summary+=", WWp="+str(WWp)+" GeV"
summary+=", gSF="+str(gSF)
summary+=", gL="+str(gL)
summary+=", gR="+str(gR)
mglog.info(summary)

### Printing cards
mglog.info('Now printing cards')
if os.access('proc_card_mg5.dat',os.R_OK):
  mglog.info("proc_card_mg5.dat:")
  procC = subprocess.Popen(['cat','proc_card_mg5.dat'])
  procC.wait()
else:
  mglog.warning('No proc_card_mg5.dat found')
if os.access('run_card.dat',os.R_OK):
  mglog.info("run_card.dat:")
  runC = subprocess.Popen(['cat','run_card.dat'])
  runC.wait()
else:
  mglog.warning('No run_card.dat found')
if os.access('param_card.dat',os.R_OK):
  mglog.info("param_card.dat:")
  runC = subprocess.Popen(['cat','param_card.dat'])
  runC.wait()
else:
  mglog.warning('No param_card.dat found')

### Define directory name
runName = 'group.phys-gener.MadGraph.'+str(DSID)+'.'+fullprocname+channel
if gSF !=1:
  runName += '_g'+str(gSF)
runName += '_M'+str(MWp)

### Make new process directory
process_dir = new_process()

### Generate events using run_card and param_card prepared above
generate(run_card_loc='run_card.dat',param_card_loc='param_card.dat',mode=mode,proc_dir=process_dir,run_name=runName)

### Arrange output file with usual naming convention
arrange_output(run_name=runName,proc_dir=process_dir,outputDS=runName+'._00001.events.tar.gz')

### Preparing output parameters
evgenConfig.description    = "MadGraph5+Pythia8+EvtGen for Wprime->tb"
evgenConfig.keywords       = ["Wprime", "top", "singleTop", "sChannel", "BSMtop", "exotic", "BSM"]
evgenConfig.inputfilecheck = "Wp"
evgenConfig.minevents      = 5000
evgenConfig.contact        = ["hector.de.la.torre.perez@cern.ch"]

### Define input generator file
runArgs.inputGeneratorFile=runName+'._00001.events.tar.gz'

### To run MadGraph in multi-core mode - if so, need to disactivate it now for Pythia8
if 'ATHENA_PROC_NUMBER' in os.environ:
  evgenLog.info('Noticed that you have run with an athena MP-like whole-node setup.  Will re-configure now to make sure that the remainder of the job runs serially.')
  njobs = os.environ.pop('ATHENA_PROC_NUMBER')
  # Try to modify the opts underfoot
  if not hasattr(opts,'nprocs'): mglog.warning('Did not see option!')
  else: opts.nprocs = 0
  print opts

### Running Pythia8 and EvtGen on input file prepared above
# Using A14 tune with NNPDF23LO PDF set
include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")

### Configure Pythia8 to read input events from an LHEF file
include("Pythia8_i/Pythia8_MadGraph.py")