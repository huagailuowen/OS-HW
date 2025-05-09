#!/bin/bash

# 创建目录
mkdir -p rootfs
# 创建磁盘镜像(1GB)
dd if=/dev/zero of=rootfs.img bs=1M count=1024
# 格式化磁盘
mkfs.ext3 rootfs.img
# 挂载文件系统
sudo mount rootfs.img rootfs
# BusyBox 
sudo mkdir -p rootfs/{bin,sbin,etc,proc,sys,dev,usr/{bin,sbin}}
sudo cp /bin/busybox rootfs/bin
cd rootfs
sudo ln -s bin/busybox init
echo "FUCK"
for prog in $(./bin/busybox --list); do
    sudo ln -s bin/busybox bin/$prog;
done
cd ..

sudo mkdir -p rootfs/etc

sudo bash -c 'cat > rootfs/etc/inittab << EOF
::sysinit:/etc/init.d/rcS
::respawn:-/bin/sh
::restart:/sbin/init
::ctrlaltdel:/sbin/reboot
EOF'

# 创建初始化脚本
sudo mkdir -p rootfs/etc/init.d
sudo bash -c 'cat > rootfs/etc/init.d/rcS << EOF
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
echo -e "\033[32mStarting KV test...\033[0m"
/run_test.sh
echo -e "\033[33mTest completed, you can now shutdown the VM\033[0m"
EOF'
sudo chmod +x rootfs/etc/init.d/rcS