#!/bin/sh

# 创建ramfs挂载点
mkdir -p /mnt/ramfs

# 在测试脚本中确保使用ramfs
mount -t ramfs -o size=10M none /mnt/ramfs

# 检查挂载类型
mount | grep ramfs
cat /proc/mounts | grep ramfs

# 在ramfs中创建测试文件
echo "这是ramfs测试文件内容" > /mnt/ramfs/test.txt
echo "另一个测试文件" > /mnt/ramfs/test2.txt

# 调用sync触发fsync
echo "开始同步文件..."
sync

# 检查备份目录是否创建
echo "检查备份目录..."
ls -la /var/backup/

# 检查备份文件
echo "检查备份文件..."
ls -la /var/backup/ramfs/

# 显示备份文件内容
echo "备份文件内容："
cat /var/backup/ramfs/test.txt 2>/dev/null || echo "未找到备份文件"
cat /var/backup/ramfs/test2.txt 2>/dev/null || echo "未找到备份文件"

echo "测试完成"