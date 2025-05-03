#include <linux/kernel.h>
#include <linux/sched.h>
#include <asm/vdso.h>
#include <linux/vdso_task_info.h>

notrace int __vdso_get_task_info(struct task_info __user *info)
{
    struct task_struct *task = current;
    struct task_info local_info;
    
    if (unlikely(!info))
        return -EINVAL;
    
    /* 填充任务信息 */
    local_info.pid = task->pid;
    local_info.task_struct_ptr = task;
    local_info.state = task->__state;
    local_info.prio = task->prio;
    
    /* 获取父进程ID */
    if (task->real_parent)
        local_info.ppid = task->real_parent->pid;
    else
        local_info.ppid = 0;
    
    /* 复制进程名称 */
    memcpy(local_info.comm, task->comm, sizeof(local_info.comm));
    
    /* 安全地复制到用户空间 */
    if (unlikely(copy_to_user(info, &local_info, sizeof(local_info))))
        return -EFAULT;
    
    return 0;
}