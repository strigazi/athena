# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

""" Trigger specific flags  """


__author__  = 'T. Bold, J. Baines'
__version__=""
__doc__="Trigger specific flags  "


from AthenaCommon.Logging import logging
log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )
log.setLevel(logging.DEBUG)


#import traceback
#stack = traceback.extract_stack()
#log.info( "Imported TriggerFlags from %s, line %i" % (stack[-2][0], stack[-2][1]) )

try:
    from TriggerMenu import useNewTriggerMenu
    useNewTM = useNewTriggerMenu()
    log.info("Using new TriggerMenu: %r" % useNewTM)
except:
    useNewTM = False
    log.info("Using old TriggerMenuPython since TriggerMenu.useNewTriggerMenu can't be imported")


from AthenaCommon.JobProperties import JobProperty, JobPropertyContainer, jobproperties

if useNewTM:
    from TriggerMenu.menu.CommonSliceHelper import AllowedList
else:
    from TriggerMenuPython.CommonSliceHelper import AllowedList

from TrigConfigSvc.TrigConfigSvcUtils import getKeysFromNameRelease, getMenuNameFromDB


_flags = []

# Define Default Flags
class doLVL1(JobProperty):
    """ run the LVL1 simulation (set to FALSE to read the LVL1 result from BS file) """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doLVL1)

class doL1Topo(JobProperty):
    """ Run the L1 Topo simulation (set to FALSE to read the L1 Topo result from BS file) """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doL1Topo)

class readLVL1Calo(JobProperty):
    """  read LVL1 Calo info from pool or BS """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(readLVL1Calo)

class readLVL1Muon(JobProperty):
    """ read LVL1 Muon in from Pool or BS """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(readLVL1Muon)

class fakeLVL1(JobProperty):
    """ create fake RoI from KINE info  """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(fakeLVL1)

class useCaloTTL(JobProperty):
    """ False for DC1. Can use True for Rome files with Digits or post-Rome data """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(useCaloTTL)

class useL1CaloCalibration(JobProperty):
    """ Should be false for early data, true for later """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(useL1CaloCalibration)

class doCosmicSim(JobProperty):
    """ run the LVL1 simulation with special setup for cosmic simulation (set to FALSE by default, to do collisions simulation) """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doCosmicSim)

class disableRandomPrescale(JobProperty):
    """ if True, disable Random Prescales """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(disableRandomPrescale)

class doLVL2(JobProperty):
    """ if False, disable LVL2 selection """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doLVL2)

class doEF(JobProperty):
    """ if False, disable EF selection """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doEF)


class doHLT(JobProperty):
    """ if False, disable HLT selection """
    statusOn=True
    allowedType=['bool']
    StoredValue=True
    
    def _do_action(self):
        """ setup flag level consistency """
        if self.get_Value() is True:
            if TriggerFlags.doEF.is_locked():
                TriggerFlags.doEF.unlock()
                TriggerFlags.doEF.set_Off()
                TriggerFlags.doEF.lock()
            else:
                TriggerFlags.doEF.set_Off()
            if TriggerFlags.doEF.is_locked():
                TriggerFlags.doLVL2.unlock()
                TriggerFlags.doLVL2.set_Off()
                TriggerFlags.doLVL2.lock()
            else:
                TriggerFlags.doLVL2.set_Off()
            log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )
            log.info("doHLT is True: force doLVL2=False and doEF=False"  )

            
_flags.append(doHLT)


class doMergedHLTResult(JobProperty):
    """ if False disable decoding of the merged HLT Result (so decoding L2/EF Result) """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doMergedHLTResult)

class EDMDecodingVersion(JobProperty):
    """ if 1, Run1 decoding version is set; if 2, Run2 """
    statusOn=True
    allowedType=['int']
    StoredValue=2

_flags.append(EDMDecodingVersion)



class doFEX(JobProperty):
    """ if False disable Feature extraction algorithms """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doFEX)

class doHypo(JobProperty):
    """ if False disable all Hypothesis algorithms (HYPO)"""
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doHypo)

class doTruth(JobProperty):
    """ """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doTruth)

# FTK simulation switch

class doFTK(JobProperty):
    """ if False, disable FTK result reader """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doFTK)

# monitoring switch
class enableMonitoring(JobProperty):
    """ enables certain monitoring type: Validation, Online, Time"""
    statusOn=True
    allowedType=['list']
    StoredValue=[]

_flags.append(enableMonitoring)

# trigger configuration source list
class configurationSourceList(JobProperty):
    """ define where to read trigger configuration from. Allowed values: ['xml','aod','ds']"""
    statusOn=True
    allowedType=['list']
    StoredValue=[]
    allowedValues = AllowedList( ['aod','xml','ds'] )

_flags.append(configurationSourceList)

class doTriggerConfigOnly(JobProperty):
    """ if True only the configuration services should be set, no algorithm """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doTriggerConfigOnly)
              
# Flags to switch on/off Detector Slices
class doID(JobProperty):
    """ if False, disable ID algos at LVL2 and EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doID)

class doCalo(JobProperty):
    """ if False, disable Calo algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doCalo)

class doBcm(JobProperty):
    """ if False, disable BCM algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doBcm)

class doTrt(JobProperty):
    """ if False, disable TRT algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doTrt)

