#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <dlfcn.h>
#include <sys/wait.h>

typedef int (*get_pid_fn_t)(void);
typedef char* (*get_comm_fn_t)(char*, int);
typedef void *(*get_vtask_base_fn_t)(void);
typedef struct task_struct *(*get_task_struct_fn_t)(void);

int main() {
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


    pid_t pid_vdso = get_task_pid();
    std::ifstream stat_file("/proc/self/stat");
    if (!stat_file.is_open()) {
        std::cerr << "Failed to open /proc/self/stat" << std::endl;
        return 1;
    }
    pid_t pid_proc;
    stat_file >> pid_proc;
    stat_file.close();
    if (pid_proc != pid_vdso) {
        std::cerr << "PID mismatch: pid from /proc/self/stat is " << pid_proc
                  << ", pid from vDSO is " << pid_vdso << std::endl;
    } else {
        std::cout << "PID matches: " << pid_proc << std::endl;
    }
    const int num_forks = 5;
    for (int i = 0; i < num_forks; ++i) {
        pid_t pid = fork();
        if (pid < 0) {
            std::cerr << "Failed to fork" << std::endl;
            return 1;
        } else if (pid == 0) {
            
            pid_t child_pid_vdso = get_task_pid();
            std::ifstream child_stat_file("/proc/self/stat");
            if (!child_stat_file.is_open()) {
                std::cerr << "Child failed to open /proc/self/stat" << std::endl;
                exit(1);
            }
            pid_t child_pid_proc;
            child_stat_file >> child_pid_proc;
            child_stat_file.close();
            if (child_pid_proc != child_pid_vdso) {
                std::cerr << "Child PID mismatch: pid from /proc/self/stat is " << child_pid_proc
                          << ", pid from vDSO is " << child_pid_vdso << std::endl;
            } else {
                std::cout << "Child PID matches: " << child_pid_proc << std::endl;
            }
            exit(0);
        } else {
            int status;
            waitpid(pid, &status, 0);
        }
    }
   
    pid_t parent_pid_vdso = get_task_pid();
    std::ifstream parent_stat_file("/proc/self/stat");
    if (!parent_stat_file.is_open()) {
        std::cerr << "Failed to open /proc/self/stat" << std::endl;
        return 1;
    }
    pid_t parent_pid_proc;
    parent_stat_file >> parent_pid_proc;
    parent_stat_file.close();
    if (parent_pid_proc != parent_pid_vdso) {
        std::cerr << "Parent PID mismatch: pid from /proc/self/stat is " << parent_pid_proc
                  << ", pid from vDSO is " << parent_pid_vdso << std::endl;
    } else {
        std::cout << "Parent PID matches: " << parent_pid_proc << std::endl;
    }
    return 0;
}