REBOOTEXBIN = Rebootex_bin
REBOOTEX = Rebootex
INSTALLER = Installer
VSHCONTROL = Vshctrl
SYSTEMCONTROL = SystemControl
GALAXYDRIVER = ISODrivers/Galaxy
M33DRIVER = ISODrivers/March33
STARGATE = Stargate
ISOLAUNCHER = testsuite/ISOLauncher
FASTRECOVERY = FastRecovery
CONFTOOL = testsuite/ConfTool
SATELITE = Satelite
DISTRIBUTE = dist
OPT_FLAGS=-j4

ifeq ($(DEBUG), 1)
DEBUG_OPTION="DEBUG=1"
endif

all:
	@mkdir $(DISTRIBUTE) || true
	@mkdir $(DISTRIBUTE)/$(INSTALLER) || true
	@mkdir $(DISTRIBUTE)/ISOLauncher || true
	@mkdir $(DISTRIBUTE)/635FastRecovery || true
	@mkdir $(DISTRIBUTE)/635ConfTool || true
	@rm -f ./Common/*.o
	@cd $(REBOOTEXBIN); make
	@cd $(REBOOTEX); make
	@rm -f ./Common/*.o
	@cd $(INSTALLER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(VSHCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(STARGATE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(ISOLAUNCHER); make $(OPT FLAGS) $(DEBUG_OPTION)
	@cd $(CONFTOOL); make $(OPT FLAGS) $(DEBUG_OPTION)
	@cd $(FASTRECOVERY); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SATELITE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(SATELITE)/satelite.prx $(DISTRIBUTE)/$(INSTALLER)
	@mv $(INSTALLER)/EBOOT.PBP $(DISTRIBUTE)/$(INSTALLER)
	@mv $(REBOOTEX)/Rebootex.prx $(DISTRIBUTE)/$(INSTALLER)
	@mv $(VSHCONTROL)/vshctrl.prx $(DISTRIBUTE)/$(INSTALLER)
	@mv $(GALAXYDRIVER)/galaxy.prx $(DISTRIBUTE)/$(INSTALLER)
	@cp $(M33DRIVER)/march33.prx $(DISTRIBUTE)/$(INSTALLER)
	@mv $(STARGATE)/stargate.prx $(DISTRIBUTE)/$(INSTALLER)
	@cp contrib/usbdevice.prx $(DISTRIBUTE)/$(INSTALLER)
	@mv $(ISOLAUNCHER)/UI/EBOOT.PBP $(DISTRIBUTE)/ISOLauncher
	@mv $(ISOLAUNCHER)/Launcher/launcher.prx $(DISTRIBUTE)/ISOLauncher
	@mv $(CONFTOOL)/EBOOT.PBP $(DISTRIBUTE)/635ConfTool
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
	@cd $(CONFTOOL); make clean $(DEBUG_OPTION)
	@cd $(FASTRECOVERY); make clean $(DEBUG_OPTION)
	@cd $(SATELITE); make clean $(DEBUG_OPTION)
	@rm -rf $(DISTRIBUTE)

build_lib:
	@cd $(SYSTEMCONTROL)/libs; make $(OPT_FLAGS) $(DEBUG_OPTION)
	
clean_lib:
	@cd $(SYSTEMCONTROL)/libs; make clean $(DEBUG_OPTION)
