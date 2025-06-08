#ifndef _THREAD_SOCKET_FAIRNESS_H
#define _THREAD_SOCKET_FAIRNESS_H

#include <linux/types.h>
#include <linux/list.h>
#include <linux/spinlock.h>

/* 每个线程的套接字使用统计 */
struct thread_socket_stats {
    struct list_head list;          /* 链表节点 */
    pid_t tid;                      /* 线程ID */
    atomic_t active_sockets;        /* 活跃套接字数量 */
    atomic_t total_bytes_sent;      /* 已发送总字节数 */
    atomic_t total_bytes_received;  /* 已接收总字节数 */
    atomic_t pending_requests;      /* 挂起的请求数 */
    atomic_t throttled_count;       /* 被限制次数 */
    unsigned long last_throttled;   /* 上次被限制的时间戳 */
};

/* 全局管理结构 */
struct socket_fairness_manager {
    spinlock_t lock;                /* 保护链表的锁 */
    struct list_head threads;       /* 线程统计链表 */
    unsigned int max_sockets_per_thread;      /* 每线程最大套接字数 */
    unsigned int max_pending_per_thread;      /* 每线程最大挂起请求数 */
    unsigned int throttle_threshold;          /* 限制阈值 */
    bool fairness_enabled;          /* 是否启用公平性管理 */
};

extern struct socket_fairness_manager socket_fairness_mgr;

/* 函数原型 */
int socket_fairness_init(void);
void socket_fairness_exit(void);
struct thread_socket_stats *get_thread_socket_stats(pid_t tid, bool create);
bool check_socket_allocation_allowed(pid_t tid);
bool should_throttle_transmission(pid_t tid);
void update_socket_traffic(pid_t tid, size_t sent, size_t received);
void register_socket_for_thread(pid_t tid);
void unregister_socket_for_thread(pid_t tid);

#endif /* _THREAD_SOCKET_FAIRNESS_H */