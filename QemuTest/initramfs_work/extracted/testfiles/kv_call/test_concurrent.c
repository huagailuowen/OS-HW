#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

#define SYS_write_kv 449
#define SYS_read_kv  450

// 共享键值
#define SHARED_KEY 42

// 系统调用包装函数
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
    if (ret < 0) {
        errno = -ret;
        return -1;
    }
    return ret;
}

// 同步变量
int test_value = 0;
pthread_mutex_t test_mutex = PTHREAD_MUTEX_INITIALIZER;
int writer_done = 0;
int reader_success = 0;

// 写线程
void *writer_thread(void *arg) {
    int key = SHARED_KEY;
    int value = 12345;
    int ret;

    printf("Writer: 开始写入 key=%d, value=%d\n", key, value);
    
    ret = write_kv(key, value);
    
    if (ret < 0) {
        printf("Writer: 写入失败，错误: %s\n", strerror(errno));
    } else {
        printf("Writer: 写入成功，返回值: %d\n", ret);
        
        // 更新线程间共享的测试值
        pthread_mutex_lock(&test_mutex);
        test_value = value;
        writer_done = 1;
        pthread_mutex_unlock(&test_mutex);
    }
    
    // 等待一会，让读线程有时间执行
    sleep(1);
    return NULL;
}

// 读线程
void *reader_thread(void *arg) {
    int key = SHARED_KEY;
    int value, expected;
    int ret;

    // 等待写线程完成
    while (1) {
        pthread_mutex_lock(&test_mutex);
        if (writer_done) {
            expected = test_value;
            pthread_mutex_unlock(&test_mutex);
            break;
        }
        pthread_mutex_unlock(&test_mutex);
        usleep(10000); // 10毫秒
    }
    
    printf("Reader: 开始读取 key=%d\n", key);
    
    ret = read_kv(key);
    
    if (ret < 0) {
        printf("Reader: 读取失败，错误: %s\n", strerror(errno));
    } else {
        printf("Reader: 读取成功，key=%d, value=%d\n", key, ret);
        
        if (ret == expected) {
            printf("Reader: 测试通过！读取的值与写入的值匹配\n");
            reader_success = 1;
        } else {
            printf("Reader: 测试失败！期望值=%d，实际值=%d\n", expected, ret);
        }
    }
    
    return NULL;
}

int main() {
    pthread_t writer, reader;
    
    printf("=== 简化版KV存储线程共享测试 ===\n");
    
    // 首先尝试清除已有的键（如果存在）
    read_kv(SHARED_KEY);
    
    // 创建线程
    if (pthread_create(&writer, NULL, writer_thread, NULL) != 0) {
        perror("创建写线程失败");
        return 1;
    }
    
    if (pthread_create(&reader, NULL, reader_thread, NULL) != 0) {
        perror("创建读线程失败");
        return 1;
    }
    
    // 等待线程完成
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);
    
    // 测试结论
    printf("\n=== 测试结果 ===\n");
    if (reader_success) {
        printf("测试通过！多线程共享KV存储正常工作\n");
        return 0;
    } else {
        printf("测试失败！多线程之间无法共享KV存储\n");
        return 1;
    }
}