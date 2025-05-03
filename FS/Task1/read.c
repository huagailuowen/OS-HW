#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "read.h"
void print_inode_info(const char *filepath) {
    struct stat st;
    if (stat(filepath, &st) == 0) {
        printf("Inode number: %lu\n", st.st_ino);
        printf("File mode: %o\n", st.st_mode);
        printf("Owner UID: %u\n", st.st_uid);
        printf("Owner GID: %u\n", st.st_gid);
        printf("File size: %ld bytes\n", st.st_size);
        printf("Last access: %ld\n", st.st_atime);
        printf("Last modify: %ld\n", st.st_mtime);
    } else {
        perror("stat failed");
    }
}
void set_file_xattr(const char *filepath, const char *key, const char *value) {
    if (setxattr(filepath, key, value, strlen(value), 0) == -1) {
        perror("setxattr failed");
    } else {
        printf("Set xattr %s = %s\n", key, value);
    }
}

void get_file_xattr(const char *filepath, const char *key) {
    char buf[256];
    ssize_t len = getxattr(filepath, key, buf, sizeof(buf));
    if (len == -1) {
        perror("getxattr failed");
    } else {
        buf[len] = '\0'; // 确保字符串结束
        printf("Got xattr %s = %s\n", key, buf);
    }
}