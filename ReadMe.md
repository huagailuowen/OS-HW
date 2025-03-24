## Qemu Scripts

```
# 创建磁盘镜像 (例如100MB)
dd if=/dev/zero of=rootfs.img bs=1M count=100

# 创建 ext4 文件系统
mkfs.ext4 rootfs.img

# 挂载文件系统
mkdir -p /tmp/rootfs
sudo mount -o loop rootfs.img /tmp/rootfs