class doZdc(JobProperty):
    """ if False, disable ZDC algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doZdc)

class doLucid(JobProperty):
    """ if False, disable Lucid algorithms at LVL2 & EF """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doLucid)

class doMuon(JobProperty):
    """ if FAlse, disable Muons, note: muons need input file containing digits"""
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doMuon)

# Flags to switch on/off physics selection slices
# defaults L2ID, L2Calo, L2Muon, EFID, EFCalo, EFMuon
#         class TauSlice(JobProperty):---""" """ statusOn=True\allowedType=['bool']\nStoredValue=TriggerSliceFlags(L2ID(JobProperty):True, L2Calo(JobProperty):True, L2Muon(JobProperty):False,
#                                         EFID(JobProperty):False, EFCalo(JobProperty):False, EFMuon(JobProperty):False)

#         class JetSlice(JobProperty):TriggerSliceFlags(L2ID(JobProperty):False, L2Calo(JobProperty):False, L2Muon(JobProperty):False,
#                                         EFID(JobProperty):False, EFCalo(JobProperty):False, EFMuon(JobProperty):False)

#         class BphysicsSlice(JobProperty):TriggerSliceFlags(L2ID(JobProperty):False, L2Calo(JobProperty):False, L2Muon(JobProperty):False,
#                                              EFID(JobProperty):False, EFCalo(JobProperty):False, EFMuon(JobProperty):False)

#         class BjetSlice(JobProperty):TriggerSliceFlags(L2ID(JobProperty):False, L2Calo(JobProperty):False, L2Muon(JobProperty):False,
#                                          EFID(JobProperty):False, EFCalo(JobProperty):False, EFMuon(JobProperty):False)

#         class METSlice(JobProperty):TriggerSliceFlags(L2ID(JobProperty):False, L2Calo(JobProperty):False, L2Muon(JobProperty):False,
#                                              EFID(JobProperty):False, EFCalo(JobProperty):False, EFMuon(JobProperty):False)

#         class EgammaSlice(JobProperty):TriggerSliceFlags(L2ID(JobProperty):True, L2Calo(JobProperty):True, L2Muon(JobProperty):False,
#                                            EFID(JobProperty):True, EFCalo(JobProperty):False, EFMuon(JobProperty):False)

#         class MuonSlice(JobProperty):TriggerSliceFlags(L2ID(JobProperty):False, L2Calo(JobProperty):False, L2Muon(JobProperty):False,
#                                          EFID(JobProperty):False, EFCalo(JobProperty):False, EFMuon(JobProperty):False)

#         class mySlice(JobProperty):TriggerSliceFlags(L2ID(JobProperty):False, L2Calo(JobProperty):False, L2Muon(JobProperty):False,
#                                        EFID(JobProperty):False, EFCalo(JobProperty):False, EFMuon(JobProperty):False)


class doHLTpersistency(JobProperty):
    """ serialise L2result """
    statusOn=True
    allowedType=['bool']
    StoredValue=True

_flags.append(doHLTpersistency)

class useOfflineSpacePoints(JobProperty):
    """ use online convertors for Si SpacePoints"""
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(useOfflineSpacePoints)

class doNtuple(JobProperty):
    """ """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(doNtuple)


class writeBS(JobProperty):
    """ """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(writeBS)

class readBS(JobProperty):
    """ """
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(readBS)


class AODEDMSet(JobProperty):
    """ Define which sets of object go to AOD """
    statusOn=True
    allowedType=['list']
    StoredValue='AODSLIM'

_flags.append(AODEDMSet)

class ESDEDMSet(JobProperty):
    """ Define which sets of object go to ESD (or derived ESD) """
    statusOn=True
    allowedType=['list']
    StoredValue='ESD'

_flags.append(ESDEDMSet)

# =========
#
# trigger flags used by trigger configuration
#

class configForStartup(JobProperty):
    """ A temporary flag to determine the actions to be taken for the different cases of HLT running in the startup phase"""
    statusOn=True
    allowedType=['string']
    StoredValue = 'HLTonlineNoL1Thr'
    
    allowedValues = [
        'HLTonline',
        'HLToffline',
        'HLTonlineNoL1Thr',
        'HLTofflineNoL1Thr'
        ]

_flags.append(configForStartup)

class dataTakingConditions(JobProperty):
    """ A flag that describes the conditions of the Trigger at data taking, and determines which part of it will be processed in reconstruction."""
    statusOn=True
    allowedType=['string']
    StoredValue = 'FullTrigger'
    
    allowedValues = [
        'HltOnly',
        'Lvl1Only',
        'FullTrigger',
        'NoTrigger'
        ]

_flags.append(dataTakingConditions)

class triggerUseFrontier(JobProperty):
    """Flag determines if frontier should be used to connect to the oracle database, current default is False"""
    statusOn=True
    allowedType=['bool']
    StoredValue = False
    def _do_action(self):
        log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )
        log.info("Setting TriggerFlags.triggerUseFrontier to %r" % self.get_Value())
        
_flags.append(triggerUseFrontier)



class triggerConfig(JobProperty):
    """ Flag to set various menus and options (read from XML or DB)
    Allowed values:

    Note that we use LVL1 prefix here in order not to touch the
    HLT if we're only running a LVL1 digitization job. The
    prefix is automatically added in the Digi job transform.
    
    NONE or OFF                             -trigger off 

    For digitization (L1) only use LVL1 prefix:
    LVL1:DEFAULT                            -default L1 menu
    LVL1:MenuName                           -takes the L1 xml representation of this menu
    LVL1:DB:connectionstring:SMKey,L1PSKey  -takes these db keys
    LVL1:DB:connectionstring:MenuName,Rel   -takes this menu from the db - not yet supported

    For MC reconstruction use MCRECO prefix:
    MCRECO:DEFAULT                                       -default L1 and HLT menu
    MCRECO:MenuName                                      -takes the L1 and HLT xml respresentations of the menu
    MCRECO:L1CaloCalib=True/False:MenuName               -takes the L1 and HLT xml respresentations of the menu, sets L1 calo calib
    MCRECO:DB:connectionstring:SMKey,L1PSK,HLTPSK[,BGK]  -takes these db keys
    MCRECO:DB:L1CaloCalib=True/False:connectionstring:SMKey,L1PSK,HLTPSK  -takes these db keys, sets L1 calo calib
    MCRECO:DB:connectionstring:MenuName,Rel              -takes this menu from the db (looks up the SMK)
                                                         -NB for the above: move to alias tables?
                                                   
    For data reconstruction: use DATARECO prefix. TO BE IMPLEMENTED. 
    DATARECO:ONLINE
    DATARECO:OFFLINE
    DATARECO:DB:connectionstring:SMKey,L1PSK,HLTPSK
    
    InFile: TO BE IMPLEMENTED

    connectionstring can be one of the following
    1)  <ALIAS>                              -- usually TRIGGERDB or TRIGGERDBMC (generally any string without a colon ':')
    2)  <type>:<detail>                      -- <type> has to be oracle, mysql, or sqlite_file, <detail> is one of the following
    2a) sqlite_file:filename.db              -- an sqlite file, no authentication needed, will be opened in read-only mode
    2b) oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_V2  -- a service description type://server/schema without user and password
    2c) oracle://ATLAS_CONFIG/ATLAS_CONF_TRIGGER_V2;username=ATLAS_CONF_TRIGGER_V2_R;password=<...>  -- a service description with user and password

    Note: specifying :DBF: instead of :DB: will set the trigger flag triggerUseFrontier to true
    """
    
    statusOn=''
    allowedType=['string']
    StoredValue = 'MCRECO:DEFAULT';


    def _do_action(self):
        """ setup some consistency """
        from TriggerJobOpts.TriggerFlags import TriggerFlags as tf
                
        log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )
        log.info("triggerConfig: \""+self.get_Value()+"\"")
        # We split the string passed to the flag
        configs = self.get_Value().split(":")
        
        ## ------
        ##  OFF or NONE: we want to turn everything related to trigger to false (via rec flag)
        ##  Note that this is true for reconstruction only at the moment. For LVL1 Digitization jobs,
        ##  which don't use rec flags, this is still done in the skeleton. Might be changed in future.
        ## ------
        if (configs[0] == 'OFF' or configs[0] == 'NONE'):
            from RecExConfig.RecFlags  import rec
            rec.doTrigger=False
            log.info("triggerConfig: Setting rec.doTrigger to False")
            
            
        ## ------
        ## DATARECO : We deal with data (cosmics, single run, collisions)
        ## ------               
        elif configs[0] == 'DATARECO':
            if configs[1] == 'ONLINE': # We read config from COOL directly
                log.warning("triggerConfig: DATARECO:ONLINE (reco from cool) is not yet implemented. You should not use it.")
            elif configs[1] == 'OFFLINE': # We read config from XML
                log.warning("triggerConfig: DATARECO:OFFLINE (reco from xml) is not yet implemented. You should not use it.")
            elif configs[1] == 'REPR': # We read config from XML
                log.info("triggerConfig: DATARECO:REPR is designed to configure the offline reconstruction in a trigger reprocessing job")
                try:
                    f = open("MenuCoolDbLocation.txt",'r')
                    tf.triggerCoolDbConnection = f.read()
                    f.close()
                except IOError, e:
                    log.fatal("triggerConfig=DATARECO:REPR requires 'MenuCoolDbLocation.tx' to be present in the local directory (reco part of trigger reprocessing)")
                    
            elif configs[1] == 'DB' or configs[1] == 'DBF': # We read config from a private DB
                ### We read the menu from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 3):                            # we got 3 keys (SM, L1PS, HLTPS)
                    tf.triggerDbKeys=[int(x) for x in DBkeys] + [1]
                    log.info("triggerConfig: DATARECO from DB with speficied keys SMK %i, L1 PSK %i, and HLT PSK %i." % tuple(tf.triggerDbKeys()[0:3])   )
                elif (len(DBkeys) == 2):                       # we got a menu name and a release which we need to look up 
                    log.info("triggerConfig: DATARECO from DB with specified menu name and release: finding keys...")
                    tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],False) + [1]
                    log.info("triggerConfig: DATARECO from DB with keys SMK %i, L1 PSK %i, and HLT PSK %i." % tuple(tf.triggerDbKeys()[0:3])   )
                else:
                    log.info("triggerConfig: DATARECO from DB configured with wrong number of keys/arguments" )

        ## ---------
        ##  InFile : We wish to read a file with config info already in it (ESD, AOD, ...)
        ## ---------            
        elif configs[0] == 'InFile': 
            log.warning("triggerConfig: Infile is not yet implemented. You should not use it.")

        ## ------
        ##  LVL1 : For LVL1 simulation only in Digitization job 
        ## ------
        elif configs[0] == 'LVL1':
            if configs[1] == 'DB' or configs[1]=='DBF':
                ### We read config from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 2): #We got either 2 keys (SM, L1PS) or menu name plus release. If latter, second object will contain a .
                    if not '.' in str(DBkeys[1]):
                        tf.triggerDbKeys=[int(x) for x in DBkeys] +[-1,1] # SMkey, L1PSkey, HLTPSkey, BGkey
                        log.info("triggerConfig: LVL1 from DB with specified keys SMK %i and L1 PSK %i." % tuple(tf.triggerDbKeys()[0:2])   )
                    else:
                        log.info("triggerConfig: LVL1 from DB with speficied menu name and release: finding keys...")
                        tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],True) + [-1,1]
                        log.info("triggerConfig: LVl1 from DB with keys SMK %i and L1 PSK %i" % tuple(tf.triggerDbKeys()[0:2])   )
                else:                  #We got a menu name which we need to look up - not implemented yet
                    log.info("triggerConfig: LVL1 from DB configured with wrong number of keys/arguments" )

            else:
                ### We read config from XML
                tf.readLVL1configFromXML=True
                if (configs[1] == 'DEFAULT' or configs[1] == 'default'):
                    tf.triggerMenuSetup = 'default'
                else:
                    tf.triggerMenuSetup = configs[1]
                log.info("triggerConfig: LVL1 menu from xml (%s)" % tf.triggerMenuSetup())

                

        #------
        # MCRECO: Reconstruction of MC
        #------            
        elif configs[0] == 'MCRECO':
            from RecExConfig.RecFlags  import rec
            from RecJobTransforms.RecConfig import recConfig
            rec.doTrigger = True

            if configs[1] == 'DB' or configs[1]=='DBF':
                ### We read the menu from the TriggerDB
                tf.readMenuFromTriggerDb=True
                tf.triggerUseFrontier = (configs[1]=='DBF')
                
                #see if L1 calib arg supplied
                if "L1CaloCalib" in configs[2]:
                    if configs[2].split("=")[-1] == "True" or configs[2].split("=")[-1] == "true":
                        log.info("Setting L1CaloCalib from TriggerConfig command to %s " % configs[2].split("=")[-1])
                        tf.useL1CaloCalibration=True
                    elif configs[2].split("=")[-1] == "False" or configs[2].split("=")[-1] == "false":
                        log.info("Setting L1CaloCalib from TriggerConfig command to %s " % configs[2].split("=")[-1])
                        tf.useL1CaloCalibration=False
                    else:
                        log.warning("Unknown value for L1CaloCalib ('%s'), will use default" % configs[2].split("=")[-1])
                    tf.triggerDbConnection = ':'.join(configs[3:-1])  # the dbconnection goes from third to last ':', it can contain ':'
                else:
                    tf.triggerDbConnection = ':'.join(configs[2:-1])  # the dbconnection goes from second to last ':', it can contain ':'
                DBkeys = configs[-1].split(",")
                if (len(DBkeys) == 4):                            # we got 4 keys (SM, L1PS, HLTPS,BGK)
                    tf.triggerDbKeys=[int(x) for x in DBkeys]
                    log.info("triggerConfig: MCRECO from DB with speficied keys SMK %i, L1 PSK %i, HLT PSK %i, and BGK %i." % tuple(tf.triggerDbKeys()[0:4])   )
                if (len(DBkeys) == 3):                            # we got 3 keys (SM, L1PS, HLTPS)
                    tf.triggerDbKeys=[int(x) for x in DBkeys] + [1]
                    log.info("triggerConfig: MCRECO from DB with speficied keys SMK %i, L1 PSK %i, and HLT PSK %i." % tuple(tf.triggerDbKeys()[0:3])   )
                elif (len(DBkeys) == 2):                       # we got a menu name and a release which we need to look up 
                    log.info("triggerConfig: MCRECO from DB with specified menu name and release: finding keys...")
                    tf.triggerDbKeys=getKeysFromNameRelease(tf.triggerDbConnection(),DBkeys[0],DBkeys[1],False) + [1]
                    log.info("triggerConfig: MCRECO from DB with keys SMK %i, L1 PSK %i, and HLT PSK %i." % tuple(tf.triggerDbKeys()[0:3])   )
                else:
                    log.info("triggerConfig: MCRECO from DB configured with wrong number of keys/arguments" )

                # we need to set triggerMenuSetup to the correct name
                # that we get from the triggerDB, otherwise
                # TriggerGetter->GenerateMenu.generateMenu() would be
                # run with the wrong menu and the configuration of the
                # algorithms would be incorrect (bug 72547)
                tf.triggerMenuSetup=getMenuNameFromDB(tf.triggerDbConnection(),tf.triggerDbKeys()[2])
                log.info("triggerConfig: Setting tf.triggerMenuSetup to " + tf.triggerMenuSetup())
            else:
                ### We read the menu from xml
                tf.readLVL1configFromXML=True
                tf.readHLTconfigFromXML=True
                if "L1CaloCalib" in configs[1]:
                    if configs[1].split("=")[-1] == "True" or configs[1].split("=")[-1] == "true":
                        log.info("Setting L1CaloCalib from TriggerConfig command to %s " % configs[1].split("=")[-1])
                        tf.useL1CaloCalibration=True
                    elif configs[1].split("=")[-1] == "False" or configs[1].split("=")[-1] == "false":
                        log.info("Setting L1CaloCalib from TriggerConfig command to %s " %  configs[1].split("=")[-1])
                        tf.useL1CaloCalibration=False
                    else:
                        log.warning("Unknown value for L1CaloCalib ('%s'), will use default" % configs[1].split("=")[-1])
                if (configs[-1] == 'DEFAULT' or configs[-1] == 'default'):
                    tf.triggerMenuSetup = 'default'
                else:
                    tf.triggerMenuSetup = configs[-1]
                log.info("triggerConfig: MCRECO menu from xml (%s)" % tf.triggerMenuSetup())

            # This part was there in the original (old) csc_reco_trigger.py snippet
            # Still wanted?
            if rec.doTrigger:
                # Switch off trigger of sub-detectors
                for detOff in recConfig.detectorsOff:
                    cmd = 'TriggerFlags.do%s = False' % detOff
                    # possibly not all DetFlags have a TriggerFlag
                    try:
                        exec cmd
                        recoLog.info(cmd)
                    except AttributeError:
                        pass
        #------            
        # We passed a wrong argument to triggerConfig
        #------
        else:
            log.error("triggerConfig argument \""+self.get_Value()+"\" not understood. "
                       + "Please check in TriggerFlags.py to see the allowed values.")
            

_flags.append(triggerConfig)



class readL1TopoConfigFromXML(JobProperty):
    """Use to specify external l1topo xml configuration file
    (e.g. from the release or a local directory)
    
    If set to True:
    the L1Topo config will be taken from TriggerFlags.inputL1TopoConfigFile()
    
    If set to False:    
    the L1Topo config xml file is read from the python generated XML
    file, which is specified in TriggerFlags.outputL1TopoconfigFile()
    """
    statusOn=True
    allowedType=['bool']
    # note: if you change the following default value, you must also change the default value in class inputLVL1configFile
    # StoredValue=False
    StoredValue = False # once the python generation is implemented the default should be False

    def _do_action(self):
        """ setup some consistency """
        if self.get_Value() is False:
            TriggerFlags.inputL1TopoConfigFile = TriggerFlags.outputL1TopoConfigFile()

_flags.append(readL1TopoConfigFromXML)




class readLVL1configFromXML(JobProperty):
    """ If set to True the LVL1 config file is read from earlier generated XML file """
    statusOn=True
    allowedType=['bool']
    # note: if you change the following default value, you must also change the default value in class inputLVL1configFile
    # StoredValue=False
    StoredValue = False

    def _do_action(self):
        """ setup some consistency """
        import os
        log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )

        import TriggerMenu.l1.Lvl1Flags
        
        if self.get_Value() is False:
            TriggerFlags.inputLVL1configFile = TriggerFlags.outputLVL1configFile()
            TriggerFlags.Lvl1.items.set_On()
        else:
            xmlFile=TriggerFlags.inputLVL1configFile()
            from TrigConfigSvc.TrigConfigSvcConfig import findFileInXMLPATH
            if xmlFile!='NONE' and not os.path.exists(findFileInXMLPATH(xmlFile)):
                log.debug("The LVL1 xml file is missing. ")
                nightlyPaths=os.environ['XMLPATH'].split(':')
                
                nightlyDir = [ i.split("/") for i in nightlyPaths if "AtlasTrigger" in  i ][0]
                #print nightlyDir
                OldMenuVersion = nightlyDir[nightlyDir.index('AtlasTrigger')+1]
                log.warning("LVL1 xml file will be taken with the older version: "+OldMenuVersion )
                TriggerFlags.inputLVL1configFile = "LVL1config_"+TriggerFlags.triggerMenuSetup()+"_" + OldMenuVersion + ".xml"

            TriggerFlags.Lvl1.items.set_Off()

_flags.append(readLVL1configFromXML)



class readHLTconfigFromXML(JobProperty):
    """ If set to True the HLT config file is read from earlier generated XMl file """
    statusOn=True
    allowedType=['bool']
    # note: if you change the following default value, you must also change the default value in class inputHLTconfigFile
    # StoredValue=False
    StoredValue = False

    def _do_action(self):
        """ Disable all subcontainers defining slices ON/OFF flags """

        import os
        log = logging.getLogger( 'TriggerJobOpts.TriggerFlags' )

        ## loop over all properties in the container
        # from AthenaCommon.JobProperties import JobPropertyContainer
        # from TriggerJobOpts.TriggerFlags import TriggerFlags
        for prop in TriggerFlags.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                for slice_prop_name in prop.__dict__:
                    slice_prop = prop.__dict__.get(slice_prop_name)
                    if issubclass(slice_prop.__class__, JobProperty):
                        if self.get_Value() == True: ## now depending on the value set flags are on/off
                            slice_prop.set_Off()
                        else:
                            slice_prop.set_On()
        ## in addition set inputLVL1configFile to be the same as outputLVL1configFile
        if self.get_Value() is False:
            TriggerFlags.inputHLTconfigFile = TriggerFlags.outputHLTconfigFile()
        else:
            if TriggerFlags.inputHLTconfigFile != 'NONE':
                TriggerFlags.inputHLTconfigFile = "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
                nightlyPaths=os.environ['XMLPATH'].split(':')

                for p in nightlyPaths:
                    #print p+"/TriggerMenuXML/HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
                    if os.path.exists(p+"/TriggerMenuXML/HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml") is True:
                        log.info("The HLT xml file is found in "+p+"/TriggerMenuXML/")
                        success = True
                        break
                    else:
                        success = False

                if success is False:
                    log.debug("The HLT xml file is missing: HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml")

                    nightlyDir = [ i.split("/") for i in nightlyPaths if "AtlasTrigger" in  i ][0]
                    #print nightlyDir
                    OldMenuVersion = nightlyDir[nightlyDir.index('AtlasTrigger')+1]
                    log.warning("HLT xml file will be taken with the older version: "+OldMenuVersion )
                    TriggerFlags.inputHLTconfigFile = "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + OldMenuVersion + ".xml"

                
_flags.append(readHLTconfigFromXML)


# trigger configuration source list
class readMenuFromTriggerDb(JobProperty):
    """ define the TriggerDb to be the source of the LVL1 and HLT trigger menu"""
    statusOn=False
    allowedType=['bool']
    StoredValue=False
#    def _do_action(self):
#        """ setup reading from DB requires menu readingFromXML """
#        if self.get_Value() is True:
#            TriggerFlags.readLVL1configFromXML = True
#            TriggerFlags.readHLTconfigFromXML = True
_flags.append(readMenuFromTriggerDb)

# trigger configuration source list
class readConfigFromTriggerDb(JobProperty):
    """ define the TriggerDb to be the source of the LVL1 and HLT trigger menu"""
    statusOn=False
    allowedType=['bool']
    StoredValue=False

    def _do_action(self):
        """ setup reading from DB requires menu readingFromXML """
        if self.get_Value() is True:
            # readMenuFromTriggerDb dumps only the HLTMenu to an XML file - it is of no use since HLTConfigSvc is set for the DB
            TriggerFlags.readMenuFromTriggerDb = False
            TriggerFlags.readLVL1configFromXML = False
            TriggerFlags.readHLTconfigFromXML = False

_flags.append(readConfigFromTriggerDb)

class triggerDbKeys(JobProperty):
    """ define the keys [Configuration, LVL1Prescale, HLTPrescale, L1BunchGroupSet] in that order!"""
    statusOn=False
    allowedType=['list']
    StoredValue=[0,0,0,1]

_flags.append(triggerDbKeys)

class triggerDbConnection(JobProperty):
    """ define triggerDB connection parameters"""
    statusOn=False
    allowedType=['dict']
    StoredValue="TRIGGERDB"

_flags.append(triggerDbConnection)

class triggerCoolDbConnection(JobProperty):
    """ define connection parameters to cool if external sqlite file is to be used"""
    statusOn=True
    allowedType=['str']
    StoredValue=''

_flags.append(triggerCoolDbConnection)

class outputL1TopoConfigFile(JobProperty):
    """ File name for output L1Topo configuration XML file produced by the python menu generation """
    statusOn=True
    allowedType=['str']
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            return "L1Topoconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(outputL1TopoConfigFile)

class outputLVL1configFile(JobProperty):
    """ File name for output LVL1 configuration XML file """
    statusOn=True
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            return "LVL1config_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(outputLVL1configFile)

class outputHLTconfigFile(JobProperty):
    """ File name for output HLT configuration XML file """
    statusOn=True
#    allowedType=['str']
    StoredValue=""
    
    def __call__(self):
        if self.get_Value() == "":
            return "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()

_flags.append(outputHLTconfigFile)



class inputL1TopoConfigFile(JobProperty):
    """Used to define an external L1Topo configuration file. To be
    used together with trigger flag readL1TopoConfigFromXML.

    If TriggerFlags.readL1TopoConfigFromXML()==True, then this file is
    used for L1TopoConfiguration.
    
    Defaults to L1TopoConfig_<triggerMenuSetup>_<menuVersion>.xml
    """
    statusOn=True
    allowedType=['str']
    StoredValue=""

    def __call__(self):
        if self.get_Value() == "":
            return "L1TopoConfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(inputL1TopoConfigFile)



class inputLVL1configFile(JobProperty):
    """ File name for input LVL1 configuration XML file """
    statusOn=True
#    allowedType=['str']
#   The following default is appropriate when XML cofig is the default
#    StoredValue="TriggerMenuXML/LVL1config_default_" + TriggerFlags.menuVersion() + ".xml"
#   The following default is appropriate when python config is the default
    StoredValue=""
#    StoredValue = "TriggerMenuXML/LVL1config_default_" + TriggerFlags.menuVersion() + ".xml"

    def __call__(self):
        if self.get_Value() == "":
            return "LVL1config_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(inputLVL1configFile)



class inputHLTconfigFile(JobProperty):
    """ File name for input HLT configuration XML file """
    statusOn=True
    allowedType=['str']
#   The following default is appropriate when XML cofig is the default
#    StoredValue="TriggerMenuXML/HLTconfig_default_" + TriggerFlags.menuVersion() + ".xml"
#   The following default is appropriate when python config is the default
    StoredValue=""
#    StoredValue = "TriggerMenuXML/HLTconfig_default_" + TriggerFlags.menuVersion() + ".xml"

    def __call__(self):
        if self.get_Value() == "":
            return "HLTconfig_"+TriggerFlags.triggerMenuSetup()+"_" + TriggerFlags.menuVersion() + ".xml"
        else:
            return self.get_Value()
        
_flags.append(inputHLTconfigFile)

class abortOnConfigurationError(JobProperty):
    """ Should the job be stoped if there is an error in configuration"""
    statusOn=True
    allowedType=['bool']
    StoredValue=False

_flags.append(abortOnConfigurationError)


# =================
#
# trigger menu flags - menu version, prescale sets
#
# =================
class menuVersion(JobProperty):
    """ Defines the menu version to use, usually the same as the release number. This is part of the XML file name. """
    statusOn=True
    allowedType=['str']
    
    from AthenaCommon.AppMgr import release_metadata
    StoredValue = release_metadata()['release']  # returns '?' if missing
    
_flags.append(menuVersion)


class triggerMenuSetup(JobProperty):
    """ Defines the luminosity dependent setup of trigger lumi01 == 10^33, switches on/off signatures """
    statusOn=True
    allowedType=['str']
    allowedValues = [
        'default', 'cosmic_default', 'InitialBeam_default',
        # menus for 10^31 with EMScale L1 calib
        'Physics_lumi1E31_simpleL1Calib','Physics_lumi1E31_simpleL1Calib_no_prescale',
        'MC_lumi1E31_simpleL1Calib','MC_lumi1E31_simpleL1Calib_no_prescale',
        'MC_lumi1E31_simpleL1Calib_physics_prescale',
        # menus for 10^32 with EMScale L1 calib
        'Physics_lumi1E32_simpleL1Calib','Physics_lumi1E32_simpleL1Calib_no_prescale',
        'MC_lumi1E32_simpleL1Calib','MC_lumi1E32_simpleL1Calib_no_prescale',
        'MC_lumi1E32_simpleL1Calib_physics_prescale',
        # menus for 10^33
        'Physics_lumi1E33','Physics_lumi1E33_no_prescale',
        'MC_lumi1E33','MC_lumi1E33_no_prescale',
        'Physics_lumi1E34','Physics_lumi1E34_no_prescale',
        'MC_lumi1E34','MC_lumi1E34_no_prescale',
        #
        'Cosmics','Cosmic_v1', 'Cosmic2009_v1', 'Cosmic2009_v2', 
        'InitialBeam_v1', 'MC_InitialBeam_v1', 'MC_InitialBeam_v1_no_prescale',
        'Cosmic2009_simpleL1Calib', 'Cosmic2009_inclMuons',
        'enhBias',
        # for 2010 running
        'Cosmic_v2','Cosmic_v3',
        'InitialBeam_v2', 'MC_InitialBeam_v2', 'MC_InitialBeam_v2_no_prescale',
        'InitialBeam_v3', 'MC_InitialBeam_v3', 'MC_InitialBeam_v3_no_prescale',
        #for 2010-2011 running
        'Physics_pp_v1', 'Physics_pp_v1_no_prescale', 'Physics_pp_v1_cosmics_prescale',
        'MC_pp_v1', 'MC_pp_v1_no_prescale',
        'MC_pp_v1_tight_mc_prescale', 'MC_pp_v1_loose_mc_prescale',
        #v2 
        'Physics_pp_v2', 'Physics_pp_v2_no_prescale', 'Physics_pp_v2_cosmics_prescale', 
        'MC_pp_v2', 'MC_pp_v2_primary', 'MC_pp_v2_no_prescale', 
        'MC_pp_v2_tight_mc_prescale', 'MC_pp_v2_loose_mc_prescale',
        #v3
        'Physics_pp_v3', 'Physics_pp_v3_no_prescale', 'Physics_pp_v3_cosmics_prescale', 
        'MC_pp_v3', 'MC_pp_v3_primary', 'MC_pp_v3_no_prescale', 
        'MC_pp_v3_tight_mc_prescale', 'MC_pp_v3_loose_mc_prescale',
        #v4
        'Physics_pp_v4', 'Physics_pp_v4_no_prescale', 'Physics_pp_v4_cosmics_prescale',
        'MC_pp_v4', 'MC_pp_v4_primary', 'MC_pp_v4_no_prescale',
        'MC_pp_v4_upgrade_mc_prescale','MC_pp_v4_tight_mc_prescale', 'MC_pp_v4_loose_mc_prescale',
        # L1 v2 for testing
        'L1_pp_v2',
        'L1_pp_v3',
        'L1_pp_v4',
        'L1_pp_test',
        'L1_alfa_v1',
        'L1_alfa_v2',
        # for HeavyIon
        'InitialBeam_HI_v1', 'InitialBeam_HI_v1_no_prescale',
        'MC_InitialBeam_HI_v1', 'MC_InitialBeam_HI_v1_no_prescale',
        'Physics_HI_v1', 'Physics_HI_v1_no_prescale',
        'MC_HI_v1',     'MC_HI_v1_no_prescale', 'MC_HI_v1_pPb_mc_prescale',
        #
        'Physics_HI_v2', 'Physics_HI_v2_no_prescale', 
        'MC_HI_v2',  'MC_HI_v2_no_prescale', 'MC_HI_v2_pPb_mc_prescale',
        #
        'Physics_default', 'MC_loose_default', 'MC_tight_default',
        # -----------------------------------------------------------------
        # Run 2
        'MC_pp_v5', 'MC_pp_v5_no_prescale', 'MC_pp_v5_tight_mc_prescale', 'MC_pp_v5_loose_mc_prescale','MC_pp_v5_special_mc_prescale', # for development and simulation
        'Physics_pp_v5', # for testing algorithms and software quality during LS1, later for data taking
        'LS1_v1', # for P1 detector commissioning (cosmics, streamers)
        'DC14', 'DC14_no_prescale', 'DC14_tight_mc_prescale', 'DC14_loose_mc_prescale', # for DC14
        'Physics_HI_v3', 'Physics_HI_v3_no_prescale', # for 2015 lead-lead menu 
        ]

    _default_menu='MC_pp_v5_tight_mc_prescale'
    _default_cosmic_menu='Physics_pp_v4_cosmics_prescale'
    _default_InitialBeam_menu='MC_InitialBeam_v3_no_prescale'
    
    StoredValue = _default_menu

    def _do_action(self):
        """ setup some consistency """

        # meaning full default menu
        if self.get_Value() == 'default':
            self.set_Value(self._default_menu)
            self._log.info("%s - trigger menu 'default' changed to '%s'" % (self.__class__.__name__, self.get_Value()))
        elif self.get_Value() == 'cosmic_default':
            self.set_Value(self._default_cosmic_menu)
            self._log.info("%s - trigger menu 'cosmic_default' changed to '%s'" % (self.__class__.__name__, self.get_Value()))
        elif self.get_Value() == 'InitialBeam_default':
            self.set_Value(self._default_InitialBeam_menu)
            self._log.info("%s - trigger menu 'InitialBeam_default' changed to '%s'" % (self.__class__.__name__, self.get_Value()))
            
        # filenames for LVL1 and HLT
        if TriggerFlags.readLVL1configFromXML() is True:
            TriggerFlags.inputLVL1configFile = "LVL1config_"+self.get_Value()+"_" + TriggerFlags.menuVersion() + ".xml"
        if TriggerFlags.readHLTconfigFromXML() is True and (TriggerFlags.inputHLTconfigFile=="" or TriggerFlags.inputHLTconfigFile==None):
            TriggerFlags.inputHLTconfigFile = "HLTconfig_"+self.get_Value()+"_" + TriggerFlags.menuVersion() + ".xml"

_flags.append(triggerMenuSetup)

class L1PrescaleSet(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues = [
        '', 'None',
        # Physics menus
        'L1Prescales100_Physics_lumi1E31_simpleL1Calib','L1PrescalesNone_Physics_lumi1E31_simpleL1Calib',
        'L1Prescales100_Physics_lumi1E32_simpleL1Calib','L1PrescalesNone_Physics_lumi1E32_simpleL1Calib',
        'L1Prescales100_Physics_lumi1E33','L1PrescalesNone_Physics_lumi1E33',
        'L1Prescales100_Physics_lumi1E34','L1PrescalesNone_Physics_lumi1E34',
        'L1Prescales100_MC_lumi1E31_simpleL1Calib','L1PrescalesNone_MC_lumi1E31_simpleL1Calib',
        'L1Prescales100_MC_lumi1E32_simpleL1Calib','L1PrescalesNone_MC_lumi1E32_simpleL1Calib',
        'L1Prescales100_MC_lumi1E33','L1PrescalesNone_MC_lumi1E33',
        # Enhanced bias
        'L1Prescales100_enhBias','L1PrescalesNone_enhBias',
        # Cosmic menus
        'L1Prescales100_Cosmic_v1', 'L1PrescalesNone_Cosmic_v1',
        'L1Prescales100_Cosmic2009_v1', 'L1PrescalesNone_Cosmic2009_v1',
        'L1Prescales100_Cosmic2009_v2', 'L1PrescalesNone_Cosmic2009_v2',
        'L1Prescales100_Cosmic_v2', 'L1PrescalesNone_Cosmic_v2',
        'L1Prescales100_Cosmic_v3', 'L1PrescalesNone_Cosmic_v3',
        'L1Prescales100_Cosmic2009_simpleL1Calib', 'L1PrescalesNone_Cosmic2009_simpleL1Calib',
        'L1Prescales100_Cosmic2009_inclMuons', 'L1PrescalesNone_Cosmic2009_inclMuons',
        # Commissioning menus
        'L1Prescales100_InitialBeam_v1', 'L1PrescalesNone_InitialBeam_v1',
        'L1Prescales100_MC_InitialBeam_v1', 'L1PrescalesNone_MC_InitialBeam_v1',
        'L1Prescales100_InitialBeam_v2', 'L1PrescalesNone_InitialBeam_v2',
        'L1Prescales100_MC_InitialBeam_v2', 'L1PrescalesNone_MC_InitialBeam_v2',
        'L1Prescales100_InitialBeam_v3', 'L1PrescalesNone_InitialBeam_v3',
        'L1Prescales100_MC_InitialBeam_v3', 'L1PrescalesNone_MC_InitialBeam_v3',
        #2010-2011 menus
        'L1Prescales100_Physics_pp_v1','L1PrescalesNone_Physics_pp_v1',
        'L1Prescales100_MC_pp_v1','L1PrescalesNone_MC_pp_v1',
        'L1Prescales100_MC_pp_v1_tight_mc_prescale','L1PrescalesNone_MC_pp_v1_tight_mc_prescale',
        'L1Prescales100_MC_pp_v1_loose_mc_prescale','L1PrescalesNone_MC_pp_v1_loose_mc_prescale',
        # HeavyIon
        'L1Prescales100_InitialBeam_HI_v1','L1PrescalesNone_InitialBeam_HI_v1',
        'L1Prescales100_MC_InitialBeam_HI_v1', 'L1PrescalesNone_MC_InitialBeam_HI_v1',
        'L1Prescales100_Physics_HI_v1','L1PrescalesNone_Physics_HI_v1',
        'L1Prescales100_MC_HI_v1','L1PrescalesNone_MC_HI_v1',
        'L1Prescales100_Physics_HI_v2','L1PrescalesNone_Physics_HI_v2',
        'L1Prescales100_MC_HI_v2','L1PrescalesNone_MC_HI_v2',
        ]
    StoredValue = ''
_flags.append(L1PrescaleSet)

class HLTPrescaleSet(JobProperty):
    statusOn = True
    allowedTypes = ['str']
    allowedValues = [
        '', 'None',
        # Physics menus
        'HLTPrescales100_Physics_lumi1E31_simpleL1Calib','HLTPrescalesNone_Physics_lumi1E31_simpleL1Calib',
        'HLTPrescales100_Physics_lumi1E32_simpleL1Calib','HLTPrescalesNone_Physics_lumi1E32_simpleL1Calib',
        'HLTPrescales100_Physics_lumi1E33','HLTPrescalesNone_Physics_lumi1E33',
        'HLTPrescales100_Physics_lumi1E34','HLTPrescalesNone_Physics_lumi1E34',        
        'HLTPrescales100_MC_lumi1E31_simpleL1Calib','HLTPrescalesNone_MC_lumi1E31_simpleL1Calib',
        'HLTPrescales100_MC_lumi1E32_simpleL1Calib','HLTPrescalesNone_MC_lumi1E32_simpleL1Calib',
        'HLTPrescales100_MC_lumi1E33','HLTPrescalesNone_MC_lumi1E33',
        # Enhanced bias
        'HLTPrescales100_enhBias','HLTPrescalesNone_enhBias',
        # Cosmic menus
        'HLTPrescales100_Cosmic_v1', 'HLTPrescalesNone_Cosmic_v1',
        'HLTPrescales100_Cosmic2009_v1', 'HLTPrescalesNone_Cosmic2009_v1',
        'HLTPrescales100_Cosmic2009_v2', 'HLTPrescalesNone_Cosmic2009_v2',
        'HLTPrescales100_Cosmic2009_simpleL1Calib', 'HLTPrescalesNone_Cosmic2009_simpleL1Calib',
        'HLTPrescales100_Cosmic2009_inclMuons', 'HLTPrescalesNone_Cosmic2009_inclMuons',
        'HLTPrescales100_Cosmic_v2', 'HLTPrescalesNone_Cosmic_v2',        
        'HLTPrescales100_Cosmic_v3', 'HLTPrescalesNone_Cosmic_v3',        
        # Commissioning menus
        'HLTPrescales100_InitialBeam_v1', 'HLTPrescalesNone_InitialBeam_v1',
        'HLTPrescales100_MC_InitialBeam_v1', 'HLTPrescalesNone_MC_InitialBeam_v1',
        'HLTPrescales100_InitialBeam_v2', 'HLTPrescalesNone_InitialBeam_v2',
        'HLTPrescales100_MC_InitialBeam_v2', 'HLTPrescalesNone_MC_InitialBeam_v2',
        'HLTPrescales100_InitialBeam_v3', 'HLTPrescalesNone_InitialBeam_v3',
        'HLTPrescales100_MC_InitialBeam_v3', 'HLTPrescalesNone_MC_InitialBeam_v3',
        # 2010-2011 menus
        #2010-2011 menus
        'HLTPrescales100_Physics_pp_v1','HLTPrescalesNone_Physics_pp_v1',
        'HLTPrescales100_MC_pp_v1','HLTPrescalesNone_MC_pp_v1',
        'HLTPrescales100_MC_pp_v1_tight_mc_prescale','HLTPrescalesNone_MC_pp_v1_tight_mc_prescale',
        'HLTPrescales100_MC_pp_v1_loose_mc_prescale','HLTPrescalesNone_MC_pp_v1_loose_mc_prescale',
        # HeavyIon
        'HLTPrescales100_InitialBeam_HI_v1','HLTPrescalesNone_InitialBeam_HI_v1',
        'HLTPrescales100_MC_InitialBeam_HI_v1', 'HLTPrescalesNone_MC_InitialBeam_HI_v1',
        'HLTPrescales100_Physics_HI_v1','HLTPrescalesNone_Physics_HI_v1',
        'HLTPrescales100_MC_HI_v1','HLTPrescalesNone_MC_HI_v1',
        'HLTPrescales100_Physics_HI_v2','HLTPrescalesNone_Physics_HI_v2',
        'HLTPrescales100_MC_HI_v2','HLTPrescalesNone_MC_HI_v2',
        ]
    StoredValue = ''
_flags.append(HLTPrescaleSet)



# the container of all trigger flags

class Trigger(JobPropertyContainer):
    """ Trigger top flags """
      
    def Slices_LVL2_setOn(self):
        """ Runs setL2 flags in all slices. Effectivelly enable LVL2. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.setL2()

    def Slices_EF_setOn(self):
        """ Runs setEF flags in all slices. Effectivelly enable EF. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.setEF()

    def Slices_all_setOn(self):
        """ Runs setL2 and setEF in all slices. Effectivelly enable trigger. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.setAll()

    def Slices_LVL2_setOff(self):
        """ Runs unsetL2 flags in all slices.  Effectivelly disable LVL2. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.unsetL2()


    def Slices_EF_setOff(self):
        """ Runs unsetEF flags in all slices.  Effectivelly disable EF. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.unsetEF()

    def Slices_all_setOff(self):
        """ Runs unsetAll in all slices. Effectivelly disable trigger. """
        for prop in self.__dict__.values():
            if issubclass( prop.__class__, JobPropertyContainer ) and "signatures" in prop.__dict__.keys():
                prop.unsetAll()

