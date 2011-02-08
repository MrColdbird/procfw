REBOOTEXBIN = Rebootex_bin
REBOOTEX = Rebootex
INSTALLER = Installer
VSHCONTROL = Vshctrl
SYSTEMCONTROL = SystemControl
SYSTEMCONTROLPXE = PXE/SystemControlPXE
REBOOTEXPXE = PXE/RebootexPXE
LAUNCHER = Launcher
GALAXYDRIVER = ISODrivers/Galaxy
M33DRIVER = ISODrivers/March33
STARGATE = Stargate
ISOLAUNCHER = testsuite/ISOLauncher
FASTRECOVERY = FastRecovery
SATELITE = Satelite
DISTRIBUTE = dist
OPT_FLAGS=-j4

#DEBUG=1

ifeq ($(DEBUG), 1)
DEBUG_OPTION="DEBUG=1"
endif

all:
# Preparing Distribution Folders
	@mkdir $(DISTRIBUTE) || true
	@mkdir $(DISTRIBUTE)/635PROUPDATE || true
	@mkdir $(DISTRIBUTE)/ISOLauncher || true
	@mkdir $(DISTRIBUTE)/635FastRecovery || true
	@rm -f ./Common/*.o

# Creating Live-System Reboot Buffer
	@cd $(REBOOTEXBIN); make
	@cd $(REBOOTEX); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(REBOOTEX)/Rebootex.prx $(DISTRIBUTE)/635PROUPDATE

# Creating PXE Executable
	@cd $(INSTALLER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROLPXE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	contrib/pspgz.py $(REBOOTEXPXE)/systemctrl.prx contrib/SystemControl.hdr $(SYSTEMCONTROLPXE)/systemctrl.prx
	@cd $(REBOOTEXPXE); make
	@mv $(REBOOTEXPXE)/rebootex.h $(LAUNCHER)
	@cd $(LAUNCHER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(LAUNCHER)/EBOOT.PBP $(DISTRIBUTE)/635PROUPDATE
	@mv $(INSTALLER)/installer.prx $(DISTRIBUTE)/635PROUPDATE

# Creating Live-System Components
	@cd $(VSHCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@rm -f ./Common/*.o
	@cd $(STARGATE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SATELITE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(SATELITE)/satelite.prx $(DISTRIBUTE)/635PROUPDATE
	@mv $(GALAXYDRIVER)/galaxy.prx $(DISTRIBUTE)/635PROUPDATE
	@cp $(M33DRIVER)/march33.prx $(DISTRIBUTE)/635PROUPDATE
	@cp contrib/usbdevice.prx $(DISTRIBUTE)/635PROUPDATE

# Creating Debugging Suite for Live-System
	@cd $(ISOLAUNCHER); make $(OPT FLAGS) $(DEBUG_OPTION)
	@cd $(FASTRECOVERY); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(ISOLAUNCHER)/UI/EBOOT.PBP $(DISTRIBUTE)/ISOLauncher
	@mv $(ISOLAUNCHER)/Launcher/launcher.prx $(DISTRIBUTE)/ISOLauncher
	@mv $(FASTRECOVERY)/EBOOT.PBP $(DISTRIBUTE)/635FastRecovery

clean:
	@cd $(REBOOTEXBIN); make clean $(DEBUG_OPTION)
	@cd $(REBOOTEX); make clean $(DEBUG_OPTION)
	@cd $(INSTALLER); make clean $(DEBUG_OPTION)
	@cd $(VSHCONTROL); make clean $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make clean $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make clean $(DEBUG_OPTION)
	@cd $(STARGATE); make clean $(DEBUG_OPTION)
	@cd $(ISOLAUNCHER); make clean $(DEBUG_OPTION)
	@cd $(FASTRECOVERY); make clean $(DEBUG_OPTION)
	@cd $(SATELITE); make clean $(DEBUG_OPTION)
	@cd $(LAUNCHER); make clean $(DEBUG_OPTION)
	@cd $(REBOOTEXPXE); make clean $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROLPXE); make clean $(DEBUG_OPTION)
	@rm -rf $(DISTRIBUTE)

build_lib:
	@cd $(SYSTEMCONTROL)/libs; make $(OPT_FLAGS) $(DEBUG_OPTION)
	
clean_lib:
	@cd $(SYSTEMCONTROL)/libs; make clean $(DEBUG_OPTION)
