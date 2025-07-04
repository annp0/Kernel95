#include <string.h>
#include <errno.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>

#include <fcntl.h>
#include <sys/stat.h>

static int dupfd(unsigned int fd, unsigned int arg) {
    if (fd >= NR_OPEN || !current->filp[fd])
        return -EBADF;
    if (arg >= NR_OPEN)
        return -EINVAL;

    while (arg < NR_OPEN) {
        if (current->filp[arg])
            arg++;
        else
            break;
    }

    if (arg >= NR_OPEN)
        return -EMFILE;

    current->close_on_exec &= ~(1<<arg);
    (current->filp[arg] = current->filp[fd])->f_count++;
    return arg;
}

int sys_dup(unsigned int fildes) {
    return dupfd(fildes, 0);
}

