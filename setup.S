INITSEG = 0x9000
SYSSEG  = 0x1000
SETUPSEG = 0x9020

.code16
.text
.global _start_setup

_start_setup:
    # basic setup
    movw %cs, %ax
    movw %ax, %ds
    movw %ax, %es

    # print 
    movw $setup_msg, %ax
    movw %ax, %bp
    movw $0x1301, %ax
    movw $0x0c, %bx
    movw $16, %cx
    movb $3, %dh
    movb $0, %dl
    int $0x10

    # gather hardware info
    movw $INITSEG, %ax
    movw %ax, %ds
    # function 0x03 of int 0x10
    movb $0x03, %ah
    # zeroing bh
    xor %bh, %bh
    int $0x10
    # moving info to DS:0 (0x9000 << 4 + 0)
    # this will overwrite the bootloader (it is not going to be used)
    # DX stores the location of the cursor
    movw %dx, (0)
    # function 0x88 of int 0x15, get RAM size
    movb $0x88, %ah
    int $0x15
    # 0x90002
    movw %ax, (2)

    # info on video display
    # 0x0f of 0x10, gather display mode
    movb $0x0f, %ah
    int $0x10
    # display mode
    movw %bx, (4)
    # Num of pages
    movw %ax, (6)
    # function 0x12
    movb $0x12, %ah
    movb $0x10, %bl
    int $0x10
    # some other infomation that will be used later for initializing console
    movw %ax, (8)
    movw %bx, (10)
    movw %cx, (12)

    # hard disk info
    movw $0x0000, %ax 
    movw %ax, %ds
    # hd0 (floppy)
    ldsw (4 * 0x41), %si
    movw $INITSEG, %ax
    movw %ax, %es
    movw $0x0080, %di
    movw $16, %cx
    # from 0x0:(4*0x41) to 0x9000:0x80, 16 bytes
    rep
    movsb

    # hd1 info
    movw $0x0000, %ax
    movw %ax, %ds
    ldsw (4 * 0x46), %si
    movw $INITSEG, %ax
    movw %ax, %es
    movw $0x0090, %di
    movw $16, %cx
    # to 0x9000:0x90
    rep
    movsb

    # detect other hard drives
    movw $0x1500, %ax
    movb $0x81, %dl
    int  $0x13
    # if carry - no disk
    jc no_disk1
    # AH = 1 / 2 - floppy
    # AH = 3 -> hard drive
    cmpb $3, %ah
    je is_disk1

no_disk1:
    movw $INITSEG, %ax
    movw %ax, %es
    movw $0x0090, %di
    movw $16, %cx
    movw $0x00, %ax
    # zero 0x900A0 
    rep
    stosb

is_disk1:
    cli

    movw $0x0000, %ax
    cld
do_move:
    movw %ax, %es
    addw $0x1000, %ax
    cmpw $0x9000, %ax
    jz   end_move
    movw %ax, %ds
    subw %di, %di
    subw %si, %si
    movw $0x8000, %cx
    rep
    movsw
    jmp  do_move

end_move:
    movw $0xb800, %ax
    movw %ax, %gs
    movb $0xf, %ah
    movb $0x41, %al
    movl $0x100, %edi
    movw %ax, %gs:(%di)

    movw $SETUPSEG, %ax
    movw %ax, %ds
    lgdt gdt_48

    call empty_8042
    movb $0xD1, %al
    outb %al,  $0x64
    call empty_8042
    movb $0xDF, %al
    outb %al,  $0x60
    call empty_8042

    movl %cr0, %eax
    xorb $1, %al
    movl %eax, %cr0

    .byte 0x66, 0xea
    .long 0x0
    .word 0x8




setup_msg:
    .ascii "Setup is running"
