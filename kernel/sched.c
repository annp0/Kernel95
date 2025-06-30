#include <errno.h>
#include <kernel/sched.h>
#include <kernel/sys.h>

#include <asm/system.h>
#include <asm/io.h>

#include <unistd.h>
 
#define COUNTER 100

#define LATCH (1193180/HZ)
#define PAGE_SIZE 4096

extern int system_call();
extern void timer_interrupt();

union task_union {
    struct task_struct task;
    char stack[PAGE_SIZE];
};

static union task_union init_task = {INIT_TASK, };

struct task_struct * current = &(init_task.task);
struct task_struct * task[NR_TASKS] = {&(init_task.task), };

long user_stack[PAGE_SIZE >> 2];

struct
{
    long *a;
    short b;
} stack_start = {&user_stack[PAGE_SIZE >> 2], 0x10};


int clock = COUNTER;

void schedule() {
    int i,next,c;
    struct task_struct ** p;

    while(1) {
        c = -1;
        next = 0;
        i = NR_TASKS;
        p = &task[NR_TASKS];

        while (--i) {
            if (!*--p)
                continue;

            if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
                c = (*p)->counter, next = i;
        }

        if (c) break;
        for(p = &LAST_TASK ; p > &FIRST_TASK ; --p) {
            if (!(*p))
                continue;

            (*p)->counter = ((*p)->counter >> 1) + (*p)->priority;
        }
    }
    switch_to(next);
}

static inline void __sleep_on(struct task_struct** p, int state) {
    struct task_struct* tmp;

    if (!p)
        return;
    if (current == &(init_task.task))
        panic("task[0] trying to sleep");

    tmp = *p;
    *p = current;
    current->state = state;

repeat:
    schedule();

    if (*p && *p != current) {
        (**p).state = 0;
        current->state = TASK_UNINTERRUPTIBLE;
        goto repeat;
    }

    if (!*p)
        printk("Warning: *P = NULL\n\r");
    *p = tmp;
    if (*p)
        tmp->state = 0;
}

void interruptible_sleep_on(struct task_struct** p) {
    __sleep_on(p, TASK_INTERRUPTIBLE);
}

void sleep_on(struct task_struct** p) {
    __sleep_on(p, TASK_UNINTERRUPTIBLE);
}

void wake_up(struct task_struct **p) {
    if (p && *p) {
        if ((**p).state == TASK_STOPPED)
            printk("wake_up: TASK_STOPPED");
        if ((**p).state == TASK_ZOMBIE)
            printk("wake_up: TASK_ZOMBIE");
        (**p).state=0;
    }
}

void do_timer(long cpl) {
    if ((--current->counter)>0) return;
    current->counter=0;
    if (!cpl) return;
    schedule();
}

void sched_init() {
    int i;
    struct desc_struct * p;

    set_tss_desc(gdt + FIRST_TSS_ENTRY, &(init_task.task.tss));
    set_ldt_desc(gdt + FIRST_LDT_ENTRY, &(init_task.task.ldt));

    p = gdt+2+FIRST_TSS_ENTRY;

    for(i=1;i<NR_TASKS;i++) {
        task[i] = 0;
        p->a = p->b = 0;
        p++;
        p->a = p->b = 0;
        p++;
    }

    __asm__("pushfl; andl $0xffffbfff, (%esp); popfl");
    ltr(0);
    lldt(0);

    /* open the clock interruption! */
    outb_p(0x36, 0x43);
    outb_p(LATCH & 0xff, 0x40);
    outb(LATCH >> 8, 0x40);
    set_intr_gate(0x20, &timer_interrupt);
    outb(inb_p(0x21) & ~0x01, 0x21);

    set_system_gate(0x80, &system_call);
}

void test_a(void) {
    char b;
    int n = 0;
    while ((n = read(0, &b, 1)) > 0) {
        write(1, &b, n);
    }

__asm__("movl $0x0, %edi\n\r"
        "movw $0x1b, %ax\n\t"
        "movw %ax, %gs \n\t"
        "movb $0x0c, %ah\n\r"
        "movb $'A', %al\n\r"
        "loopa:\n\r"
        "movw %ax, %gs:(%edi)\n\r"
        "jmp loopa");
}

void test_b(void) {
__asm__("movl $0x0, %edi\n\r"
        "movw $0x1b, %ax\n\t"
        "movw %ax, %gs \n\t"
        "movb $0x0f, %ah\n\r"
        "movb $'B', %al\n\r"
        "loopb:\n\r"
        "movw %ax, %gs:(%edi)\n\r"
        "jmp loopb");
}
