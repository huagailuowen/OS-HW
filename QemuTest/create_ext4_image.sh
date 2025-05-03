#!/bin/bash
# filepath: /home/luowen/syshw/QemuTest/create_ext4_image.sh

# 设置变量
# cp -r ../../FS/Task1/test_read extracted/testfiles


SOURCE_DIR="./initramfs_work/extracted"
IMAGE_SIZE="512M"  # 设置合适的大小，可以根据内容调整
IMAGE_FILE="rootfs.img"

echo "===== 创建 ext4 镜像文件 ====="

# 创建空白镜像文件
dd if=/dev/zero of=$IMAGE_FILE bs=1M count=${IMAGE_SIZE%M} status=progress
echo "空白镜像文件创建完成"

# 格式化为 ext4
mkfs.ext4 $IMAGE_FILE
echo "镜像格式化为 ext4 文件系统"

# 创建挂载点
MOUNT_POINT=$(mktemp -d)
echo "创建临时挂载点: $MOUNT_POINT"

# 挂载镜像
sudo mount -o loop $IMAGE_FILE $MOUNT_POINT
echo "镜像已挂载到 $MOUNT_POINT"

# 复制文件
sudo cp -a $SOURCE_DIR/* $MOUNT_POINT/
echo "文件已复制到挂载的镜像中"

# 设置正确的权限
sudo chown -R root:root $MOUNT_POINT
echo "设置了正确的所有权"

# 卸载镜像
sudo umount $MOUNT_POINT
echo "镜像已卸载"

# 删除挂载点
rmdir $MOUNT_POINT
echo "临时挂载点已删除"

echo "===== ext4 镜像文件创建完成 ====="
echo "镜像文件路径: $IMAGE_FILE"