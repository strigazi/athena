"""Instantiates AlgTools from parameters stored in a node instance"""

# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
from TrigHLTJetHypo.TrigHLTJetHypoConf import (TrigJetHypoToolConfig_simple,
                                               TrigJetHypoToolConfig_dijet,
                                               NotHelperTool,
                                               AndHelperTool,
                                               OrHelperTool,
                                               TrigJetHypoToolHelperMT,
                                               CombinationsHelperTool,)

class ToolSetter(object):
    """Visitor to set instantiated AlgTools to a jet hypo tree"""
    
    def __init__(self, name):

        self.tool_factories = {
            'simple': [TrigJetHypoToolConfig_simple, 0],
            'not': [NotHelperTool, 0],
            'and': [AndHelperTool, 0],
            'or': [OrHelperTool, 0],
            'dijet': [TrigJetHypoToolConfig_dijet, 0],
            'combgen': [CombinationsHelperTool, 0],
            }

        self.mod_router = {
            'not': self.mod_logical_unary,
            'and': self.mod_logical_binary,
            'or': self.mod_logical_binary,
            'simple': self.mod_simple,
            'combgen': self.mod_combgen,
            'dijet': self.mod_dijet,
        }

    def mod_logical_binary(self, node):
        """Set the HypoConfigTool instance the 'and' and 'or' scenarios
        these take two predicates"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        tool = klass(name=name)

        # print 'ToolSetter, setting lhs ', node.children[0].tool
        tool.lhs = node.children[0].tool
        tool.rhs = node.children[1].tool

        tool.node_id = node.node_id
        tool.parent_id = node.parent_id

        node.tool = tool


    def mod_logical_unary(self, node):
        """Set the HypoConfigTool instance for the 'not' scenario
        this takes a single predicate"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        tool = klass(name=name)

        # print 'ToolSetter, setting lhs ', node.children[0].tool
        tool.hypoTool = node.children[0].tool

        tool.node_id = node.node_id
        tool.parent_id = node.parent_id

        node.tool = tool


    def mod_combgen(self, node):
        """Set the HypoConfigTool instance for the 'not' scenario
        this takes a single predicate"""
        
        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        self.tool_factories[scen][1] += 1

        tool = klass(name=name)

        tool.groupSize = node.conf_attrs['groupSize']
        tool.children = [child.tool for child in node.children]

        tool.node_id = node.node_id
        tool.parent_id = node.parent_id

        node.tool = tool


    def mod_simple(self, node):
        """Set the HypoConfigTool instance in a hypo tree node"""

        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        
        self.tool_factories[scen][1] += 1

        config_tool = klass(name=name+'_config')
        [setattr(config_tool, k, v) for k, v in node.conf_attrs.items()]
        
        helper_tool = TrigJetHypoToolHelperMT(name=name+'_helper')
        helper_tool.HypoConfigurer = config_tool
        helper_tool.node_id = node.node_id
        helper_tool.parent_id = node.parent_id

        node.tool = helper_tool
 
    def mod_dijet(self, node):
        """Set the HypoConfigTool instance in a hypo tree node"""

        scen = node.scenario
        klass = self.tool_factories[scen][0]
        sn = self.tool_factories[scen][1]
        name = '%s_%d' % (scen, sn)
        
        self.tool_factories[scen][1] += 1

        config_tool = klass(name=name+'_config')
        [setattr(config_tool, k, v) for k, v in node.conf_attrs.items()]
        helper_tool = TrigJetHypoToolHelperMT(name=name+'_helper')
        helper_tool.HypoConfigurer = config_tool

        helper_tool.node_id = node.node_id
        helper_tool.parent_id = node.parent_id

        node.tool = helper_tool                               

    def mod(self, node):
        """Set the HypoConfigTool instance according to the scenario.
        Note: node.accept must perform depth first navigation to ensure
        child tools are set."""
        
        self.mod_router[node.scenario](node)

    def report(self):
        wid = max(len(k) for k in self.tool_factories.keys())
        rep = '\n%s: ' % self.__class__.__name__

        rep += '\n'.join(
            ['%s: %d' % (k.ljust(wid), v[1])
             for k, v in self.tool_factories.items()])

        return rep
