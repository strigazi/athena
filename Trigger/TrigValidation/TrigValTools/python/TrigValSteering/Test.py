#
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#

'''
Definition of a Trigger ART test to be configured and executed
'''

import sys
import os
import json

from TrigValTools.TrigValSteering.Common import get_logger, art_result, clear_art_summary, package_prefix_dict
from TrigValTools.TrigValSteering.Step import get_step_from_list


class Test(object):
    '''Definition of a Trigger ART test to be configured and executed'''
    def __init__(self):
        self.log = get_logger()
        self.name = None
        self.package_name = None
        self.art_type = None
        self.exec_steps = []
        self.check_steps = []
        self.dry_run = None
        self.configured = False

    def configure(self):
        # Configure name
        self.configure_name()
        self.log.info("Configuring test %s", self.name)

        # Configure steps
        for step in self.exec_steps:
            step.configure(self)
        for step in self.check_steps:
            step.configure(self)

        duplicate_names = self.duplicate_step_names()
        if len(duplicate_names) > 0:
            self.configuration_error(
                'Found test steps with duplicate names: '
                '{}'.format(duplicate_names)+' Aborting because this'
                ' configuration could lead to overwriting logs')

        # Configure dry_run option
        if self.dry_run is None:
            # A hook to use dry_run for unit tests
            env = os.environ.get('TRIGVALSTEERING_DRY_RUN')
            if env:
                self.dry_run = True

        self.configured = True

        # Print configuration
        self.log.debug(
            'Test configuration complete:\n-- %s',
            '\n-- '.join(['{}: {}'.format(k, v) for k, v in self.__dict__.items()]))

    def run(self):
        if not self.configured:
            self.configure()

        self.log.info("Running test %s", self.name)

        # Clear the result summary log
        clear_art_summary()

        # Store the executed commands
        commands = {
            'exec_steps': [],
            'check_steps': []
        }

        # Run the exec steps
        for step in self.exec_steps:
            code, cmd = step.run(self.dry_run)
            commands['exec_steps'].append(cmd)

        # Make a summary result code for all exec steps if there are multiple
        if len(self.exec_steps) > 1:
            exec_summary = 0
            for step in self.exec_steps:
                if step.result > exec_summary:
                    exec_summary = step.result
            if exec_summary > 0:
                self.log.info('At least one of the exec steps failed, using the largest code as ExecSummary')
            else:
                self.log.info('All exec steps succeeded')
            art_result(exec_summary, 'ExecSummary')

        # Run the check steps
        for step in self.check_steps:
            code, cmd = step.run(self.dry_run)
            commands['check_steps'].append(cmd)

        # Dump all commands to JSON
        with open('commands.json', 'w') as outfile:
            json.dump(commands, outfile, indent=4)

        # Create the exit code from required steps
        exit_code = 0
        for step in self.exec_steps + self.check_steps:
            if step.required and (step.result > exit_code):
                exit_code = step.result
        self.log.info('Test %s finished with code %s', self.name, exit_code)
        return exit_code

    def configure_name(self):
        filename = os.path.basename(sys.argv[0])
        self.log.debug('Parsing file name %s', filename)
        prefix = 'test_'
        suffix = '.py'
        if not filename.startswith(prefix) or not filename.endswith(suffix):
            self.configuration_error(
                'Test file name {} does not match '.format(filename) +
                'the required pattern {}*{}'.format(prefix, suffix))
        for package_name, package_prefix in package_prefix_dict.items():
            if filename.startswith(prefix+package_prefix):
                self.package_name = package_name
        if self.package_name is None:
            self.configuration_error(
                'Test file name {} could not be matched '.format(filename) +
                'to any of the required package prefixes: {}'.format(
                    package_prefix_dict.values()))
        max_len = 50
        if len(filename) > max_len:
            self.configuration_error(
                'Test file name is too long. The limit is {} '.format(max_len) +
                'characters, but it has {}'.format(len(filename)))
        self.name = filename[len(prefix):-len(suffix)]

    def duplicate_step_names(self):
        d = {}
        for step in self.exec_steps:
            d.setdefault(step.name, 0)
            d[step.name] += 1
        for step in self.check_steps:
            d.setdefault(step.name, 0)
            d[step.name] += 1
        duplicates = [name for name, count in d.items() if count > 1]
        self.log.debug('all steps: %s', d)
        self.log.debug('duplicates: %s', duplicates)
        return duplicates

    def configuration_error(self, message):
        self.log.error(message)
        art_result(1, 'TestConfig')
        sys.exit(1)

    def get_step(self, step_name):
        step = get_step_from_list(step_name, self.exec_steps)
        if step is None:
            step = get_step_from_list(step_name, self.check_steps)
        return step
