#define __LIBRARY__

#include <kernel/tty.h>
#include <kernel/kernel.h>

void main(void) 
{
    tty_init();
    printk("hello %d", 28);
    for (int i = 0; i < 25; i++) {
        printk("this is line %d\n\r", i);
    }
    __asm__("int $0x80 \n\r"::);
    __asm__ __volatile__(
            "loop:\n\r"
            "jmp loop"
            ::);
}