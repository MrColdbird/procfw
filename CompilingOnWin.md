# Introduction #

After being requested by several people here is a compilation guide for PRO CFW on Windows Operating Systems using Cygwin.

# Instructions #

In this tutorial I will use the Cygwin Bash Shell to reproduce a Linux-Like Environment for PSP Development, capable of compiling not only but also PRO CFW.

Follow the listed instructions and commands and you will be guided to a fully working Windows Cygwin Setup for PRO CFW compiling.

  * Grab Cygwin Setup from http://cygwin.com/setup.exe
  * (**Optional**) Find the fastest mirror using this [Cygwin Mirror Tester](https://gist.github.com/devnoname120/10009977#comment-1206438)
  * Install Cygwin to Default Location and make sure to install ALL PACKAGES, takes a few hours to install and takes about 2GB download quota
  * Grab Minimalistic PSPSDK from http://sourceforge.net/projects/minpspw/
  * Install Minimalist PSPSDK to Default Location
  * Open Cygwin Bash Shell from Start Menu -> Cygwin
  * Issue the following commands to install Mercurial
  * wget http://peak.telecommunity.com/dist/ez_setup.py
  * python ez\_setup.py
  * easy\_install -U mercurial
  * Grab a copy of the PRO CFW sourcecode using the following command
  * hg clone https://code.google.com/p/procfw/
  * Issue the following commands to install the PrxEncrypter
  * cd procfw/contrib/PrxEncrypter
  * make
  * mv PrxEncrypter.exe /bin/
  * Edit the build.mak file in the procfw/contrib/PrxEncrypter folder with a editor of your choice (Notepad++, VIM, etc.), change the line starting with "ENC = ..." to "ENC = PrxEncrypter" and save the file.
  * Copy the build.mak file to C:\pspsdk\psp\sdk\lib and overwrite the old build.mak file, this will result in giving your SDK EBOOT.PBP Signature Support.
  * Issue the following commands to add the make utility in version 3.80, the one shipped with Cygwin (3.81) doesn't support windows paths, which prevents you from compiling PRO CFW.
  * wget http://dl.dropbox.com/u/13011238/make.exe
  * mv /bin/make.exe /bin/make-381.exe
  * mv make.exe /bin/

Congratulations. You have setup a fully working Windows Cygwin PSPSDK ready for use with PRO CFW and hopefully a lot of other PSP developments.

To compile PRO CFW, navigate into the procfw folder and issue the required make commands.

For 6.20 PRO it would be...

  * cd ~/procfw
  * make deps
  * make CONFIG\_620=1