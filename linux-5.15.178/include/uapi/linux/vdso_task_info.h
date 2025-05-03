#ifndef _UAPI_LINUX_VDSO_TASK_INFO_H
#define _UAPI_LINUX_VDSO_TASK_INFO_H

#include <linux/types.h>

struct task_info {
    pid_t pid;
    void *task_struct_ptr;
    char comm[16];      /* 进程名称 */
    unsigned int state; /* 进程状态 */
    int prio;           /* 进程优先级 */
    pid_t ppid;         /* 父进程ID */
};

int get_task_struct_info(struct task_info *info);

#endif /* _UAPI_LINUX_VDSO_TASK_INFO_H */