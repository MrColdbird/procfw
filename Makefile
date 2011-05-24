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
INFERNO = ISODrivers/Inferno
STARGATE = Stargate
ISOLAUNCHER = testsuite/ISOLauncher
FASTRECOVERY = FastRecovery
SATELITE = Satelite
POPCORN = Popcorn
RECOVERY = Recovery
PERMANENT = Permanent
DISTRIBUTE = dist
OPT_FLAGS=-j4

ifeq ($(CONFIG_635), 1)
OPT_FLAGS+=CONFIG_635=1
endif

ifeq ($(CONFIG_620), 1)
OPT_FLAGS+=CONFIG_620=1
endif

ifeq ($(CONFIG_639), 1)
OPT_FLAGS+=CONFIG_639=1
endif

ifeq ($(PSID_CHECK), 1)
RELEASE_OPTION=PSID_CHECK=1
endif

#DEBUG=1

ifeq ($(DEBUG), 1)
DEBUG_OPTION=DEBUG=1
endif

all:
# Preparing Distribution Folders
	@mkdir $(DISTRIBUTE) || true
	@mkdir $(DISTRIBUTE)/PROUPDATE || true
	@mkdir $(DISTRIBUTE)/FastRecovery || true
ifeq ($(CONFIG_620), 1)
	@mkdir $(DISTRIBUTE)/620PRO_Permanent || true
endif
	@rm -f ./Common/*.o

# Creating Live-System Reboot Buffer
	@cd $(REBOOTEXBIN); make $(OPT_FLAGS)
	@cd $(REBOOTEX); make $(OPT_FLAGS) $(DEBUG_OPTION)

# Creating Live-System Components
	@cd $(RECOVERY); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@rm -f ./Common/*.o
	@cd $(VSHCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(INFERNO); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(STARGATE); make $(OPT_FLAGS) $(DEBUG_OPTION) $(RELEASE_OPTION)
	@cd $(SATELITE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(POPCORN); make $(OPT_FLAGS) $(DEBUG_OPTION)

# Creating PXE Executable
	@cd $(INSTALLER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROLPXE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(REBOOTEXPXE); make $(OPT_FLAGS)
	@mv $(REBOOTEXPXE)/rebootex.h $(LAUNCHER)
	@cd $(LAUNCHER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(LAUNCHER)/EBOOT.PBP $(DISTRIBUTE)/PROUPDATE

# Creating Debugging Suite for Live-System
	@cd $(FASTRECOVERY); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(FASTRECOVERY)/EBOOT.PBP $(DISTRIBUTE)/FastRecovery
ifeq ($(CONFIG_620), 1)
	@cd $(PERMANENT); make $(DEBUG_OPTION)
	@mv $(PERMANENT)/EBOOT.PBP $(DISTRIBUTE)/620PRO_Permanent
	@mv $(PERMANENT)/hen.prx $(DISTRIBUTE)/620PRO_Permanent
	@mv $(PERMANENT)/kmod.prx $(DISTRIBUTE)/620PRO_Permanent
endif

clean:
	@cd $(REBOOTEXBIN); make clean $(DEBUG_OPTION)
	@cd $(REBOOTEX); make clean $(DEBUG_OPTION)
	@cd $(INSTALLER); make clean $(DEBUG_OPTION)
	@cd $(VSHCONTROL); make clean $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make clean $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make clean $(DEBUG_OPTION)
	@cd $(INFERNO); make clean $(DEBUG_OPTION)
	@cd $(STARGATE); make clean $(DEBUG_OPTION) $(RELEASE_OPTION)
	@cd $(FASTRECOVERY); make clean $(DEBUG_OPTION)
ifeq ($(CONFIG_620), 1)
	@cd $(PERMANENT); make clean $(DEBUG_OPTION)
endif
	@cd $(SATELITE); make clean $(DEBUG_OPTION)
	@cd $(LAUNCHER); make clean $(DEBUG_OPTION)
	@cd $(REBOOTEXPXE); make clean $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROLPXE); make clean $(DEBUG_OPTION)
	@cd $(POPCORN); make clean $(DEBUG_OPTION)
	@cd $(RECOVERY); make clean $(DEBUG_OPTION)
	@rm -rf $(DISTRIBUTE)

deps:
	make clean_lib
	make build_lib

build_lib:
	@cd $(SYSTEMCONTROL)/libs; make $(OPT_FLAGS) $(DEBUG_OPTION)
	
clean_lib:
	@cd $(SYSTEMCONTROL)/libs; make clean $(DEBUG_OPTION)
