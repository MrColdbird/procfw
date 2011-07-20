	.set noreorder
	.set noat

	.global kernelSyscall
	.global get_power_address

	.ent    kernelSyscall
kernelSyscall:
	jal kernel_permission_call
	nop
# sceInterruptManager+0xDD8 in 6.35
	lui $ra, 0x8802
	addiu $ra, $ra, 0x3DD8
	jr $ra
	nop

	.end kernelSyscall

	.ent    get_power_address
get_power_address:
	addiu $sp, $sp, -8
	sw $ra, 0($sp)
	move $a1, $a0
	jal scePowerRegisterCallbackPrivate
	move $a0, $0
	lw $ra, 0($sp)
	move $v0, $v1
	jr   $ra
	addiu $sp, $sp, 8

	.end get_power_address
