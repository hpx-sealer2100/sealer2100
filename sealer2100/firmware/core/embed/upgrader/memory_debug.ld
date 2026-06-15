/* Trezor v2 bootloader linker script */

ENTRY(reset_handler)

MEMORY {
  FLASH  (rx)  : ORIGIN = 0x08000000, LENGTH = 384K
  AXIRAM (wal) : ORIGIN = 0x24000000, LENGTH = 512K
  SRAM   (wal) : ORIGIN = 0x20000000, LENGTH = 128K
  SRAM4   (wal) : ORIGIN = 0x38000000, LENGTH = 64K
}

main_stack_base = ORIGIN(AXIRAM) + LENGTH(AXIRAM); /* 栈基址在 AXIRAM 顶部 */

/* used by the startup code to populate variables used by the C code */
data_lma = LOADADDR(.data);
data_vma = ADDR(.data);
data_size = SIZEOF(.data);

/* used by the startup code to wipe memory */
axiram_start = ORIGIN(AXIRAM);
axiram_end = ORIGIN(AXIRAM) + LENGTH(AXIRAM);

/* used by the startup code to wipe memory */
sram_start = ORIGIN(SRAM);
sram_end = ORIGIN(SRAM) + LENGTH(SRAM);

_codelen = SIZEOF(.flash) + SIZEOF(.data);

SECTIONS {
  .flash : ALIGN(512) {
    _vector_offset = .;
    KEEP(*(.vector_table));
    . = ALIGN(1024);
    *(.text*);
    . = ALIGN(4);
    *(.rodata*);
    . = ALIGN(4);
    *(.ARM.exidx*);
    . = ALIGN(512);
  } >FLASH AT>FLASH

  .data : ALIGN(4) {
    *(.data*);
    . = ALIGN(512);
  } >SRAM AT>FLASH

  .bss : ALIGN(4) {
    *(.bss*);
    . = ALIGN(4);
  } >SRAM

  .heap (NOLOAD) : ALIGN(4)
  {
    PROVIDE ( end = .);
    PROVIDE ( _end = .);
    __HeapBase  = .;
    __HeapLimit = .;
  } >SRAM

  .stack : ALIGN(8) {
    . = 512; /* this acts as a build time assertion that at least this much memory is available for stack use */
  } >AXIRAM
}
