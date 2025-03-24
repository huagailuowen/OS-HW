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
SYSCALL_DEFINE2(write_kv, int, k, int, v) {
    struct hlist_head *bucket;
    struct kv_pair *kv, *found = NULL;
    spinlock_t *lock;
    
    bucket = get_hash_bucket(k);
    lock = get_bucket_lock(k);
    
    spin_lock(lock);
    
    hlist_for_each_entry(kv, bucket, node) {
        if (kv->key == k) {
            found = kv;
            break;
        }
    }
    
    if (found) {
        found->value = v;
    } else {
        kv = kmalloc(sizeof(struct kv_pair), GFP_KERNEL);
        if (!kv) {
            spin_unlock(lock);
            return -1; 
        }
        
        kv->key = k;
        kv->value = v;
        hlist_add_head(&kv->node, bucket);
    }
    
    spin_unlock(lock);
    return sizeof(int); 
}

SYSCALL_DEFINE1(read_kv, int, k) {
    struct hlist_head *bucket;
    struct kv_pair *kv;
    spinlock_t *lock;
    int value = -1;
    
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