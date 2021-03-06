## Initialise Herwig7 for run with built-in/old-style matrix elements
include("Herwig7_i/Herwig7_BuiltinME.py")

## Provide config information
evgenConfig.generators += ["Herwig7"]
evgenConfig.tune        = "H7.1-Default"
evgenConfig.description = "Herwig7 dijet sample with MMHT2014 PDF and H7.1-Default tune"
evgenConfig.keywords    = ["SM","QCD", "dijet"]
evgenConfig.contact     = ["Daniel Rauch (daniel.rauch@desy.de)"]

## Configure Herwig7
Herwig7Config.me_pdf_commands(order="NLO", name="MMHT2014nlo68cl")
Herwig7Config.shower_pdf_commands(order="NLO", name="MMHT2014nlo68cl")
Herwig7Config.tune_commands()

Herwig7Config.add_commands("""
## ------------------
## Hard process setup
## ------------------
insert /Herwig/MatrixElements/SubProcess:MatrixElements[0] /Herwig/MatrixElements/MEQCD2to2
set /Herwig/UnderlyingEvent/MPIHandler:IdenticalToUE 0
set /Herwig/Cuts/JetKtCut:MinKT 15*GeV
set /Herwig/Cuts/LeptonKtCut:MinKT 0.0*GeV
""")

## run the generator
Herwig7Config.run()
