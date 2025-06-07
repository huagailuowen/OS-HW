#!/bin/sh

# 创建ramfs挂载点
mkdir -p /mnt/ramfs

# 挂载ramfs
mount -t ramfs -o size=10M none /mnt/ramfs

export backup_dir="/var/backup/ramfs"
# 在ramfs中创建测试文件

echo "这是ramfs测试文件内容" > /tmp/source_content.txt
dd if=/tmp/source_content.txt of=/mnt/ramfs/test.txt conv=fsync 2>/dev/null
cat $backup_dir/test.txt
# 或者使用Python等语言显式调用fsync
# python3 -c '
# import os
# f = open("/mnt/ramfs/test.txt", "a")
# os.fsync(f.fileno())
# f.close()
# print("fsync调用完成")
# '


echo "这是另一个ramfs测试文件内容" > /tmp/source_content.txt
dd if=/tmp/source_content.txt of=/mnt/ramfs/test2.txt conv=fsync 2>/dev/null
cat $backup_dir/test2.txt
# 对第二个文件也调用fsync
# python3 -c '
# import os
# f = open("/mnt/ramfs/test2.txt", "a")
# os.fsync(f.fileno())
# f.close()
# print("fsync调用完成")
# '

# 检查备份目录
echo "检查备份目录..."
ls -la /var/backup/ 2>/dev/null || echo "备份目录不存在"
ls -la /var/backup/ramfs/ 2>/dev/null || echo "ramfs备份目录不存在"