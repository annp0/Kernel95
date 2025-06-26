.code32
.text
.global system_call

int_msg:
    .asciz "in kernel interrupt\n\r"

system_call:
    pushl %eax
    pushl %ecx
    pushl %edx
    pushl %ebx
    pushl %esi
    pushl %edi
    pushw %ds
    pushw %es
    pushw %fs

    movl $0x10, %eax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs

    pushl $int_msg
    call printk
    popl %eax

    popw %fs
    popw %es
    popw %ds
    popl %edi
    popl %esi
    popl %ebx
    popl %edx
    popl %ecx
    popl %eax
    iret

