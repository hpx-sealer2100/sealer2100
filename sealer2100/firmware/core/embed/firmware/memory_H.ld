/* TREZORv2 firmware linker script */

ENTRY(reset_handler)

MEMORY {
  FLASH  (rx)  : ORIGIN = 0x08060000, LENGTH = 1536K
  FLASH2 (r)   : ORIGIN = 0x90000000, LENGTH = 3M
  AXIRAM (wal) : ORIGIN = 0x24000000, LENGTH = 512K
  /* DTCM */
  SRAM   (wal) : ORIGIN = 0x20000000, LENGTH = 128K
  EXRAM   (wal) : ORIGIN = 0xD1D00000, LENGTH = 3M
}

main_stack_base = ORIGIN(SRAM) + LENGTH(SRAM); /* 8-byte aligned full descending stack */
_sstack = main_stack_base - SIZEOF(.stack);
_estack = main_stack_base;

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
_ram_start = sram_start;
_ram_end = sram_end;

_codelen = LENGTH(FLASH) + SIZEOF(.flash2);
_flash_start = ORIGIN(FLASH);
_flash_end = ORIGIN(FLASH) + LENGTH(FLASH);
_heap_start = ADDR(.heap);
_heap_end = ADDR(.heap) + SIZEOF(.heap);

_flash2_load_addr = LOADADDR(.flash2);
_flash2_start = ORIGIN(EXRAM);
_flash2_end = _flash2_start + SIZEOF(.flash2);

SECTIONS {
  .flash2 : ALIGN(256) {
    build/firmware/frozen_mpy.o(.rodata*);
    build/firmware/frozen_mpy.o(.text*);
    *librust_c.a:*(.text .text*);
    *librust_c.a:*(.rodata*);
    build/firmware/embed/extmod/modairgapped/modairgapped.o(.text .text*);
    build/firmware/embed/extmod/modairgapped/modairgapped.o(.rodata*);
    . = ALIGN(256);
  } >EXRAM AT>FLASH2

  .flash : ALIGN(512) {
    KEEP(*(.header));
    . = ALIGN(4);
    _vector_offset = .;
    KEEP(*(.vector_table));
    . = ALIGN(4);
    *(.text*);
    . = ALIGN(4);
    *(.rodata*);
    . = ALIGN(4);
  } >FLASH AT>FLASH

  .data : ALIGN(4) {
    *(.data*);
    . = ALIGN(512);
  } >SRAM AT>FLASH

  .bss : ALIGN(4) {
    *(.bss*);
    . = ALIGN(4);
  } >SRAM

  .stack : ALIGN(8) {
    . = 32K; /* this acts as a build time assertion that at least this much memory is available for stack use */
  } >SRAM

  .heap : ALIGN(4) {
    PROVIDE ( end = . );
    PROVIDE ( _end = . );
    . = 512K; /* this acts as a build time assertion that at least this much memory is available for heap use */
    /*. = ABSOLUTE(sram_end - 16K);  this explicitly sets the end of the heap effectively giving the stack at most 16K */
  } >AXIRAM
}
