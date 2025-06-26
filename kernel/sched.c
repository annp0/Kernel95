#include <kernel/sched.h>
#include <asm/system.h>
#include <kernel/head.h>
#include <errno.h>
#include <string.h>
#include <kernel/kernel.h>
#include <asm/io.h>
#include <kernel/fork.h>

#define COUNTER 100

#define PAGE_SIZE 4096
#define LATCH (1193180/HZ)

long user_stack[PAGE_SIZE >> 2];

extern void timer_interrupt();
extern int system_call();

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
static int cnt = 0;
static int isFirst = 1;


void do_timer(long cpl) {
    cnt++;

    if (cnt > 2) {
        printk("cnt is %d\n\r", cnt);
        return;
    }

    if (clock >0 && clock <= COUNTER) {
        clock--;
    }
    else if (clock == 0) {
        clock = COUNTER;
        if (isFirst) {
            isFirst = 0;
            switch_to(1);
        }
        else {
            isFirst = 1;
            switch_to(0);
        }
    }
    else {
        clock = COUNTER;
    }
    cnt--;
}


void sched_init(){
    int i;
    struct desc_struct* p;
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

    create_second_process();

    __asm__("pushfl; andl $0xffffbfff, (%esp); popfl");
    ltr(0);
    lldt(0);
    set_system_gate(0x80, &system_call);

    /* open the clock interruption! */
    outb_p(0x36, 0x43);
    outb_p(LATCH & 0xff, 0x40);
    outb(LATCH >> 8, 0x40);
    set_intr_gate(0x20, &timer_interrupt);
    outb(inb_p(0x21) & ~0x01, 0x21);

    set_system_gate(0x80, &system_call);
}

int create_second_process() {
    struct task_struct *p;
    int nr;

    nr = find_empty_process();
    if (nr < 0)
        return -EAGAIN;

    p = (struct task_struct*) get_free_page();
    memcpy(p, current, sizeof(struct task_struct));

    set_tss_desc(gdt+(nr<<1)+FIRST_TSS_ENTRY,&(p->tss));
    set_ldt_desc(gdt+(nr<<1)+FIRST_LDT_ENTRY,&(p->ldt));

    memcpy(&p->tss, &current->tss, sizeof(struct tss_struct));

    p->tss.eip = (long)test_b;
    p->tss.ldt = _LDT(nr);
    p->tss.ss0 = 0x10;
    p->tss.esp0 = PAGE_SIZE + (long)p;
    p->tss.ss  = 0x10;
    p->tss.ds  = 0x10;
    p->tss.es  = 0x10;
    p->tss.cs  = 0x8;
    p->tss.fs  = 0x10;
    p->tss.esp = PAGE_SIZE + (long)p;
    p->tss.eflags = 0x602;

    task[nr] = p;
    return nr;
}

void test_a(void) {
__asm__("movl $0, %edi\n\r"
        "movl $0x17, %eax\n\t"
        "movw %ax, %ds \n\t"
        "movw %ax, %es \n\t"
        "movw %ax, %fs \n\t"
        "movw $0x18, %ax\n\t"
        "movw %ax, %gs \n\t"
        "movb $0x0c, %ah\n\r"
        "movb $'A', %al\n\r"
        "loopa:\n\r"
        "movw %ax, %gs:(%edi)\n\r"
        "jmp loopa");
}

void test_b(void) {
__asm__("movl $0, %edi\n\r"
        "movw $0x18, %ax\n\t"
        "movw %ax, %gs \n\t"
        "movb $0x0f, %ah\n\r"
        "movb $'B', %al\n\r"
        "loopb:\n\r"
        "movw %ax, %gs:(%edi)\n\r"
        "jmp loopb");
}