#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
void* mmap_remap(void *addr, size_t size) ;
int file_mmap_write(const char* filename, size_t offset, char* content) ;