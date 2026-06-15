    .syntax unified

#include "version.h"

    .section .header, "a"
    .global header
    .type header, %object
    .balign 4


header:
    .word 0x42545048                 // `magic: BOOTLOADER_IMAGE_MAGIC`
    .byte VERSION_BUILD              // `version.build`
    .byte VERSION_PATCH              // `version.patch`
    .byte VERSION_MINOR              // `version.minor`
    .byte VERSION_MAJOR              // `version.major`
    .word 0x400                      // `header_size`
    .word _codelen                   // `code_size`
    .word 2                          // `required_sig_count: KEY_M`
    .space 0x80-20                   // `_reserved`
    .space 0x400 - 0x80              // `unprotected, filled zero`
