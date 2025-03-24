// test_kv.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include <string.h>

#define SYS_write_kv 449
#define SYS_read_kv  450

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

int main(int argc, char *argv[]) {
    int key, value, ret;
    
    printf("KV Store System Call Tester\n");

    // 写入测试
    key = 42;
    value = 12345;
    printf("Writing: key=%d, value=%d\n", key, value);
    ret = write_kv(key, value);
    if (ret < 0) {
        printf("write_kv failed: %s\n", strerror(errno));
        return 1;
    }
    printf("write_kv returned: %d\n", ret);

    // 读取测试
    printf("Reading key=%d\n", key);
    ret = read_kv(key);
    if (ret < 0) {
        printf("read_kv failed: %s\n", strerror(errno));
        return 1;
    }
    printf("read_kv returned: %d\n", ret);
    
    if (ret == value) {
        printf("Test PASSED!\n");
    } else {
        printf("Test FAILED! Expected %d, got %d\n", value, ret);
        return 1;
    }
    
    // 测试不存在的键
    key = 9999;
    printf("Reading non-existent key=%d\n", key);
    ret = read_kv(key);
    if (ret < 0) {
        printf("read_kv correctly failed for non-existent key\n");
    } else {
        printf("Unexpected success! read_kv returned: %d\n", ret);
    }
    
    return 0;
}