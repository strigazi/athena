from MadGraphControl.MadGraphUtils import *

# General settings
nevents=150000
mode=0
nJobs=1
gridpack_dir=None
gridpack_mode=False
cluster_type=None
cluster_queue=None

# MG Particle cuts
mllcut=10
mmllmax=40
ptl=5

# Merging settings
maxjetflavor=5
ickkw=0
nJetMax=4
ktdurham=30
dparameter=0.4


### DSID lists (extensions can include filters, systematics samples, etc.)
Zee_5fl_Np0=[361628]
Zee_5fl_Np1=[361629]
Zee_5fl_Np2=[361630]
Zee_5fl_Np3=[361631]
Zee_5fl_Np4=[361632]

Zmumu_5fl_Np0=[361633]
Zmumu_5fl_Np1=[361634]
Zmumu_5fl_Np2=[361635]
Zmumu_5fl_Np3=[361636]
Zmumu_5fl_Np4=[361637]

Ztautau_5fl_Np0=[361638]
Ztautau_5fl_Np1=[361639]
Ztautau_5fl_Np2=[361640]
Ztautau_5fl_Np3=[361641]
Ztautau_5fl_Np4=[361642]

### Electrons
if runArgs.runNumber in Zee_5fl_Np0:
    mgproc="generate p p > e+ e- @0"
    name='Zee_Np0'
    process="pp>e+e-"
elif runArgs.runNumber in Zee_5fl_Np1:
    mgproc="generate p p > e+ e- j @1"
    name='Zee_Np1'
    process="pp>e+e-"
elif runArgs.runNumber in Zee_5fl_Np2:
    mgproc="generate p p > e+ e- j j @2"
    name='Zee_Np2'
    process="pp>e+e-"
elif runArgs.runNumber in Zee_5fl_Np3:
    mgproc="generate p p > e+ e- j j j @3"
    name='Zee_Np3'
    process="pp>e+e-"
    
    nevents=5000
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
elif runArgs.runNumber in Zee_5fl_Np4:
    mgproc="generate p p > e+ e- j j j j @4"
    name='Zee_Np4'
    process="pp>e+e-"
    
    nevents=5000
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20

### Muons    
elif runArgs.runNumber in Zmumu_5fl_Np0:
    mgproc="generate p p > mu+ mu- @0"
    name='Zmumu_Np0'
    process="pp>mu+mu-"
elif runArgs.runNumber in Zmumu_5fl_Np1:
    mgproc="generate p p > mu+ mu- j @1"
    name='Zmumu_Np1'
    process="pp>mu+mu-"
elif runArgs.runNumber in Zmumu_5fl_Np2:
    mgproc="generate p p > mu+ mu- j j @2"
    name='Zmumu_Np2'
    process="pp>mu+mu-"
elif runArgs.runNumber in Zmumu_5fl_Np3:
    mgproc="generate p p > mu+ mu- j j j @3"
    name='Zmumu_Np3'
    process="pp>mu+mu-"
    
    nevents=5000
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
elif runArgs.runNumber in Zmumu_5fl_Np4:
    mgproc="generate p p > mu+ mu- j j j j @4"
    name='Zmumu_Np4'
    process="pp>mu+mu-"
    
    nevents=5000

    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
    
### Taus
elif runArgs.runNumber in Ztautau_5fl_Np0:
    mgproc="generate p p > ta+ ta- @0"
    name='Ztautau_Np0'
    process="pp>ta+ta-"
elif runArgs.runNumber in Ztautau_5fl_Np1:
    mgproc="generate p p > ta+ ta- j @1"
    name='Ztautau_Np1'
    process="pp>ta+ta-"
elif runArgs.runNumber in Ztautau_5fl_Np2:
    mgproc="generate p p > ta+ ta- j j @2"
    name='Ztautau_Np2'
    process="pp>ta+ta-"
elif runArgs.runNumber in Ztautau_5fl_Np3:
    mgproc="generate p p > ta+ ta- j j j @3"
    name='Ztautau_Np3'
    process="pp>ta+ta-"
    
    nevents=5000
    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
elif runArgs.runNumber in Ztautau_5fl_Np4:
    mgproc="generate p p > ta+ ta- j j j j @4"
    name='Ztautau_Np4'
    process="pp>ta+ta-"
    
    nevents=5000

    gridpack_mode=True
    gridpack_dir='madevent/'

    mode=1
    cluster_type='pbs'
    cluster_queue='medium'
    nJobs=20
    
else: 
    raise RuntimeError("runNumber %i not recognised in these jobOptions."%runArgs.runNumber)


stringy = 'madgraph.'+str(runArgs.runNumber)+'.MadGraph_'+str(name)


fcard = open('proc_card_mg5.dat','w')
fcard.write("""
import model sm-no_b_mass
define p = g u c d s b u~ c~ d~ s~ b~
define j = g u c d s b u~ c~ d~ s~ b~
"""+mgproc+"""
output -f
""")
fcard.close()


beamEnergy=-999
if hasattr(runArgs,'ecmEnergy'):
    beamEnergy = runArgs.ecmEnergy / 2.
else: 
    raise RuntimeError("No center of mass energy found.")




#Fetch default LO run_card.dat and set parameters
extras = { 'lhe_version'  : '2.0',
           'cut_decays'   : 'F', 
#           'pdlabel'      : "'nn23lo1'",
           'pdlabel'      : "'lhapdf'",
           'lhaid'        : 247000,
           'maxjetflavor' : maxjetflavor,
           'asrwgtflavor' : maxjetflavor,
           'ickkw'        : 0,
           'ptj'          : 20,
           'ptb'          : 20,
           'mmll'         : mllcut,
           'mmllmax'      : mmllmax,      
           'mmjj'         : 0,
           'drjj'         : 0,
           'drll'         : 0,
           'drjl'         : 0.4,
           'ptl'          : ptl,
           'etal'         : 10,
           'etab'         : 6,
           'etaj'         : 6,
           'ktdurham'     : ktdurham,    
           'dparameter'   : dparameter  }

build_run_card(run_card_old=get_default_runcard(),run_card_new='run_card.dat', 
               nevts=nevents,rand_seed=runArgs.randomSeed,beamEnergy=beamEnergy,xqcut=0.,
               extras=extras)

print_cards()
process_dir = new_process(grid_pack=gridpack_dir)
generate(run_card_loc='run_card.dat',param_card_loc=None,mode=mode,njobs=nJobs,proc_dir=process_dir,
         grid_pack=gridpack_mode,gridpack_dir=gridpack_dir,cluster_type=cluster_type,cluster_queue=cluster_queue,
         nevents=nevents,random_seed=runArgs.randomSeed)
arrange_output(proc_dir=process_dir,outputDS=stringy+'._00001.events.tar.gz')



#### Shower 
evgenConfig.description = 'MadGraph_'+str(name)
evgenConfig.keywords+=['Z','electron','jets','drellYan']
evgenConfig.inputfilecheck = stringy
runArgs.inputGeneratorFile=stringy+'._00001.events.tar.gz'

include("Pythia8_i/Pythia8_A14_NNPDF23LO_EvtGen_Common.py")    
include("Pythia8_i/Pythia8_MadGraph.py")


PYTHIA8_nJetMax=nJetMax
PYTHIA8_TMS=ktdurham
PYTHIA8_Dparameter=dparameter
PYTHIA8_Process=process                                                      
PYTHIA8_nQuarksMerge=maxjetflavor
include("Pythia8_i/Pythia8_CKKWL_kTMerge.py")                   




