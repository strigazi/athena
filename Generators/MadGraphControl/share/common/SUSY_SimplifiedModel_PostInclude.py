# This comes after all Simplified Model setup files
from MadGraphControl.MadGraphUtils import SUSY_Generation,modify_param_card,check_reset_proc_number

# Set maximum number of events if the event multiplier has been modified
if evt_multiplier>0:
    if runArgs.maxEvents>0:
        nevts=runArgs.maxEvents*evt_multiplier
    else:
        nevts=evgenConfig.nEventsPerJob*evt_multiplier
else:
    # Sensible default
    nevts=evgenConfig.nEventsPerJob*2.
run_settings.update({'nevents':nevts})

# Only needed for something non-standard (not 1/4 heavy mass)
if ktdurham is not None:
    run_settings.update({'ktdurham':ktdurham})

# systematic variation
if 'scup' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[0]
elif 'scdw' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[1]
elif 'alup' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[2]
elif 'aldw' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[3]
elif 'qcup' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[6]
elif 'qcdw' in runArgs.jobConfig[0]:
    syst_mod=dict_index_syst[7]

# Pass arguments as a dictionary: the "decays" argument is not accepted in older versions of MadGraphControl
argdict = {'runArgs'        : runArgs,
           'process'        : process,
           'params'         : {'MASS':masses},
           'fixEventWeightsForBridgeMode': fixEventWeightsForBridgeMode,
           'madspin_card'   : madspin_card,
           'keepOutput'     : keepOutput, # Should only ever be true for testing!
           'run_settings'   : run_settings, # All goes into the run card
           'writeGridpack'  : writeGridpack,
           'syst_mod'       : syst_mod,
           'param_card'     : param_card # Only set if you *can't* modify the default param card to get your settings
           }
# Add decays in if needed
if len(decays)>0: argdict['params']['DECAY']=decays

# First the standard case: No input LHE file
if not hasattr(runArgs,'inputGeneratorFile') or runArgs.inputGeneratorFile is None:
    # Try-except block to handle grid pack generation
    try:
        ktdurham = SUSY_Generation(**argdict)
    except RuntimeError as rte:
        for an_arg in rte.args:
            if 'Gridpack sucessfully created' in an_arg:
                print 'Handling exception and exiting'
                theApp.finalize()
                theApp.exit()
        print 'Unhandled exception - re-raising'
        raise rte

else:
    # These manipulations require a dummy SUSY process
    from MadGraphControl.MadGraphUtils import new_process,update_lhe_file,add_madspin,arrange_output,SUSY_process
    process_dir = new_process(SUSY_process('generate p p > go go'))
    modify_param_card(process_dir=process_dir,params={'MASS':masses,'DECAY':decays})
    param_card_old = process_dir+'/Cards/param_card.dat'
    ktdurham = -1
    import tarfile
    myTarball = tarfile.open(runArgs.inputGeneratorFile)
    myEvents = None
    for afile in myTarball.getnames():
        if afile.endswith('.events'): myEvents = afile
    if myEvents is None:
        raise RuntimeError('No input events file found!')
    else:
        events_file = myTarball.extractfile( myEvents )
        update_lhe_file(lhe_file_old=myEvents,param_card_old=param_card_old,masses=masses)
        for aline in events_file:
            if 'ktdurham' in aline and "=" in aline:
                ktdurham = float(aline.split('=')[0].strip())
                break
    myTarball.close()

    if madspin_card is not None:
        # Do a stupid addition of madspin - requires a dummy process
        add_madspin(madspin_card,process_dir=process_dir)
        arrange_output(process_dir=process_dir,saveProcDir=keepOutput,runArgs=runArgs,fixEventWeightsForBridgeMode=fixEventWeightsForBridgeMode)

# Check if we were running multi-core, and if so move back to single core for Pythia8
check_reset_proc_number(opts)

# Pythia8 setup for matching if necessary
njets=max([l.count('j') for l in process.split('\n')])
if njets>0 and hasattr(genSeq,'Pythia8'):
    genSeq.Pythia8.Commands += ["Merging:mayRemoveDecayProducts = on",
                                "Merging:nJetMax = "+str(njets),
                                "Merging:doKTMerging = on",
                                "Merging:TMS = "+str(ktdurham),
                                "Merging:ktType = 1",
                                "Merging:Dparameter = 0.4",
                                "Merging:nQuarksMerge = 4"]

# Configuration for EvgenJobTransforms
#--------------------------------------------------------------
evgenConfig.keywords += ["SUSY"]
