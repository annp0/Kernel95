#include <kernel/sched.h>
#include <asm/system.h>
#include <kernel/head.h>

#define PAGE_SIZE 4096

long user_stack[PAGE_SIZE >> 2];

struct
{
    long *a;
    short b;
} stack_start = {&user_stack[PAGE_SIZE >> 2], 0x10};

extern int system_call();

void sched_init(){
    set_intr_gate(0x80, &system_call);
}