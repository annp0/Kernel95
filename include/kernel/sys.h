extern int sys_fork();

fn_ptr sys_call_table[] = {
    sys_fork,
};

int NR_syscalls = sizeof(sys_call_table)/sizeof(fn_ptr);
