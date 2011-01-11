REBOOTEXBIN = Rebootex_bin
REBOOTEX = Rebootex
INSTALLER = Installer
VSHCONTROL = Vshctrl
SYSTEMCONTROL = SystemControl
GALAXYDRIVER = ISODrivers/Galaxy
M33DRIVER = ISODrivers/March33
STARGATE = Stargate
DISTRIBUTE = dist
OPT_FLAGS=-j4

ifeq ($(DEBUG), 1)
DEBUG_OPTION="DEBUG=1"
endif

all:
	@cd $(REBOOTEXBIN); make
	@cd $(REBOOTEX); make
	@cd $(INSTALLER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(VSHCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(SYSTEMCONTROL); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(GALAXYDRIVER); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@cd $(STARGATE); make $(OPT_FLAGS) $(DEBUG_OPTION)
	@mv $(INSTALLER)/EBOOT.PBP $(DISTRIBUTE)
	@mv $(REBOOTEX)/Rebootex.prx $(DISTRIBUTE)
	contrib/pspgz.py $(DISTRIBUTE)/systemctrl.prx contrib/SystemControl.hdr $(SYSTEMCONTROL)/systemctrl.prx
	@mv $(VSHCONTROL)/vshctrl.prx $(DISTRIBUTE)
	@mv $(GALAXYDRIVER)/galaxy.prx $(DISTRIBUTE)
	@cp $(M33DRIVER)/march33.prx $(DISTRIBUTE)
	@mv $(STARGATE)/stargate.prx $(DISTRIBUTE)

clean:
	@cd $(REBOOTEXBIN); make clean
	@cd $(REBOOTEX); make clean
	@cd $(INSTALLER); make clean
	@cd $(VSHCONTROL); make clean
	@cd $(SYSTEMCONTROL); make clean
	@cd $(GALAXYDRIVER); make clean
	@cd $(STARGATE); make clean
	rm $(DISTRIBUTE)/*
