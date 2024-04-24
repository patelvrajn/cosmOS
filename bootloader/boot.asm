ORG 0x7C00 ; Bootloader should be loaded by the BIOS into memory address 0x7C00.
BITS 16    ; Generate code designed to run on a processor operating in 16 bit (real) mode.

start:
    mov ah, 0Eh ; BIOS Interrupt Call ; VIDEO - TELETYPE OUTPUT
    mov al, 'A' ; Output character 'A' to the terminal.
    int 0x10 

    jmp $ ; Jump to this line forever i.e. don't execute the zeroes and the bootloader signature.

# $ = current address of this assembly instruction
# $$ = address of the beginning of the section
# ($ - $$) = length of the section so far
# times db = Fills the rest of the section upto 510 bytes with zeroes.  
times 510 - ($ - $$) db 0

# Bootloader signature (0xAA55 in little endian.)
# AA is the last byte of the sector.
dw 0xAA55
