REBOOTEXBIN = Rebootex_bin
REBOOTEX = Rebootex
INSTALLER = Installer
VSHCONTROL = Vshctrl
SYSTEMCONTROL = SystemControl
GALAXYDRIVER = ISODrivers/Galaxy
M33DRIVER = ISODrivers/March33
STARGATE = Stargate
ISOLAUNCHER = ISOLauncher
DISTRIBUTE = dist
OPT_FLAGS=-j4

ifeq ($(DEBUG), 1)
DEBUG_OPTION="DEBUG=1"
endif

all:
	@mkdir $(DISTRIBUTE)
	@mkdir $(DISTRIBUTE)/$(INSTALLER)
	@mkdir $(DISTRIBUTE)/$(ISOLAUNCHER)
	@cd $(REBOOTEXBIN); make
	@cd $(REBOOTEX); make
	@cd $(INSTALLER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(VSHCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(STARGATE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(ISOLAUNCHER); make $(OPT FLAGS) $(DEBUG_OPTION)
	@mv $(INSTALLER)/EBOOT.PBP $(DISTRIBUTE)/$(INSTALLER)
	@mv $(REBOOTEX)/Rebootex.prx $(DISTRIBUTE)/$(INSTALLER)
	contrib/pspgz.py $(DISTRIBUTE)/$(INSTALLER)/systemctrl.prx contrib/SystemControl.hdr $(SYSTEMCONTROL)/systemctrl.prx
	@mv $(VSHCONTROL)/vshctrl.prx $(DISTRIBUTE)/$(INSTALLER)
	@mv $(GALAXYDRIVER)/galaxy.prx $(DISTRIBUTE)/$(INSTALLER)
	@cp $(M33DRIVER)/march33.prx $(DISTRIBUTE)/$(INSTALLER)
	@mv $(STARGATE)/stargate.prx $(DISTRIBUTE)/$(INSTALLER)
	@mv $(ISOLAUNCHER)/UI/EBOOT.PBP $(DISTRIBUTE)/$(ISOLAUNCHER)
	@mv $(ISOLAUNCHER)/Launcher/launcher.prx $(DISTRIBUTE)/$(ISOLAUNCHER)

clean:
	@cd $(REBOOTEXBIN); make clean
	@cd $(REBOOTEX); make clean
	@cd $(INSTALLER); make clean
	@cd $(VSHCONTROL); make clean
	@cd $(SYSTEMCONTROL); make clean
	@cd $(GALAXYDRIVER); make clean
	@cd $(STARGATE); make clean
	@cd $(ISOLAUNCHER); make clean
	@rm -r $(DISTRIBUTE)
