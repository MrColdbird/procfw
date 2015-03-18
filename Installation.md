# Introduction #

Before installing, you need to make sure your PSP is compatible.

At the time of writing this article, PRO CFW supports 4 different PSP Kernel.

  * 6.20
  * 6.35
  * 6.39
  * 6.60

If your PSP runs a firmware, lower or equal to 6.60, your PSP should be compatible to PRO CFW.

Should your PSP run a firmware, lower than your target PRO Base Kernel Version, grab and install the appropriate Update File from the mirror chart below.

| Version | UMD Models | GO Model |
|:--------|:-----------|:---------|
| 6.20 | [DOWNLOAD](http://e6326a84.linkbucks.com) | [DOWNLOAD](http://f6a9df67.linkbucks.com) |
| 6.35 | [DOWNLOAD](http://25e7f183.linkbucks.com) | [DOWNLOAD](http://d1dcdcb2.linkbucks.com) |
| 6.39 | [DOWNLOAD](http://ce05fbdf.linkbucks.com) | [DOWNLOAD](http://18a9b7cc.linkbucks.com) |
| 6.60 | [DOWNLOAD](http://38f9ef80.linkbucks.com) | [DOWNLOAD](http://758de455.linkbucks.com) |

# Installation #

You read the introduction and now know your PSP is compatible to PRO CFW.
Now it's time to install the [Aftermarket Firmware Modules](AftermarketModules.md) into your system.

Grab the latest binary release from the Google Code Download Page and unpack it.
You will find two (or three, if you got the 6.20 / 6.39 / 6.60 version) folders of interest which you will use to install and operate the Aftermarket Firmware.

  * **[PROUPDATE](ProUpdate.md)** - Application to install [Aftermarket Module](AftermarketModules.md)
  * **FastRecovery** - Application to QuickStart [Aftermarket Modules](AftermarketModules.md)
  * **[620PRO\_Permanent](Pro620Bootloader.md)** - Permanent Bootloader Installer (for 6.20 Kernel only)
  * **CIPLFlasher** - Application to install a CIPL-based permanent Bootloader (for 1g and 2g-first gen units only)

  1. Copy all of them onto your PSP inside the PSP/GAME subfolder and disconnect your PSP from PC.
  1. Start the [PROUPDATE Executable](ProUpdate.md) to install the [Aftermarket Modules](AftermarketModules.md), delete it afterwards, you won't need it again unless you wish to uninstall PRO CFW again.
  1. If you are running the 6.20 Kernel Version of PRO CFW, you can, if you wish to, execute the [620PRO\_Permanent](Pro620Bootloader.md) executable, to install a custom bootloader to automatically load and execute [Aftermarket Modules](AftermarketModules.md) for you when you turn on your PSP, afterwards, delete this executable aswell, unless you wish to uninstall the bootloader again.
  1. The same applies for 6.39 / 6.60 Kernel Version of PRO CFW, but using the CIPLFlasher application.

Note: Holding L Button during installation will reset your configuration to default and rewrite all the Aftermarket Modules.

# Daily Use #

You have installed the PRO CFW [Aftermarket Modules](AftermarketModules.md) on your PSP as described above.
Now it's time to learn how to use it efficiently.

After the installation, you will be [QuickStarted](QuickStart.md) into the CFW environment automatically, however, in the future you will have to boot it manually every time you shutdown and restart your PSP.

Due to the PSPs security design, it's not possible for all PRO CFW versions to QuickBoot automatically everytime you turn on your system.

This is only possible if your PSP is compatible to the 6.20 / 6.39 / 6.60 PRO CFW version and only if you installed the optional [6.20 PRO Bootloader](Pro620Bootloader.md) or CIPL.

All other PRO revisions need to be _[QuickStarted](QuickStart.md)_ manually by executing the FastRecovery application.

Once executed, PRO CFW will remain active until you shutdown your PSP system.

Summed up, if you restart your PSP, it will be back in OFW mode and behave just like every other normal PSP until you _QuickStart_ it back into CFW mode.