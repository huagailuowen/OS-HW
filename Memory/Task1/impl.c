#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

/**
 * @brief 重新映射一块虚拟内存区域
 * @param addr 原始映射的内存地址，如果为 NULL 则由系统自动选择一个合适的地址
 * @param size 需要映射的大小（单位：字节）
 * @return 成功返回映射的地址，失败返回 NULL
 * @details 该函数用于重新映射一个新的虚拟内存区域。如果 addr 参数为 NULL，
 *          系统会自动选择一个合适的地址进行映射。映射的内存区域大小为 size 字节。
 *          映射失败时返回 NULL。
 */
void* mmap_remap(void *addr, size_t size) {
    // TODO: TASK1
    void* new_addr = mmap(
        NULL,                   // 让系统选择新地址
        size,                   // 保持相同大小
        PROT_READ | PROT_WRITE, // 读写权限
        MAP_PRIVATE | MAP_ANONYMOUS, // 私有匿名映射
        -1,                     // 匿名映射时文件描述符为-1
        0                       // 匿名映射时偏移量为0
    );
    
    if (new_addr == MAP_FAILED) {
        return NULL;
    }
    
    if (addr != NULL) {
        memcpy(new_addr, addr, size);
    }
    
    return new_addr; // 需要返回映射的地址
}

/**
 * @brief 使用 mmap 进行文件读写
 * @param filename 待操作的文件路径
 * @param offset 写入文件的偏移量（单位：字节）
 * @param content 要写入文件的内容
 * @return 成功返回 0，失败返回 -1
 * @details 该函数使用内存映射（mmap）的方式进行文件写入操作。
 *          通过 filename 指定要写入的文件，
 *          offset 指定写入的起始位置，
 *          content 指定要写入的内容。
 *          写入成功返回 0，失败返回 -1。
 */
int file_mmap_write(const char* filename, size_t offset, char* content) {
    // TODO: TASK2
    int fd;
    struct stat file_stat;
    void* map_addr;
    size_t content_len = strlen(content);
    size_t tot_size = offset + content_len;
    
    fd = open(filename, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        return -1;
    }
    
    if (fstat(fd, &file_stat) == -1) {
        close(fd);
        return -1;
    }
    
    if (file_stat.st_size < tot_size) {
        if (ftruncate(fd, tot_size) == -1) {
            close(fd);
            return -1;
        }
    }
    

    map_addr = mmap(NULL, tot_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (map_addr == MAP_FAILED) {
        close(fd);
        return -1;
    }

    memcpy((char*)map_addr + offset, content, content_len);
    

    if (msync(map_addr, tot_size, MS_SYNC) == -1) {
        munmap(map_addr, tot_size);
        close(fd);
        return -1;
    }
    
    if (munmap(map_addr, tot_size) == -1) {
        close(fd);
        return -1;
    }
    close(fd);
    
    return 0; 
    // 需要返回正确的执行状态
}