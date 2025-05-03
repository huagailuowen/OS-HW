#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

// Function declarations from read.c
void print_inode_info(const char *filepath);
void set_file_xattr(const char *filepath, const char *key, const char *value);
void get_file_xattr(const char *filepath, const char *key);