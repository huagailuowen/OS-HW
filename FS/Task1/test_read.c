/*
 * usage: use make qemu-ext4 to run the test
 * compile with: gcc -static -o test_read test_read.c read.c -I.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/xattr.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include "read.h"

// Function declarations from read.c
void print_inode_info(const char *filepath);
void set_file_xattr(const char *filepath, const char *key, const char *value);
void get_file_xattr(const char *filepath, const char *key);

// Test function for print_inode_info
void test_inode_info(const char *filepath) {
    printf("\n=== Testing print_inode_info ===\n");
    printf("Displaying inode information for: %s\n", filepath);
    print_inode_info(filepath);
}

// Test function for xattr operations
void test_xattr_operations(const char *filepath) {
    printf("\n=== Testing extended attributes ===\n");
    
    // Test setting an extended attribute
    const char *key = "custom.test_attr";
    const char *value = "test_value_123";
    
    printf("Setting extended attribute...\n");
    set_file_xattr(filepath, key, value);
    
    // Test getting the attribute we just set
    printf("Getting extended attribute...\n");
    get_file_xattr(filepath, key);
    
    // Test getting a non-existent attribute
    printf("Trying to get a non-existent attribute...\n");
    get_file_xattr(filepath, "user.nonexistent");
}

// Create a temporary test file
int create_test_file(const char *filepath, const char *content) {
    int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Failed to create test file");
        return -1;
    }
    
    if (write(fd, content, strlen(content)) == -1) {
        perror("Failed to write to test file");
        close(fd);
        return -1;
    }
    
    close(fd);
    return 0;
}

int main() {
    const char *test_file = "test_file.txt";
    const char *test_content = "This is a test file for inode and xattr operations";
    
    printf("=== Starting tests for file system operations ===\n");
    
    // Create test file
    if (create_test_file(test_file, test_content) != 0) {
        return EXIT_FAILURE;
    }
    
    // Test print_inode_info
    test_inode_info(test_file);
    
    // Test xattr operations
    // Check if filesystem supports extended attributes
    if (setxattr(test_file, "custom.test", "test", 4, 0) == -1) {
        if (errno == ENOTSUP) {
            printf("\nFilesystem does not support extended attributes. Skipping xattr tests.\n");
        } else {
            perror("Error checking for xattr support");
        }
    } else {
        test_xattr_operations(test_file);
    }
    
    // Test with non-existent file
    printf("\n=== Testing with non-existent file ===\n");
    print_inode_info("nonexistent_file.txt");
    
    // Clean up
    if (unlink(test_file) == -1) {
        perror("Failed to remove test file");
    }
    
    printf("\n=== All tests completed ===\n");
    return EXIT_SUCCESS;
}