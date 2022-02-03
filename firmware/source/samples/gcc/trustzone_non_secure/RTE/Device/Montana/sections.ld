/**
 * @file sections.ld
 * @brief Simple sections load file
 *
 * @copyright @parblock
 * Copyright (c) 2021 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */
MEMORY
{
  FLASH              (rx)  : ORIGIN = 0x00100000, LENGTH = 64K     /* Flash code region */
  FLASH_SG           (rx)  : ORIGIN = 0x00110000, LENGTH = 8K      /* Flash code secure gateway region */
  FLASH_NS           (rx)  : ORIGIN = 0x00120000, LENGTH = 64K     /* Flash code non secure region */
  DRAM               (rwx) : ORIGIN = 0x20000000, LENGTH = 32K     /* DRAM region */
  DRAM_NS            (rwx) : ORIGIN = 0x20008000, LENGTH = 32K     /* DRAM non-secure region */
}

/* ----------------------------------------------------------------------------
 * Stack related defines and provided variables
 * ------------------------------------------------------------------------- */
__stack = ORIGIN(DRAM_NS) + LENGTH(DRAM_NS);

PROVIDE ( __stack = __stack ) ;

/*
 * Default stack sizes.
 * These are used by the startup in order to allocate stacks 
 * for the different modes.
 */
__Main_Stack_Size = 1024 ;
PROVIDE ( _Main_Stack_Size = __Main_Stack_Size ) ;

__Main_Stack_Limit = __stack  - __Main_Stack_Size ;
PROVIDE ( _Main_Stack_Limit = __Main_Stack_Limit ) ;
PROVIDE ( __stack_limit = __Main_Stack_Limit);

/* ----------------------------------------------------------------------------
 * Heap related defines and provided variables
 * Heap starts where the last stackable item ends on stack
 * ------------------------------------------------------------------------- */
PROVIDE ( __Heap_Begin__ = __noinit_end__ ) ;
PROVIDE ( __Heap_Limit__ = __stack - __Main_Stack_Size ) ;

/* 
 * The entry point is informative, for debuggers and simulators,
 * since the Cortex-M vector points to it anyway.
 */
ENTRY(Reset_Handler)


/* Sections Definitions */

SECTIONS
{
    /*
     * For Cortex-M devices, the beginning of the startup code is stored in
     * the .interrupt_vector section, which goes to FLASH 
     */
    .text :
    {
        . = ALIGN(4);
        KEEP(*(.interrupt_vector)) /* Interrupt vectors */
        . = ALIGN(4);
        /* 
         * This section is here to store the startup code immediately after
         * the interrupt vectors, as required by the program ROM.
         */
        KEEP(*(.reset))

        /* Pre-initialization Code */
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start__ = .);

        /* System initialization and the platform initialization (if present)
         * should be first */
        KEEP(*(.preinit_array_sysinit .preinit_array_sysinit.*))
        KEEP(*(.preinit_array_platform .preinit_array_platform.*))

        /* Pre-initialization functions (to be executed before C++ 
         * constructors are run) */
        KEEP(*(.preinit_array .preinit_array.*))

        PROVIDE_HIDDEN (__preinit_array_end__ = .);

        /* Initialization Code */
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start__ = .);

        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))

        PROVIDE_HIDDEN (__init_array_end__ = .);

        /*
         * The program code is stored in the .text section, 
         * which goes to FLASH.
         */
        . = ALIGN(4);
      
        *(.text .text.*)            /* all remaining code */
        *(.rodata .rodata.*)        /* read-only data (constants) */
        *(vtable)            /* C++ virtual tables */

    } >FLASH_NS
    


    /* 
     * This address is used by the startup code to 
     * initialise the .data section.
     */
    . = ALIGN(4);
    __data_init__ = .;


   
    /*
     * The initialised data section.
     * The program executes knowing that the data is in the RAM
     * but the loader puts the initial values in the FLASH (inidata).
     * It is one task of the startup to copy the initial values from 
     * FLASH to RAM.
     */
    .data  : AT ( __data_init__ )
    {
        . = ALIGN(4);

        /* This is used by the startup code to initialise the .data section */
        __data_start__ = . ;
        *(.data_begin .data_begin.*)
        *(.data .data.*)
        *(.data_end .data_end.*)
        . = ALIGN(4);

        /* This is used by the startup code to initialise the .data section */
        __data_end__ = . ;

    } >DRAM_NS
      
    /*
     * The uninitialised data section. NOLOAD is used to avoid
     * the "section `.bss' type changed to PROGBITS" warning
     */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss_begin .bss_begin.*)

        *(.bss .bss.*)
        *(COMMON)
        
        *(.bss_end .bss_end.*)
        . = ALIGN(4);
        __bss_end__ = .;
    } >DRAM_NS
    
    .noinit (NOLOAD) :
    {
        . = ALIGN(4);
        __noinit_start__ = .;
        
        *(.noinit .noinit.*) 
        
         . = ALIGN(4) ;
        __noinit_end__ = .;   
    } > DRAM_NS

}
