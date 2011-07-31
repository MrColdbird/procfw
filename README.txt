This is the source code of 635PRO.
Author: Team PRO

Detail:
SystemControl: kernel of CFW
Rebootex_bin: patch reboot.bin and redirect the loading of bootconfs
Rebootex: load rebootex.bin into kernel and boot the whole CFW
Vshctrl: all VSH related patch goes here. And the ISO VSH dipslay code too.
ISODrivers: march33 and galalxy included for ISO loading
Popcorn: for custom PS1 hack
Installer: install the CFW into the system. After that it can use Rebootex to boot into CFW.
CIPL: permanently install 635PRO on 01g/02g
Satellite: VSH menu
Recovery: Recovery menu as those in 5.XX kernel
testsuite: test tool/homebrews/utils for 635PRO
FastRecovery: Boot the 635PRO faster once installed

include: common used header goes here
libs: common used library goes here
contrib: scripts or else goes here
docs: complex documents goes here
Common: Common code goes here
Imports: The imports of 635 functions go here

Compile:

You need to set $(PRO_HOME) to your source directory
