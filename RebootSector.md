# Introduction #

The PSP uses a Runlevel-System similiar to that of Linux Operating Systems to dynamically load and unload modules.

The Reboot Sector is the very first code that gets executed everytime the system switches from one runlevel to another.

# Functionality #

The Reboot Sector of the PSP is always located at the same location in Kernel Memory, it contains security code to prevent the execution of unsigned modules.

In PRO CFW this is circumvented by replacing the Reboot Sector with a custom one to remove parts of the security code.

This is done by utilizing a handful of different Kernel Exploits.