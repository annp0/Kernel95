.code32
.text
.global startup_32

startup_32:
    # GDT fourth entry, index 3
    movl $0x18, %eax
    movw %ax, %gs
    movl $0, %edi
    # white color
    movb $0xf, %ah
    # 'B'
    movb $0x42, %al
    movw %ax, %gs:(%edi)

    # GDT third entry, index 2
    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %es 
    movw %ax, %fs

loop:
    jmp loop