#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define SYS_write_kv 449
#define SYS_read_kv  450

#define NUM_THREADS 5
#define NUM_OPERATIONS 100
#define SHARED_KEY 42
#define NUM_KEYS 10

// 互斥锁，用于保护共享状态
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
// 测试统计
int total_reads = 0;
int total_writes = 0;
int successful_reads = 0;
int failed_reads = 0;
bool test_passed = true;
bool reader_passed = false;

int write_kv(int k, int v) {
    long ret = syscall(SYS_write_kv, k, v);
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    return ret;
}

int read_kv(int k) {
    long ret = syscall(SYS_read_kv, k);
    if (ret < 0 && ret != -ENOENT) {
        errno = -ret;
        return -1;
    }
    return ret;
}

// 需要将这些函数移到外部
void *writer_func(void *arg) {
    int new_value = 12345;
    printf("Writer thread: writing key=%d, value=%d\n", SHARED_KEY, new_value);
    if (write_kv(SHARED_KEY, new_value) < 0) {
        printf("Writer thread: write failed: %s\n", strerror(errno));
        return NULL;
    }
    sleep(1); // 给读取线程时间
    return NULL;
}

void *reader_func(void *arg) {
    // 等待写入线程更新值
    usleep(500000); // 500ms
    
    printf("Reader thread: reading key=%d\n", SHARED_KEY);
    int value = read_kv(SHARED_KEY);
    if (value < 0) {
        printf("Reader thread: read failed: %s\n", strerror(errno));
        return NULL;
    }
    
    printf("Reader thread: read value=%d\n", value);
    if (value == 12345) {
        printf("Reader thread: TEST PASSED! Read correct value.\n");
        reader_passed = true;
    } else {
        printf("Reader thread: TEST FAILED! Expected 12345, got %d\n", value);
    }
    return NULL;
}

typedef struct {
    int thread_id;
    int last_value; // 最后写入的值
} thread_data_t;

// 线程函数
void* thread_func(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int thread_id = data->thread_id;
    int value, ret;
    
    printf("Thread %d started\n", thread_id);
    
    srand(time(NULL) + thread_id); // 确保每个线程有不同的随机序列
    
    // 执行一系列随机读写操作
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        // 决定是读还是写
        bool is_write = (rand() % 2 == 0);
        // 决定使用哪个键（偶尔使用共享键，其余时间使用线程特定键）
        int key = (rand() % 5 == 0) ? SHARED_KEY : (thread_id * 100 + (rand() % NUM_KEYS));
        
        if (is_write) {
            value = rand() % 10000;
            ret = write_kv(key, value);
            
            pthread_mutex_lock(&lock);
            total_writes++;
            pthread_mutex_unlock(&lock);
            
            if (ret < 0) {
                printf("Thread %d: write_kv(%d, %d) failed: %s\n", 
                       thread_id, key, value, strerror(errno));
                test_passed = false;
            } else {
                if (key == SHARED_KEY) {
                    // 更新线程记录的共享键最后值
                    data->last_value = value;
                    printf("Thread %d: wrote shared key=%d, value=%d\n", 
                           thread_id, key, value);
                }
            }
        } else {
            ret = read_kv(key);
            
            pthread_mutex_lock(&lock);
            total_reads++;
            if (ret >= 0) successful_reads++;
            else failed_reads++;
            pthread_mutex_unlock(&lock);
            
            if (ret < 0 && ret != -ENOENT) {
                printf("Thread %d: read_kv(%d) error: %s\n", 
                       thread_id, key, strerror(errno));
                test_passed = false;
            } else if (ret >= 0 && key == SHARED_KEY) {
                printf("Thread %d: read shared key=%d, value=%d\n", 
                       thread_id, key, ret);
            }
        }
        
        // 随机休眠一小段时间，模拟真实工作负载
        usleep(rand() % 1000);
    }
    
    printf("Thread %d completed all operations\n", thread_id);
    return NULL;
}

// 验证共享关键字测试
void shared_key_test() {
    printf("\n=== Running Shared Key Test ===\n");
    
    // 初始化共享键
    int initial_value = 9999;
    if (write_kv(SHARED_KEY, initial_value) < 0) {
        printf("Failed to initialize shared key: %s\n", strerror(errno));
        return;
    }
    printf("Initialized shared key %d with value %d\n", SHARED_KEY, initial_value);
    
    // 从一个线程写入，从另一个线程读取
    pthread_t writer, reader;
    int writer_id = 100, reader_id = 200;
    reader_passed = false;
    
    // 创建和执行线程
    pthread_create(&writer, NULL, writer_func, NULL);
    pthread_create(&reader, NULL, reader_func, NULL);
    
    // 等待线程完成
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);
    
    printf("Shared key test %s!\n", reader_passed ? "PASSED" : "FAILED");
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    thread_data_t thread_data[NUM_THREADS];
    
    printf("=== KV Store Concurrent Access Test ===\n");
    printf("Testing with %d threads, %d operations per thread\n", 
           NUM_THREADS, NUM_OPERATIONS);
    
    // 首先运行特定的共享键测试
    shared_key_test();
    
    // 创建多个线程并发访问KV存储
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i + 1;
        thread_data[i].last_value = -1;
        
        if (pthread_create(&threads[i], NULL, thread_func, &thread_data[i]) != 0) {
            perror("Failed to create thread");
            return 1;
        }
    }
    
    // 等待所有线程完成
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // 输出统计信息
    printf("\n=== Test Results ===\n");
    printf("Total read operations: %d (successful: %d, failed: %d)\n", 
           total_reads, successful_reads, failed_reads);
    printf("Total write operations: %d\n", total_writes);
    
    // 进行最终验证：读取共享键值并检查是否与某个线程最后写入的值匹配
    int final_value = read_kv(SHARED_KEY);
    bool found_match = false;
    
    if (final_value >= 0) {
        printf("Final value of shared key %d: %d\n", SHARED_KEY, final_value);
        
        for (int i = 0; i < NUM_THREADS; i++) {
            if (thread_data[i].last_value == final_value) {
                printf("Value matches last write from thread %d\n", thread_data[i].thread_id);
                found_match = true;
                break;
            }
        }
        
        if (!found_match) {
            printf("Warning: Final value doesn't match any thread's last write!\n");
            test_passed = false;
        }
    } else {
        printf("Error reading final value of shared key: %s\n", strerror(errno));
        test_passed = false;
    }
    
    printf("\nOverall test %s!\n", test_passed ? "PASSED" : "FAILED");
    
    return test_passed ? 0 : 1;
}