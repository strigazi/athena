# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration

from __future__ import print_function

from .Lvl1Thresholds import LVL1Thresholds
from .Lvl1MenuItems import LVL1MenuItems
from .Lvl1MonCounters import Lvl1MonCounters
from .CaloInfo import CaloInfo
from .MuctpiInfo import MuctpiInfo
from .CTPInfo import CTPInfo
from .Limits import Limits
from TriggerJobOpts.TriggerFlags import TriggerFlags

from AthenaCommon.Logging import logging
log = logging.getLogger("TriggerMenu.l1.Lvl1Menu")

class Lvl1Menu:

    def __init__(self, menuName):
        self.menuName = menuName

        # items in menu
        self.items = LVL1MenuItems()
        
        # thresholds in menu
        self.thresholds = LVL1Thresholds()

        # thresholds in menu
        self.counters = Lvl1MonCounters()

        # CTP Info in the menu
        self.CTPInfo = CTPInfo()
        
        # Muon Info in the menu
        self.MuctpiInfo = MuctpiInfo(low_pt=1, high_pt=1, max_cand=13)

        # Calo Info in the menu
        em_scale=2
        if hasattr(TriggerFlags, 'useRun1CaloEnergyScale'):
            if TriggerFlags.useRun1CaloEnergyScale :
                em_scale=1
                
        self.CaloInfo = CaloInfo(name='standard', globalEmScale=em_scale, globalJetScale=1)

        if self.menuName:
            from .Lvl1MenuUtil import get_smk_psk_Name
            smk_psk_Name = get_smk_psk_Name(self.menuName)
            self.items.menuName = smk_psk_Name["smkName"]
            self.items.pssName  = smk_psk_Name["pskName"]


    @staticmethod
    def partitioning():
        from .Lvl1Flags import Lvl1Flags
        first = Lvl1Flags.MenuPartitioning()
        last = first[1:] + [ Limits.MaxTrigItems ]
        partitioning = dict( zip([1,2,3],zip(first,last)) )
        return partitioning

    def addThreshold(self, threshold):
        self.thresholds += threshold


    def addItem(self, item):
        self.items += item


    def getItem(self,name):
        return self.items.findItemByName(name)


    def addCounters(self):
        from TriggerMenu.l1menu.MonitorDef import MonitorDef
        # add the CTPIN counters
        for counter in MonitorDef.ctpinCounters( self.thresholds ):
            self.counters.addCounter( counter )

        # add the CTPMon counters
        for counter in MonitorDef.ctpmonCounters( self.thresholds ):
            self.counters.addCounter( counter )

        # mark the L1 Items that they should be monitored
        MonitorDef.applyItemCounter( self.items )
        




    def xml(self, ind=1, step=2):
        s  = '<?xml version="1.0"?>\n'
        s += '<!DOCTYPE LVL1Config SYSTEM "LVL1config.dtd">\n'
        s += '<LVL1Config name="%s" ctpVersion="%i" l1Version="%i">\n' % (self.menuName, Limits.CTPVersion, Limits.L1CommonVersion)
        s += '  <!--File is generated by TriggerMenu-->\n'
        s += '  <!--No. L1 thresholds defined: %i-->\n' % len(self.thresholds)
        s += '  <!--No. L1 items defined: %i-->\n' % len(self.items)
        s += self.items.xml()
        s += self.counters.xml()
        s += self.thresholds.xml()
        s += self.CTPInfo.xml()
        s += self.CaloInfo.xml()
        s += self.MuctpiInfo.xml()
        s += '</LVL1Config>\n'
        return s


    def readMenuFromXML(self,inputFile):
        from XMLMenuReader import readMenuFromXML
        readMenuFromXML(self, inputFile)

    def printCabling(self):
        cablemap = []
        for thr in self.thresholds:
            if thr.ttype=='TOPO': continue
            cabling = thr.cableinfo
            cablemap += [(cabling.slot, cabling.connector, thr.ttype, cabling.range_begin, cabling.range_end, str(thr))]

        cablemap.sort()

        cable = None
        for c in cablemap:
            k = (c[0],c[1])
            if k != cable:
                cable = k
                print ("\nCable %s, %s" % cable)
                print ("=================")
            print ("%s  bit %i-%i (%s)" % (c[2],c[3],c[4],c[5]))

    def checkL1(self):
        """
        All other checks should be implemented in TrigConfStorage/src/CheckConsistency.cxx
        This method is only for quick solutions but should be intermediate
        """

        success = True
        
        # Checks if all needed thresholds are in the menu
        for item in self.items:
            allThrNames = self.thresholds.thresholdNames()
            itemThrNames = item.thresholdNames(include_bgrp=False)
            for thrName in itemThrNames:
                if thrName not in allThrNames:
                    log.error('Menu item %s contains threshold %s which is not part of the menu!' % (item.name,thrName))
                    success = False

        if not success:
            raise RuntimeError("There is a problem in the menu that needs fixing")

        # Check that the threshold multiplicities don't exceed the number of encodable values give the available bits
        for item in self.items:
            conditions = item.conditions()
            for c in conditions:
                log.debug("Item %s has threshold %s on cable %s with multiplicity %i" % (item, c.threshold, c.threshold.cableinfo, c.multiplicity) )
                maxAllowMult = {1 : 1, 2 : 3, 3 : 7} [c.threshold.cableinfo.bitnum]
                if c.multiplicity > maxAllowMult:
                    log.error("Item %s has condition %s. Threshold %s is on cable %s which allows maximum multiplicity %i" %
                              (item.name, c, c.threshold.name, c.threshold.cableinfo.name, maxAllowMult) )
                    success = False
                    
        if not success:
            raise RuntimeError("There is a problem in the menu that needs fixing")

        # # Check all items are in the correct partition
        # for item in self.items:
        #     partitionRange = Lvl1Menu.partitioning()[item.partition]
        #     if not (item.ctpid>=partitionRange[0] and item.ctpid<partitionRange[1]):
        #         log.error('Item %s (ctpid %i, partition %i) outside allowed range for partition %r!' % (item.name, item.ctpid, item.partition, partitionRange))
        #         success = False

        # if not success:
        #     raise RuntimeError("There is a problem in the menu that needs fixing")
        

        # Check for all items that their bunchgroups are in the same partition
        bgpart = dict( [("BGRP%i" % bg.internalNumber, bg.menuPartition) for bg in self.CTPInfo.bunchGroupSet.bunchGroups] )
        for item in self.items:
            bgs = [t for t in item.thresholdNames(include_bgrp=True) if t.startswith('BGRP')]
            if not 'BGRP0' in bgs:
                log.error('Item %s (partition %i) is not using BGRP0 which is mandatory!' % (item.name,item.partition))
            else:
                bgs.remove('BGRP0')
            for bg in bgs:
                if bgpart[bg] != item.partition:
                    log.error('Item %s (partition %i) uses BG %s which is in partition %i!' % (item.name,item.partition,bg,bgpart[bg]))
                    success = False

        if not success:
            raise RuntimeError("There is a problem in the menu that needs fixing")


        # check that L1 CalReq are the last 3 items of the menu (509,510,511) - check with Thilo/Ralf
        caldef = { "L1_CALREQ0" : 509, "L1_CALREQ1" : 510, "L1_CALREQ2" : 511 }
        for name, ctpid in caldef.items():
            calitem = self.getItem(name)
            if calitem and calitem.ctpid != ctpid:
                log.error('Item %s is not on CTPID %i' % (name,ctpid))
                success = False
        if not success:
            raise RuntimeError("There is a problem in the menu that needs fixing")

        
        # check that the number of monitored items doesn't exceed the limit
        (TBP, TAP, TAV) = (1, 2, 4)
        items_LF = { TBP : set(), TAP : set(), TAV : set() }
        items_HF = { TBP : set(), TAP : set(), TAV : set() }

        for item in self.items:
            for k in (TBP, TAP, TAV):
                if item.monitorsLF & k:
                    items_LF[k].add( item.name )
                if item.monitorsHF & k:
                    items_HF[k].add( item.name )

        counts_LF = dict( map(lambda x : (x[0],len(x[1])), items_LF.items() ) )
        counts_HF = dict( map(lambda x : (x[0],len(x[1])), items_HF.items() ) )

        lutsLF = ( max(counts_LF.values() ) -1 ) // 8 + 1
        lutsHF = ( max(counts_HF.values() ) -1 ) // 8 + 1

        
        if lutsLF + lutsHF <= 8:
            log.info("LVL1 monitoring with %i LF groups (%i items) and %i HF groups (%i items)" % (lutsLF, max(counts_LF.values()), lutsHF, max(counts_HF.values())) )
        else:
            log.error("WARNING: too many monitoring items are defined")
            print ("   low frequency  TBP: %i" % counts_LF[TBP])
            print ("                  TAP: %i" % counts_LF[TAP])
            print ("                  TAV: %i" % counts_LF[TAV])
            print ("   required LUTs: %i" % lutsLF)
            print ("   high frequency TBP: %i" % counts_HF[TBP])
            print ("                  TAP: %i" % counts_HF[TAP])
            print ("                  TAV: %i" % counts_HF[TAV])
            print ("   required LUTs: %i" % lutsHF)
            print ("   this menu requires %i monitoring LUTs while only 8 are available" % (lutsLF + lutsHF))
            print ("   LF TBP:\n     %r" % sorted(items_LF[TBP]))
            print ("   LF TAP:\n     %r" % sorted(items_LF[TAP]))
            print ("   LF TAV:\n     %r" % sorted(items_LF[TAV]))
            print ("   HF TBP:\n     %r" % sorted(items_HF[TBP]))
            print ("   HF TAP:\n     %r" % sorted(items_HF[TAP]))
            print ("   HF TAV:\n     %r" % sorted(items_HF[TAV]))
            success = False
        if not success:
            raise RuntimeError("There is a problem in the menu that needs fixing")




        return success