#    def synchronizeFlgasToRecoSetup(self):
#        """ When running in the recnstruction framework TriggerFlags must be synchronized with the Reco/Common/Global Flags  """
#        print 'globals ', globals()
#        print 'dir ',dir()
#        if 'doWriteBS' in globals().keys():
#            if doWriteBS:
#                self.writeBS = True

        # more to come



## attach yourself to the RECO flags
## from RecExConfig.RecFlags import jobproperties
from RecExConfig.RecFlags import rec
rec.add_Container(Trigger)


for flag in _flags:
    rec.Trigger.add_JobProperty(flag)
del _flags

## make an alias for trigger flags which looks like old TriggerFlags class
TriggerFlags = rec.Trigger




## add online specific flags
from TriggerJobOpts.TriggerOnlineFlags      import OnlineFlags

## add slices generation flags

if useNewTM:

    try:
        from TriggerMenu.menu.SliceFlags import *
    except ImportError:
        import TriggerMenu.egamma.EgammaSliceFlags
        import TriggerMenu.jet.JetSliceFlags
        import TriggerMenu.bjet.BjetSliceFlags
        import TriggerMenu.muon.MuonSliceFlags
        import TriggerMenu.met.METSliceFlags
        import TriggerMenu.tau.TauSliceFlags
        import TriggerMenu.bphysics.BphysicsSliceFlags
        import TriggerMenu.minbias.MinBiasSliceFlags
        import TriggerMenu.combined.CombinedSliceFlags
        import TriggerMenu.calibcosmicmon.CosmicSliceFlags
        import TriggerMenu.calibcosmicmon.CalibSliceFlags
        import TriggerMenu.calibcosmicmon.StreamingSliceFlags
        import TriggerMenu.calibcosmicmon.MonitorSliceFlags
        import TriggerMenu.calibcosmicmon.EnhancedBiasSliceFlags

