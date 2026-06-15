ENTRY(reset_handler)

MEMORY {
  EXRAM  (xrw) : ORIGIN = 0xD1D00000, LENGTH = 3M
  AXIRAM (wal) : ORIGIN = 0x24000000, LENGTH = 512K
  SRAM   (wal) : ORIGIN = 0x20000000, LENGTH = 128K
}

main_stack_base = ORIGIN(AXIRAM) + LENGTH(AXIRAM);

/* used by the startup code to wipe memory */
axiram_start = ORIGIN(AXIRAM);
axiram_end = ORIGIN(AXIRAM) + LENGTH(AXIRAM);

/* used by the startup code to wipe memory */
sram_start = ORIGIN(SRAM);
sram_end = ORIGIN(SRAM) + LENGTH(SRAM);

SECTIONS {
  .text : ALIGN(4) {
    _vector_offset = .;
    KEEP(*(.vector_table));
    . = ALIGN(1024);
    *(.text*);
    . = ALIGN(4);
    *(.rodata*);
    . = ALIGN(4);
    *(.ARM.exidx*);
    . = ALIGN(4);
  } > EXRAM

  .data : ALIGN(4) {
    *(.data*);
    . = ALIGN(4);
  } > EXRAM

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
