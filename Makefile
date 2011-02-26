REBOOTEXBIN = Rebootex_bin
REBOOTEX = Rebootex
INSTALLER = Installer
VSHCONTROL = Vshctrl
SYSTEMCONTROL = SystemControl
SYSTEMCONTROLPXE = PXE/SystemControlPXE
REBOOTEXPXE = PXE/RebootexPXE
LAUNCHER = PXE/Launcher
GALAXYDRIVER = ISODrivers/Galaxy
M33DRIVER = ISODrivers/March33
STARGATE = Stargate
ISOLAUNCHER = testsuite/ISOLauncher
FASTRECOVERY = FastRecovery
SATELITE = Satelite
POPCORN = Popcorn
RECOVERY = Recovery
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
	@mkdir $(DISTRIBUTE)/635FastRecovery || true
	@mkdir $(DISTRIBUTE)/dbg_installer || true
	@rm -f ./Common/*.o

# Creating Live-System Reboot Buffer
	@cd $(REBOOTEXBIN); make
	@cd $(REBOOTEX); make $(OPT_FLAGS) $(DEBUG_OPTION)

# Creating Live-System Components
	@cd $(RECOVERY); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@rm -f ./Common/*.o
	@cd $(VSHCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(STARGATE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SATELITE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(POPCORN); make $(OPT_FLAGS) $(DEBUG_OPTION)

# Creating PXE Executable
	@cd $(INSTALLER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROLPXE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(REBOOTEXPXE); make
	@mv $(REBOOTEXPXE)/rebootex.h $(LAUNCHER)
	@cd $(LAUNCHER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(LAUNCHER)/EBOOT.PBP $(DISTRIBUTE)/635PROUPDATE

# Creating Debugging Suite for Live-System
	@cd $(FASTRECOVERY); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(FASTRECOVERY)/EBOOT.PBP $(DISTRIBUTE)/635FastRecovery
	@cp $(INSTALLER)/EBOOT.PBP $(DISTRIBUTE)/dbg_installer

clean:
	@cd $(REBOOTEXBIN); make clean $(DEBUG_OPTION)
	@cd $(REBOOTEX); make clean $(DEBUG_OPTION)
	@cd $(INSTALLER); make clean $(DEBUG_OPTION)
	@cd $(VSHCONTROL); make clean $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make clean $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make clean $(DEBUG_OPTION)
	@cd $(STARGATE); make clean $(DEBUG_OPTION)
	@cd $(FASTRECOVERY); make clean $(DEBUG_OPTION)
	@cd $(SATELITE); make clean $(DEBUG_OPTION)
	@cd $(LAUNCHER); make clean $(DEBUG_OPTION)
	@cd $(REBOOTEXPXE); make clean $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROLPXE); make clean $(DEBUG_OPTION)
	@cd $(POPCORN); make clean $(DEBUG_OPTION)
	@cd $(RECOVERY); make clean $(DEBUG_OPTION)
	@rm -rf $(DISTRIBUTE)

build_lib:
	@cd $(SYSTEMCONTROL)/libs; make $(OPT_FLAGS) $(DEBUG_OPTION)
	
clean_lib:
	@cd $(SYSTEMCONTROL)/libs; make clean $(DEBUG_OPTION)
