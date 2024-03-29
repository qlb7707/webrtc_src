# Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Tests for the command module."""

from __future__ import print_function

import argparse
import glob
import os

from chromite.cli import command
from chromite.lib import commandline
from chromite.lib import cros_build_lib_unittest
from chromite.lib import cros_import
from chromite.lib import cros_logging as logging
from chromite.lib import cros_test_lib
from chromite.lib import partial_mock
from chromite.lib import workspace_lib


# pylint:disable=protected-access

_COMMAND_NAME = 'superAwesomeCommandOfFunness'


@command.CommandDecorator(_COMMAND_NAME)
class TestCommand(command.CliCommand):
  """A fake command."""
  def Run(self):
    print('Just testing')


class TestCommandTest(cros_test_lib.MockTestCase):
  """This test class tests that Commands method."""

  def testParserSetsCommandClass(self):
    """Tests that our parser sets command_class correctly."""
    my_parser = argparse.ArgumentParser()
    command.CliCommand.AddParser(my_parser)
    ns = my_parser.parse_args([])
    self.assertEqual(ns.command_class, command.CliCommand)

  def testCommandDecorator(self):
    """Tests that our decorator correctly adds TestCommand to _commands."""
    # Note this exposes an implementation detail of _commands.
    self.assertEqual(command._commands[_COMMAND_NAME], TestCommand)

  def testBadUseOfCommandDecorator(self):
    """Tests that our decorator correctly rejects bad test commands."""
    try:
      # pylint: disable=W0612
      @command.CommandDecorator('bad')
      class BadTestCommand(object):
        """A command that wasn't implemented correctly."""
        pass

    except command.InvalidCommandError:
      pass
    else:
      self.fail('Invalid command was accepted by the CommandDecorator')

  def testCrosAddDeviceArgument(self):
    """Tests CliCommand.AddDeviceArgument() for `cros`."""
    self.PatchObject(command, 'GetToolset', return_value='cros')
    parser = argparse.ArgumentParser()
    command.CliCommand.AddDeviceArgument(parser)
    # cros should have a positional device argument.
    parser.parse_args(['device'])
    with self.assertRaises(SystemExit):
      parser.parse_args(['--device', 'device'])

  def testBrilloAddDeviceArgument(self):
    """Tests CliCommand.AddDeviceArgument() for `brillo`."""
    self.PatchObject(command, 'GetToolset', return_value='brillo')
    parser = argparse.ArgumentParser()
    command.CliCommand.AddDeviceArgument(parser)
    # brillo should have an optional device argument.
    with self.assertRaises(SystemExit):
      parser.parse_args(['device'])
    parser.parse_args(['--device', 'device'])


class MockCommand(partial_mock.PartialMock):
  """Mock class for a generic CLI command."""
  ATTRS = ('Run',)
  COMMAND = None
  TARGET_CLASS = None

  def __init__(self, args, base_args=None):
    partial_mock.PartialMock.__init__(self)
    self.args = args
    self.rc_mock = cros_build_lib_unittest.RunCommandMock()
    self.rc_mock.SetDefaultCmdResult()
    parser = commandline.ArgumentParser(caching=True)
    subparsers = parser.add_subparsers()
    subparser = subparsers.add_parser(self.COMMAND, caching=True)
    self.TARGET_CLASS.AddParser(subparser)

    args = base_args if base_args else []
    args += [self.COMMAND] + self.args
    options = parser.parse_args(args)
    self.inst = options.command_class(options)

  def Run(self, inst):
    with self.rc_mock:
      return self.backup['Run'](inst)


class CommandTest(cros_test_lib.MockTestCase):
  """This test class tests that we can load modules correctly."""

  # pylint: disable=W0212

  def testFindModules(self):
    """Tests that we can return modules correctly when mocking out glob."""
    fake_command_file = 'cros_command_test.py'
    filtered_file = 'cros_command_unittest.py'
    mydir = 'mydir'

    self.PatchObject(glob, 'glob',
                     return_value=[fake_command_file, filtered_file])

    self.assertEqual(command._FindModules(mydir, 'cros'), [fake_command_file])

  def testLoadCommands(self):
    """Tests import commands correctly."""
    fake_module = 'cros_command_test'
    fake_command_file = '%s.py' % fake_module
    module_path = ('chromite', 'cli', 'cros', fake_module)

    self.PatchObject(command, '_FindModules', return_value=[fake_command_file])
    # The code doesn't use the return value, so stub it out lazy-like.
    load_mock = self.PatchObject(cros_import, 'ImportModule', return_value=None)

    command._ImportCommands('cros')

    load_mock.assert_called_with(module_path)

  def testListCrosCommands(self):
    """Tests we get a sane `cros` list back."""
    cros_commands = command.ListCommands('cros')
    # Pick some commands that are likely to not go away.
    self.assertIn('chrome-sdk', cros_commands)
    self.assertIn('flash', cros_commands)

  def testListBrilloCommands(self):
    """Tests we get a sane `brillo` list back.

    Needs to be separate from testListCrosCommands() because calling
    ListCommands() twice with both 'brillo' and 'cros' produces a superset
    rather than two independent sets.
    """
    brillo_commands = command.ListCommands('brillo')
    # Pick some commands that should be in `cros` but not `brillo`.
    self.assertNotIn('chrome-sdk', brillo_commands)
    self.assertNotIn('stage', brillo_commands)
    # Pick some commands that should be in `brillo`.
    self.assertIn('debug', brillo_commands)
    self.assertIn('devices', brillo_commands)


class FileLoggerSetupTest(cros_test_lib.WorkspaceTestCase):
  """Test that logging to file works correctly."""

  def setUp(self):
    self.CreateWorkspace()

  def testSetupFileLoggerFilename(self):
    """Test that the filename and path are correct."""
    patch_handler = self.PatchObject(logging, 'FileHandler',
                                     return_value=logging.StreamHandler())
    command.SetupFileLogger(filename='foo.log')

    # Test that the filename is correct.
    patch_handler.assert_called_with(
        os.path.join(self.workspace_path, workspace_lib.WORKSPACE_LOGS_DIR,
                     'foo.log'), mode='w')

  def testSetupFileLoggerNoFilename(self):
    """Test that the filename and path are correct with no arguments."""
    patch_handler = self.PatchObject(logging, 'FileHandler',
                                     return_value=logging.StreamHandler())
    command.SetupFileLogger()

    # Test that the filename is correct.
    patch_handler.assert_called_with(
        os.path.join(self.workspace_path, workspace_lib.WORKSPACE_LOGS_DIR,
                     'brillo.log'), mode='w')

  def testSetupFileLoggerLogLevels(self):
    """Test that the logger operates at the right level."""
    command.SetupFileLogger('foo.log', log_level=logging.INFO)
    logging.getLogger().setLevel(logging.DEBUG)
    logging.debug('debug')
    logging.info('info')
    logging.notice('notice')

    # Test that the logs are correct.
    logs = open(
        os.path.join(self.workspace_path, workspace_lib.WORKSPACE_LOGS_DIR,
                     'foo.log'), 'r').read()
    self.assertNotIn('debug', logs)
    self.assertIn('info', logs)
    self.assertIn('notice', logs)
