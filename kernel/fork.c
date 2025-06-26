#include <errno.h>

#include <asm/system.h>
#include <kernel/sched.h>

int find_empty_process() {
    int i;
    for (i = 1; i < NR_TASKS; i++) {
        if (!task[i])
            return i;
    }

    return -EAGAIN;
}
