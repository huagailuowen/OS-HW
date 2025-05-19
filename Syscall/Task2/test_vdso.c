#include <unistd.h>
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>


typedef int (*get_pid_fn_t)(void);
typedef char* (*get_comm_fn_t)(char*, int);
typedef void *(*get_vtask_base_fn_t)(void);
typedef struct task_struct *(*get_task_struct_fn_t)(void);

int main() {
    // ./testfiles/test_vdso
    printf("测试 VDSO 获取进程信息\n");
    // sleep(100000);
    void *vdso_handle = dlopen("linux-vdso.so.1", RTLD_LAZY);
    if (!vdso_handle) {
        fprintf(stderr, "无法加载 vdso: %s\n", dlerror());
        return 1;
    }
    
    // /* 获取 vdso 函数指针 */
    get_pid_fn_t get_task_pid = (get_pid_fn_t)dlsym(vdso_handle, "__vdso_get_task_pid");
    get_comm_fn_t get_task_comm = (get_comm_fn_t)dlsym(vdso_handle, "__vdso_get_task_comm");
    get_vtask_base_fn_t get_vtask_base = (get_vtask_base_fn_t)dlsym(vdso_handle, "__vdso_get_vtask_base");
    get_task_struct_fn_t get_task_struct = (get_task_struct_fn_t)dlsym(vdso_handle, "__vdso_get_task_struct");


    if (!get_task_pid || !get_task_comm || !get_vtask_base || !get_task_struct) {
        fprintf(stderr, "无法获取 vdso 函数: %s\n", dlerror());
        dlclose(vdso_handle);
        return 1;
    }
    fprintf(stderr, "获取函数指针成功\n");
    // /* 获取进程信息 */
    void *vtask_base = get_vtask_base();
    if (vtask_base) {
        printf("get_vtask_base 返回的地址: %p\n", vtask_base);
    } else {
        printf("get_vtask_base 返回 NULL\n");
    }
    // 测试 get_task_struct
    if (get_task_struct) {
        struct task_struct *task = get_task_struct();
        if (task) {
            printf("get_task_struct 返回的地址: %p\n", (void*)task);
            // 这里只能打印地址，无法直接访问结构体内容（除非你知道结构体定义且内存布局兼容）
        } else {
            printf("get_task_struct 返回 NULL\n");
        }
    } 


    int pid = get_task_pid();
    char comm[32] = {0};
    get_task_comm(comm, sizeof(comm));
    
    printf("从 VDSO 获取的进程信息:\n");
    printf("  PID: %d\n", pid);
    printf("  进程名: %s\n", comm);
    
    /* 通过系统调用对比 */
    printf("\n通过系统调用获取的信息:\n");
    printf("  PID: %d\n", getpid());
    
    dlclose(vdso_handle);
    return 0;
}