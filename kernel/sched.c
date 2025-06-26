#include <kernel/sched.h>
#include <asm/system.h>
#include <kernel/head.h>

#define PAGE_SIZE 4096

long user_stack[PAGE_SIZE >> 2];

extern int system_call();

union task_union {
    struct task_struct task;
    char stack[PAGE_SIZE];
};

static union task_union init_task = {INIT_TASK, };

long user_stack[PAGE_SIZE >> 2];

struct
{
    long *a;
    short b;
} stack_start = {&user_stack[PAGE_SIZE >> 2], 0x10};

void sched_init(){
    struct desc_struct* p;
    set_tss_desc(gdt + FIRST_TSS_ENTRY, &(init_task.task.tss));
    set_ldt_desc(gdt + FIRST_LDT_ENTRY, &(init_task.task.ldt));
    __asm__("pushfl; andl $0xffffbfff, (%esp); popfl");
    ltr(0);
    lldt(0);
    set_system_gate(0x80, &system_call);
}