else:
    from TriggerMenuPython.Lvl1Flags            import Lvl1Flags
    from TriggerMenuPython.EgammaSliceFlags     import EgammaSliceFlags
    from TriggerMenuPython.TauSliceFlags        import TauSliceFlags
    from TriggerMenuPython.JetSliceFlags        import JetSliceFlags
    from TriggerMenuPython.MuonSliceFlags       import MuonSliceFlags
    from TriggerMenuPython.METSliceFlags        import METSliceFlags
    from TriggerMenuPython.BphysicsSliceFlags   import BphysicsSliceFlags
    from TriggerMenuPython.BjetSliceFlags       import BjetSliceFlags
    from TriggerMenuPython.CombinedSliceFlags   import CombinedSliceFlags
    from TriggerMenuPython.MinBiasSliceFlags    import MinBiasSliceFlags
    from TriggerMenuPython.CosmicSliceFlags     import CosmicSliceFlags
    from TriggerMenuPython.HeavyIonSliceFlags   import HeavyIonSliceFlags
    from TriggerMenuPython.CalibSliceFlags      import CalibSliceFlags
    from TriggerMenuPython.L1CaloSliceFlags     import L1CaloSliceFlags
    from TriggerMenuPython.BeamSpotSliceFlags   import BeamSpotSliceFlags
    from TriggerMenuPython.GenericSliceFlags    import GenericSliceFlags
    from TriggerMenuPython.MonitorSliceFlags    import MonitorSliceFlags
    from TriggerMenuPython.StreamingSliceFlags  import StreamingSliceFlags




from TriggerJobOpts.Tier0TriggerFlags       import Tier0TriggerFlags
from TrigTier0.NtupleProdFlags              import NtupleProductionFlags


def sync_Trigger2Reco():
    from AthenaCommon.Include import include
    from RecExConfig.RecAlgsFlags import recAlgs
    from AthenaCommon.GlobalFlags  import globalflags
    from RecExConfig.RecFlags import rec
    
    if  recAlgs.doTrigger() and rec.readRDO() and not globalflags.InputFormat()=='bytestream':
        include( "TriggerRelease/TransientBS_DetFlags.py" )

    from RecExConfig.RecFlags import rec
    if globalflags.InputFormat() == 'bytestream':
        TriggerFlags.readBS = True
        TriggerFlags.doLVL1 = False
        TriggerFlags.doLVL2 = False
        TriggerFlags.doEF   = False

    if rec.doWriteBS():
        TriggerFlags.writeBS = True
                    
                                        
def sync_Reco2Trigger():
    #from AthenaCommon.Include import include
    #include( "TriggerRelease/TransientBS_DetFlags.py" )
    pass


del log
