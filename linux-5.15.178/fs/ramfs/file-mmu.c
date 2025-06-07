/* file-mmu.c: ramfs MMU-based file operations
 *
 * Resizable simple ram filesystem for Linux.
 *
 * Copyright (C) 2000 Linus Torvalds.
 *               2000 Transmeta Corp.
 *
 * Usage limits added by David Gibson, Linuxcare Australia.
 * This file is released under the GPL.
 */

/*
 * NOTE! This filesystem is probably most useful
 * not as a real filesystem, but as an example of
 * how virtual filesystems can be written.
 *
 * It doesn't get much simpler than this. Consider
 * that this file implements the full semantics of
 * a POSIX-compliant read-write filesystem.
 *
 * Note in particular how the filesystem does not
 * need to implement any data structures of its own
 * to keep track of the virtual data: using the VFS
 * caches is sufficient.
 */

#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/ramfs.h>
#include <linux/sched.h>
#include <linux/kmod.h>    /* 添加此头文件用于call_usermodehelper */
#include <linux/path.h>    /* 添加此头文件用于dentry和path操作 */

#include "internal.h"

static unsigned long ramfs_mmu_get_unmapped_area(struct file *file,
        unsigned long addr, unsigned long len, unsigned long pgoff,
        unsigned long flags)
{
    return current->mm->get_unmapped_area(file, addr, len, pgoff, flags);
}

/* 自定义fsync实现，将文件复制到备份目录 */
static int ramfs_backup_fsync(struct file *file, loff_t start, loff_t end, int datasync)
{
	printk(KERN_INFO "ramfs: 执行文件同步操作，开始备份文件\n");
    char *argv[5], *envp[3];
    char *mkdir_argv[4];
    int ret;
    char *src_path = NULL;
    char *path_ptr;  /* 用于保存d_path返回的实际路径指针 */
    char *backup_dir = "/var/backup/ramfs";  /* 备份目录 */
    
    /* 构建源文件路径 */
    src_path = kmalloc(PATH_MAX, GFP_KERNEL);
    if (!src_path)
        return -ENOMEM;
    
    /* 获取文件路径 */
    path_ptr = d_path(&file->f_path, src_path, PATH_MAX);
    if (IS_ERR(path_ptr)) {
        ret = PTR_ERR(path_ptr);
        printk(KERN_ERR "ramfs: 获取文件路径失败，错误码 %d\n", ret);
        kfree(src_path);
        return ret;
    }
    
    /* 首先创建备份目录（如果不存在） */
    mkdir_argv[0] = "/bin/mkdir";
    mkdir_argv[1] = "-p";  /* 创建所有需要的父目录 */
    mkdir_argv[2] = backup_dir;
    mkdir_argv[3] = NULL;
    
    /* 设置环境变量 */
    envp[0] = "HOME=/";
    envp[1] = "PATH=/sbin:/bin:/usr/sbin:/usr/bin";
    envp[2] = NULL;
    
    /* 尝试创建目录 */
    ret = call_usermodehelper(mkdir_argv[0], mkdir_argv, envp, UMH_WAIT_PROC);
    if (ret != 0) {
        printk(KERN_ERR "ramfs: 无法创建备份目录 %s, 错误码 %d\n", backup_dir, ret);
        kfree(src_path);
        return ret;
    }
    
    /* 设置复制命令参数 */
    argv[0] = "/bin/cp";
    argv[1] = "-f";    /* 强制覆盖 */
    argv[2] = path_ptr;  /* 使用d_path返回的正确路径指针 */
    argv[3] = backup_dir;
    argv[4] = NULL;
    
    /* 调用用户空间命令执行复制 */
    ret = call_usermodehelper(argv[0], argv, envp, UMH_WAIT_PROC);
    if (ret != 0) {
        printk(KERN_ERR "ramfs: 无法复制文件 %s 到 %s, 错误码 %d\n", 
               path_ptr, backup_dir, ret);
    }
    
    kfree(src_path);
    
    /* 返回标准fsync结果 */
    return ret ? ret : 0;
}

const struct file_operations ramfs_file_operations = {
    .read_iter	= generic_file_read_iter,
    .write_iter	= generic_file_write_iter,
    .mmap		= generic_file_mmap,
    .fsync		= ramfs_backup_fsync,  /* 替换为我们自定义的fsync */
    .splice_read	= generic_file_splice_read,
    .splice_write	= iter_file_splice_write,
    .llseek		= generic_file_llseek,
    .get_unmapped_area	= ramfs_mmu_get_unmapped_area,
};

const struct inode_operations ramfs_file_inode_operations = {
    .setattr	= simple_setattr,
    .getattr	= simple_getattr,
};