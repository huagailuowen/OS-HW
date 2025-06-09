#define KV_STORE_IMPL
#include <linux/kv_store.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/slab.h>

static inline struct task_struct *get_process_task(void) {
    // 返回主线程
    return current->group_leader;
}
static inline struct hlist_head *get_hash_bucket(int key) {
    int hash = kv_hash(key);
    struct task_struct *process_task = get_process_task();
    return &process_task->kv_data.kv_store[hash];
}

static inline spinlock_t *get_bucket_lock(int key) {
    int hash = kv_hash(key);
    struct task_struct *process_task = get_process_task();
    return &process_task->kv_data.kv_locks[hash];
}
// 内核内部KV操作函数
long write_kv(int k, int v) {
    struct hlist_head *bucket;
    struct kv_pair *kv, *found = NULL;
    struct kv_pair *new_kv = NULL;
    spinlock_t *lock;
    
    bucket = get_hash_bucket(k);
    lock = get_bucket_lock(k);
    
    // 预先分配内存，避免在持有锁时分配
    new_kv = kmalloc(sizeof(struct kv_pair), GFP_ATOMIC);
    if (!new_kv) {
        return -ENOMEM;
    }
    
    spin_lock(lock);
    
    // 查找是否已存在该键
    hlist_for_each_entry(kv, bucket, node) {
        if (kv->key == k) {
            found = kv;
            break;
        }
    }
    
    if (found) {
        // 更新现有值
        found->value = v;
        spin_unlock(lock);
        
        // 释放未使用的内存
        kfree(new_kv);
    } else {
        // 添加新的键值对
        new_kv->key = k;
        new_kv->value = v;
        hlist_add_head(&new_kv->node, bucket);
        spin_unlock(lock);
    }
    
    return 0;
}

long read_kv(int k) {
    struct hlist_head *bucket;
    struct kv_pair *kv;
    spinlock_t *lock;
    long value = -ENOENT; // 使用标准错误码
    
    bucket = get_hash_bucket(k);
    lock = get_bucket_lock(k);
    
    spin_lock(lock);
    
    // 查找键
    hlist_for_each_entry(kv, bucket, node) {
        if (kv->key == k) {
            value = kv->value;
            break;
        }
    }
    
    spin_unlock(lock);
    return value;
}

// 系统调用实现
SYSCALL_DEFINE2(write_kv, int, k, int, v) {
    return write_kv(k, v);
}

SYSCALL_DEFINE1(read_kv, int, k) {
    return read_kv(k);
}


int kv_store_init(struct task_struct *task) {
    int i;
    if (task->pid != task->tgid) {
        return 0;
    }
    for (i = 0; i < 1024; i++) {
        INIT_HLIST_HEAD(&task->kv_data.kv_store[i]);
        spin_lock_init(&task->kv_data.kv_locks[i]);
    }
    
    return 0;
}

void kv_store_exit(struct task_struct *task) {
    int i;
    struct hlist_head *bucket;
    struct kv_pair *kv;
    struct hlist_node *tmp;
    if (task->pid != task->tgid) {
        return;
    }
    for (i = 0; i < 1024; i++) {
        bucket = &task->kv_data.kv_store[i];
        
        spin_lock(&task->kv_data.kv_locks[i]);
        hlist_for_each_entry_safe(kv, tmp, bucket, node) {
            hlist_del(&kv->node);
            kfree(kv);
        }
        spin_unlock(&task->kv_data.kv_locks[i]);
    }
}

EXPORT_SYMBOL(write_kv);
EXPORT_SYMBOL(read_kv);
EXPORT_SYMBOL(kv_store_init);
EXPORT_SYMBOL(kv_store_exit);