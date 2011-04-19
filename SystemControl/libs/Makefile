all:
	make -C libpspsystemctrl_kernel
	@cp libpspsystemctrl_kernel/libpspsystemctrl_kernel.a ../../libs
	make -C libpspsystemctrl_user
	@cp libpspsystemctrl_user/libpspsystemctrl_user.a ../../libs
	make -C libpspkubridge
	@cp libpspkubridge/libpspkubridge.a ../../libs

clean:
	make -C libpspsystemctrl_kernel clean
	make -C libpspsystemctrl_user clean
	make -C libpspkubridge clean
	@rm ../../libs/libpspsystemctrl_kernel.a || true
	@rm ../../libs/libpspsystemctrl_user.a || true
	@rm ../../libs/libpspkubridge.a || true
