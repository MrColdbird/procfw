OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)

ENTRY(main)

SECTIONS
{
  . = 0x88FC0000;
  .text.start : {
    *(.text.start)
  }
  .text : {
    *(.text)
  }
  .rodata : {
    *(.rodata)
  }
  .data : {
    *(.data)
  }
  .bss : {
    *(.bss)
  }
}
