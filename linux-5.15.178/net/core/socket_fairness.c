#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/net.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "socket_fairness.h"
/* 全局公平性管理器实例 */
struct socket_fairness_manager socket_fairness_mgr;
bool is_initialized = false;         /* 是否已初始化 */

/* 初始化套接字公平性管理器 */
int socket_fairness_init(void)
{
    if(is_initialized)
        return 0;  /* 如果已初始化，则返回 */
    spin_lock_init(&socket_fairness_mgr.lock);
    INIT_LIST_HEAD(&socket_fairness_mgr.threads);
    
    /* 默认配置值 */
    socket_fairness_mgr.max_sockets_per_thread = 10;
    socket_fairness_mgr.max_pending_per_thread = 5;
    socket_fairness_mgr.throttle_threshold = 1000;
    socket_fairness_mgr.fairness_enabled = true;
    
    printk(KERN_INFO "Socket fairness management initialized\n");
    is_initialized = true;  /* 设置初始化标志 */
    return 0;
}

/* 清理函数 */
void socket_fairness_exit(void)
{
    struct thread_socket_stats *stats, *tmp;
    
    spin_lock(&socket_fairness_mgr.lock);
    list_for_each_entry_safe(stats, tmp, &socket_fairness_mgr.threads, list) {
        list_del(&stats->list);
        kfree(stats);
    }
    spin_unlock(&socket_fairness_mgr.lock);
    
    printk(KERN_INFO "Socket fairness management cleaned up\n");
}

/* 获取线程的套接字统计信息 */
struct thread_socket_stats *get_thread_socket_stats(pid_t tid, bool create)
{
    struct thread_socket_stats *stats = NULL;
    if(!is_initialized)
        socket_fairness_init();  /* 确保已初始化 */
    spin_lock(&socket_fairness_mgr.lock);
    
    list_for_each_entry(stats, &socket_fairness_mgr.threads, list) {
        if (stats->tid == tid) {
            spin_unlock(&socket_fairness_mgr.lock);
            return stats;
        }
    }
    
    if (create) {
        stats = kmalloc(sizeof(struct thread_socket_stats), GFP_KERNEL);
        if (stats) {
            stats->tid = tid;
            atomic_set(&stats->active_sockets, 0);
            atomic_set(&stats->total_bytes_sent, 0);
            atomic_set(&stats->total_bytes_received, 0);
            atomic_set(&stats->pending_requests, 0);
            atomic_set(&stats->throttled_count, 0);
            stats->last_throttled = 0;
            
            list_add(&stats->list, &socket_fairness_mgr.threads);
        }
    }
    
    spin_unlock(&socket_fairness_mgr.lock);
    return stats;
}

/* 检查线程是否允许分配新的套接字 */
bool check_socket_allocation_allowed(pid_t tid)
{
    struct thread_socket_stats *stats;
    bool allowed = true;
    
    if (!socket_fairness_mgr.fairness_enabled)
        return true;
    
    stats = get_thread_socket_stats(tid, true);
    if (!stats)
        return true;  /* 如果无法跟踪，则允许 */
    
    // printk(KERN_INFO "Check Allow\n");
    // return true;  /* 默认允许 */
    /* 检查是否超过每线程套接字限制 */
    if (atomic_read(&stats->active_sockets) >= socket_fairness_mgr.max_sockets_per_thread ||
        atomic_read(&stats->pending_requests) >= socket_fairness_mgr.max_pending_per_thread) {
        
        atomic_inc(&stats->throttled_count);
        stats->last_throttled = jiffies;
        allowed = false;
    }
    printk(KERN_INFO "Check Allow=%d\n", allowed);
    return allowed;
}

/* 更新线程的套接字流量统计 */
void update_socket_traffic(pid_t tid, size_t sent, size_t received)
{
    // printk(KERN_INFO "CheckP1\n");
    struct thread_socket_stats *stats = get_thread_socket_stats(tid, true);
    if (!stats)
        return;
    // printk(KERN_INFO "CheckP2\n");
    
    atomic_add(sent, &stats->total_bytes_sent);
    atomic_add(received, &stats->total_bytes_received);
}

/* 为线程注册新套接字 */
void register_socket_for_thread(pid_t tid)
{
    struct thread_socket_stats *stats = get_thread_socket_stats(tid, true);
    if (!stats) {
        printk(KERN_ERR "Failed to get or create stats for thread %d\n", tid);
        return;  // 如果获取失败，则返回
    }
    
    /* 增加 active_sockets */
    atomic_inc(&stats->active_sockets);
    printk(KERN_INFO "Registered socket for thread %d. Active sockets: %d\n", tid, atomic_read(&stats->active_sockets));
}

/* 为线程注销套接字 */
void unregister_socket_for_thread(pid_t tid)
{
    struct thread_socket_stats *stats = get_thread_socket_stats(tid, false);
    if (!stats) {
        printk(KERN_ERR "Failed to get stats for thread %d\n", tid);
        return;  // 如果获取失败，则返回
    }

    /* 减少 active_sockets */
    if (atomic_read(&stats->active_sockets) > 0) {
        atomic_dec(&stats->active_sockets);
        printk(KERN_INFO "Unregistered socket for thread %d. Active sockets: %d\n", tid, atomic_read(&stats->active_sockets));
    } else {
        printk(KERN_WARNING "Thread %d has no active sockets to unregister\n", tid);
    }
}


static int socket_fairness_proc_show(struct seq_file *m, void *v)
{
    struct thread_socket_stats *stats;
    
    spin_lock(&socket_fairness_mgr.lock);
    list_for_each_entry(stats, &socket_fairness_mgr.threads, list) {
        seq_printf(m, "Thread PID: %d\n", stats->tid);
        seq_printf(m, "Active Sockets: %d\n", atomic_read(&stats->active_sockets));
        seq_printf(m, "Pending Requests: %d\n", atomic_read(&stats->pending_requests));
        seq_printf(m, "Total Bytes Sent: %d\n", atomic_read(&stats->total_bytes_sent));
        seq_printf(m, "Total Bytes Received: %d\n", atomic_read(&stats->total_bytes_received));
        seq_printf(m, "Throttled Count: %d\n", atomic_read(&stats->throttled_count));
        seq_printf(m, "Last Throttled: %lu\n", stats->last_throttled);
        seq_putc(m, '\n');
    }
    spin_unlock(&socket_fairness_mgr.lock);

    return 0;
}

static int socket_fairness_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, socket_fairness_proc_show, NULL);
}

static const struct proc_ops socket_fairness_proc_fops = {
    .proc_open    = socket_fairness_proc_open,
    .proc_read    = seq_read,
    .proc_release = single_release,
};

static int __init socket_fairness_module_init(void)
{
    struct proc_dir_entry *entry;

    entry = proc_create("socket_fairness", 0, NULL, &socket_fairness_proc_fops);
    if (!entry) {
        printk(KERN_ERR "Unable to create /proc/socket_fairness\n");
        return -ENOMEM;
    }

    printk(KERN_INFO "Created /proc/socket_fairness\n");

    socket_fairness_init(); // 初始化全局结构体
    return 0;
}

static void __exit socket_fairness_module_exit(void)
{
    remove_proc_entry("socket_fairness", NULL);
    printk(KERN_INFO "Removed /proc/socket_fairness\n");

    socket_fairness_exit(); // 清理结构体资源
}

module_init(socket_fairness_module_init);
module_exit(socket_fairness_module_exit);