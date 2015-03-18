# Introduction #

This tutorial will guide you on how to setup a working development environment for compiling the PRO CFW sourcecode on Ubuntu Linux.

It requires you to have basic knowledge on how to handle a bash terminal.

# Setting up the PSPSDK #

Due to problems with the PSPSDK subversion repository, this tutorial will explain an alternative and easier route on how to setup a minimalistic PSPSDK for compiling the PRO CFW.

  1. Grab the Ubuntu .deb Package for your CPU-architecture from the [MinPSPW Sourceforge Page](http://sourceforge.net/projects/minpspw/).
  1. Open a bash terminal and issue the following commands.
    * `cd Downloads`
    * `sudo dpkg -i minpspw.deb`
    * `cd`
    * `gedit .profile`
      * Add the following lines to the bottom of the file, then save and close it.
      * export PATH=$PATH:/opt/pspsdk/bin
      * export PSPSDK=/opt/pspsdk
    * `sudo apt-get install zlib1g-dev`
    * `sudo reboot`
      * This will reboot your system, just log back into your user after it's done.
  1. Grab the latest PRO CFW sourcecode from the Mercurial Repository.
  1. Open a bash terminal and cd into the PRO CFW sourcecode folder, then issue the following commands.
    * `cd contrib/PrxEncrypter/`
    * `make`
    * `chmod +x PrxEncrypter`
    * `sudo mv PrxEncrypter /opt/pspsdk/bin/`
    * `gedit build.mak`
      * Search for...
        * /usr/local/pspdev/bin/PrxEncrypter
      * ... and replace it with...
        * PrxEncrypter
      * ... then save the file and close it.
    * `sudo cp build.mak /opt/pspsdk/psp/sdk/lib/`

Congratulations. You now have a KIRK signature compatible PSPSDK set up and are ready to compile PRO CFW.

# Compiling PRO CFW #

  1. Grab the latest PRO CFW sourcecode from the Mercurial Repository.
  1. Open a bash terminal and cd into the PRO CFW sourcecode folder, then issue the following commands.
    * `make clean`
    * `make clean_lib`
    * `make build_lib`
    * **Depending on the PRO version you wish to build this final command can be...**
      * `make CONFIG_620=1`
      * `make CONFIG_635=1`
      * `make CONFIG_639=1`

Congratulations. After waiting for the compilation process to finish, you should now have a newly created "dist" folder in your PRO CFW sourcecode directory, which contains ready to distribute PRO CFW installation executables.

**Note: If you are building the most recent commits then you should pass NIGHTLY=1 to make also well. It's good for bug-tracking.**