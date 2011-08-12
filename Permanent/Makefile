all:
	make -C installer/kmod
	make -C installer
	make -C fake_vsh_module
	@cp installer/kmod/kmod.prx .
	@cp installer/EBOOT.PBP .
	@cp fake_vsh_module/hen.prx .

clean:
	make -C installer/kmod clean
	make -C installer clean
	make -C fake_vsh_module clean
	@rm -f kmod.prx hen.prx EBOOT.PBP
