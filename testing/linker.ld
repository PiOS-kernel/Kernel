ENTRY(_start)

MEMORY
{
	FLASH(rx):ORIGIN =0x00000000,LENGTH =256K
	SRAM(rwx):ORIGIN =0x20000000,LENGTH =64K
}


SECTIONS
{
    .text :
    {
        *(.isr_vector)
        *(.text)
        *(.rodata)
        . = ALIGN(4);
        _etext = .;
    }> FLASH

    _la_data = LOADADDR(.data);

    .data :
    {
        _sdata = .;
        *(.data)
        . = ALIGN(4);
        _edata = .;
    }> SRAM AT> FLASH

    .bss :
    {
        _sbss = .;
        __bss_start__ = _sbss;
        *(.bss)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
        __bss_end__ = _ebss;
        . = ALIGN(4); 
        end = .;
        __end__ = .;
    }> SRAM

    __entryKernel = __end__;
  
}