// SPDX-License-Identifier: GPL-2.0-only
/*
 * Fast user context implementation of task_struct access functions.
 * 
 * Allows user space to access process information without syscalls.
 */
#include <linux/compiler.h>
#include <linux/types.h>
#include <asm/processor.h>
#include <asm/vdso.h>
#include <asm/unistd.h>
/* 直接使用内核的 sched.h 定义 task_struct */
#include <linux/sched.h>
#include <asm/vgtod.h>       /* 包含 vdso/vvar 相关定义 */
#include <asm/unistd.h>
#include <vdso/datapage.h>   /* 包含 _vdso_data 相关定义 */
#include <asm/vvar.h>

#include <asm/vdso/gettimeofday.h>

/* vvar 区域相关声明 */
// extern void* vvar_page;
// extern char _vdso_data[];
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif


#ifndef TASK_COMM_LEN
#define TASK_COMM_LEN 16
#endif



/* 简化的 task_struct 结构，与内核中的偏移量保持一致 */


/*
 * 获取 vtask 区域基址
 * 使用特定于 VDSO 的符号或技术获取基址
 */

 
inline void *__vdso_get_vtask_base(void)
{
    
    // /* 假设vtask区域在vvar区域前两页 */
    
    // printf( "[vdso-access] vvar_page address, in vtask_base: %px\n", &vvar_page- 6 * PAGE_SIZE);
    return (void *)((void *)(__arch_get_vdso_data()) - 6 * 4096 - 8*16);
}
void * vdso_get_vtask_base(void) __attribute__((weak, alias("__vdso_get_vtask_base")));

inline struct vtask_metadata *__vdso_get_vtask_metadata(void)
{
    return (struct vtask_metadata *)__vdso_get_vtask_base();
}

inline struct task_struct *__vdso_get_task_struct(void)
{
    const struct vtask_metadata *meta;
    void *task_page;
    
    // printf( "[vdso-access] Entering __vdso_get_task_struct\n");
    meta = __vdso_get_vtask_metadata();
    
    // printf( "[vdso-access] meta address: %px\n", meta);
    // printf( "[vdso-access] meta->magic: 0x%lx\n", (unsigned long)meta->magic);
    // printf( "[vdso-access] meta->pid: %d\n", meta->pid);
    // printf( "[vdso-access] meta->task_offset: 0x%lx\n", (unsigned long)meta->task_offset);
    // printf( "[vdso-access] meta->task_struct_addr: 0x%lx\n", (unsigned long)meta->task_struct_addr);
    // printf( "[vdso-access] meta->timestamp: 0x%lx\n", (unsigned long)meta->timestamp);
    
    // return (struct task_struct *)meta->magic;
    /* 验证元数据有效性 */
    if (!meta || meta->magic != 0x5441534B)  /* "TASK" in hex */
        return NULL;
    
    /* 通过 PID 进行额外验证 */
    if (meta->pid <= 0)
        return NULL;
    

    /* task_struct 页在元数据页后面 */
    task_page = (char *)__vdso_get_vtask_base() + PAGE_SIZE;
    
    // if(meta->task_offset != -114)
    //     return NULL;
    /* 使用偏移量计算 task_struct 的精确地址 */
    return (struct task_struct *)((char *)task_page + meta->task_offset);
}

/* VDSO 导出函数 */
int __vdso_get_task_pid(void)
{
    struct task_struct *task;
    task = __vdso_get_task_struct();
    
    if (!task)
        return -1;
    // printf( "[vdso-access] Successfully read task_struct\n");
    // printf( "[vdso-access] Successfully read task->pid: %d\n", task->pid);
    return task->pid;
}

/* 别名 - 使用 vdso_ 前缀避免与内核符号冲突 */
int vdso_get_task_pid(void) __attribute__((weak, alias("__vdso_get_task_pid")));

int __vdso_get_task_tgid(void)
{
    struct task_struct *task;
    
    task = __vdso_get_task_struct();
    if (!task)
        return -1;
        
    return task->tgid;
}

int vdso_get_task_tgid(void) __attribute__((weak, alias("__vdso_get_task_tgid")));

char *__vdso_get_task_comm(char *buf, int size)
{
    struct task_struct *task;
    int len;
    
    if (!buf || size <= 0)
        return NULL;
        
    task = __vdso_get_task_struct();
    if (!task)
        return NULL;
        
    /* 安全复制进程名 */
    len = size < TASK_COMM_LEN ? size - 1 : TASK_COMM_LEN - 1;
    memcpy(buf, task->comm, len);
    buf[len] = '\0';
    
    return buf;
}

char *vdso_get_task_comm(char *buf, int size) __attribute__((weak, alias("__vdso_get_task_comm")));

/* 附加功能：验证映射是否有效 */
int __vdso_check_task_mapping(void)
{
    struct vtask_metadata *meta = __vdso_get_vtask_metadata();
    
    if (!meta || meta->magic != 0x5441534B)
        return 0;  /* 无效 */
        
    return 1;  /* 有效 */
}

int vdso_check_task_mapping(void) __attribute__((weak, alias("__vdso_check_task_mapping")));

/* 附加功能：获取 task_struct 地址（用于调试） */
unsigned long __vdso_get_task_struct_addr(void)
{
    struct vtask_metadata *meta = __vdso_get_vtask_metadata();
    
    if (!meta || meta->magic != 0x5441534B)
        return 0;
        
    return meta->task_struct_addr;
}

unsigned long vdso_get_task_struct_addr(void) __attribute__((weak, alias("__vdso_get_task_struct_addr")));

/* 附加功能：获取更新时间戳 */
unsigned long __vdso_get_task_timestamp(void)
{
    struct vtask_metadata *meta = __vdso_get_vtask_metadata();
    
    if (!meta || meta->magic != 0x5441534B)
        return 0;
        
    return meta->timestamp;
}

unsigned long vdso_get_task_timestamp(void) __attribute__((weak, alias("__vdso_get_task_timestamp")));