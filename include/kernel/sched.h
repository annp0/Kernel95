#ifndef _SCHED_H
#define _SCHED_H

#define HZ 100

#define NR_TASKS 64

#define FIRST_TASK task[0]
#define LAST_TASK task[NR_TASKS-1]

#include <kernel/head.h>
#include <kernel/mm.h>

void sched_init();

void test_a();
void test_b();
int create_second_process();

extern struct task_struct *task[NR_TASKS];
extern struct task_struct *current;

struct tss_struct {
    long back_link;
    long esp0;
    long ss0;
    long esp1;
    long ss1;
    long esp2;
    long ss2;
    long cr3;
    long eip;
    long eflags;
    long eax, ecx, edx, ebx;
    long esp;
    long ebp;
    long esi;
    long edi;
    long es;
    long cs;
    long ss;
    long ds;
    long fs;
    long gs;
    long ldt;
    long trace_bitmap;
};

struct task_struct{
    struct desc_struct ldt[3];
    struct tss_struct tss;
};

#define INIT_TASK \
{                   \
    {               \
        {0, 0},     \
        {0x09f, 0xc0fa00},   \
        {0x09f, 0xc0f200},   \
    },              \
    {0, PAGE_SIZE + (long)&init_task, 0x10, 0, 0, 0, 0, (long)&pg_dir, \
        0, 0, 0, 0, 0, 0, 0, 0, \
        0, 0, 0x17, 0x17, 0x17, 0x17, 0x17, 0x17,   \
        _LDT(0), 0x80000000,    \
    },              \
}

/*
 * In linux is 4, because we add video selector,
 * so, it is 5 here.
 * */
#define FIRST_TSS_ENTRY 5
#define FIRST_LDT_ENTRY (FIRST_TSS_ENTRY + 1)
#define _TSS(n) ((((unsigned long)n) << 4) + (FIRST_TSS_ENTRY << 3))
#define _LDT(n) ((((unsigned long)n) << 4) + (FIRST_LDT_ENTRY << 3))
#define ltr(n) __asm__("ltr %%ax"::"a"(_TSS(n)))
#define lldt(n) __asm__("lldt %%ax"::"a"(_LDT(n)))


#define switch_to(n) {\
    struct {long a,b;} __tmp; \
    __asm__("cmpl %%ecx,current\n\t" \
            "je 1f\n\t" \
            "movw %%dx,%1\n\t" \
            "xchgl %%ecx,current\n\t" \
            "ljmp *%0\n\t" \
            "1:" \
            ::"m" (*&__tmp.a),"m" (*&__tmp.b), \
            "d" (_TSS(n)),"c" ((long) task[n])); \
}


#endif
