#########################################################################
#       preInclude.Qball.py - Wendy Taylor 23 Jun 2011      #
#########################################################################

def load_files_for_qball_scenario(MASS, CHARGE):
    import os, shutil, sys

    CODE=10000000+int(float(CHARGE)*100)

    ALINE1="M {code}                         {intmass}.E+03       +0.0E+00 -0.0E+00 Qball           +".format(code=CODE,intmass=int(MASS))
    ALINE2="W {code}                         0.E+00         +0.0E+00 -0.0E+00 Qball           +".format(code=CODE)
    BLINE1="{code}  {intmass}.00  {charge}  0.0 # Qball".format(code=CODE,intmass=int(MASS), charge=CHARGE)
    BLINE2="-{code}  {intmass}.00  -{charge}  0.0 # QballBar".format(code=CODE,intmass=int(MASS), charge=CHARGE)

    pdgmod = os.path.isfile('PDGTABLE.MeV')
    if pdgmod is True:
        os.remove('PDGTABLE.MeV')
    os.system('get_files -data PDGTABLE.MeV')
    f=open('PDGTABLE.MeV','a')
    f.writelines(str(ALINE1))
    f.writelines('\n')
    f.writelines(str(ALINE2))
    f.writelines('\n')
    f.close()
    partmod = os.path.isfile('particles.txt')
    if partmod is True:
        os.remove('particles.txt')
    f=open('particles.txt','w')
    f.writelines(str(BLINE1))
    f.writelines('\n')
    f.writelines(str(BLINE2))
    f.writelines('\n')
    f.close()

    del ALINE1
    del ALINE2
    del BLINE1
    del BLINE2

doG4SimConfig = True
from AthenaCommon.AthenaCommonFlags import athenaCommonFlags
import PyUtils.AthFile as af
try:
    f = af.fopen(athenaCommonFlags.FilesInput()[0])

    if "StreamHITS" in f.infos["stream_names"]:
        from Digitization.DigitizationFlags import digitizationFlags
        simdict = digitizationFlags.specialConfiguration.get_Value()
        doG4SimConfig = False
    else:
        from G4AtlasApps import AtlasG4Eng
        simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration
except:
    from G4AtlasApps import AtlasG4Eng
    simdict = AtlasG4Eng.G4Eng.Dict_SpecialConfiguration

assert "MASS" in simdict
assert "CHARGE" in simdict
load_files_for_qball_scenario(simdict["MASS"], simdict["CHARGE"])

if doG4SimConfig:
    from G4AtlasApps import AtlasG4Eng
    AtlasG4Eng.G4Eng.log.info("Unlocking simFlags.EquationOfMotion to reset the value for Monopole simulation.")
    from G4AtlasApps.SimFlags import simFlags
    # FIXME ideally would include this file early enough, so that the unlocking is not required
    simFlags.EquationOfMotion.unlock()
    simFlags.EquationOfMotion.set_On()
    simFlags.EquationOfMotion.set_Value_and_Lock("MonopoleEquationOfMotion")
    simFlags.G4Stepper.set_Value_and_Lock('ClassicalRK4')

    def monopole_setupg4():
        from G4AtlasApps import PyG4Atlas, AtlasG4Eng
        AtlasG4Eng.G4Eng.load_Lib("Monopole")
    simFlags.InitFunctions.add_function("preInitPhysics", monopole_setupg4)

    if (hasattr(simFlags, 'UseV2UserActions') and simFlags.UseV2UserActions()):
        # MT-compatible UserActions
        from G4AtlasServices.G4AtlasServicesConfig import addAction
        addAction("LooperKillerTool",['Step'],False)
        addAction("HIPKillerTool",['Step'],False)
    else:
        # this configures the non-MT UserActions
        try:
            from G4AtlasServices.G4AtlasUserActionConfig import UAStore
        except ImportError:
            from G4AtlasServices.UserActionStore import UAStore
        from AthenaCommon.CfgGetter import getPublicToolClone
        # use specific configuration
        lkAction = getPublicToolClone("LooperKillerQBall", "LooperKiller", PrintSteps=2, MaxSteps=2000000, VerboseLevel=0)
        UAStore.addAction(lkAction,['Step'])
        # add HIP killer
        UAStore.addAction('HIPKiller',['Step'])

del doG4SimConfig, simdict
