/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright 2008-2013 Code Red Technologies Ltd,
 * Copyright 2013-2024 NXP
 * Generated linker script file for MCXA153
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v11.9.0_prc2 [Build 2107] [2023-11-24] on Feb 19, 2024, 10:58:44 AM
 */

MEMORY
{
  /* Define each memory region */
  PROGRAM_FLASH (rx) : ORIGIN = 0x0, LENGTH = 0x20000 /* 128K bytes (alias Flash) */  
  SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x6000 /* 24K bytes (alias RAM) */  
}

  /* Define a symbol for the top of each memory region */
  __base_PROGRAM_FLASH = 0x0  ; /* PROGRAM_FLASH */  
  __base_Flash = 0x0 ; /* Flash */  
  __top_PROGRAM_FLASH = 0x0 + 0x20000 ; /* 128K bytes */  
  __top_Flash = 0x0 + 0x20000 ; /* 128K bytes */  
  __base_SRAM = 0x20000000  ; /* SRAM */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_SRAM = 0x20000000 + 0x6000 ; /* 24K bytes */  
  __top_RAM = 0x20000000 + 0x6000 ; /* 24K bytes */  
