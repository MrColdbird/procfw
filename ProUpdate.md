# Introduction #

The **PROUPDATE Executable** is part of the PRO CFW Project.

Its purpose lies in installing the [Aftermarket Modules](AftermarketModules.md) to the flash0 NAND partition for later execution through a QuickStart.

# Functionality #

The PROUPDATE Executable aquires VSH stage NAND writing permission via a PXE environment which temporarly sets the Installer Executable as the main XMB shell.

This PXE environment in turn relies on a Kernel Exploit to setup a minimalistic CFW environment.