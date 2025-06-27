#ifndef _UNISTD_H
#define _UNISTD_H

#ifndef NULL
#define NULL ((void*)0)
#endif

#define __NR_fork 0

#define _syscall0(type, name)   \
type name() {                   \
    long __res;                 \
    __asm__ volatile("int $0x80\n\r"\
        : "=a"(__res)           \
        : "a"(__NR_##name));     \
    if (__res >= 0)             \
        return (type)__res;     \
    errno = -__res;             \
    return -1;                  \
}

extern int errno;

static int fork();

#endif
