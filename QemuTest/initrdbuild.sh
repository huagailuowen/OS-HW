





# mkdir -p ./initramfs_work
# cp initramfs.cpio.gz ./initramfs_work
cd ./initramfs_work
# gunzip -k initramfs.cpio.gz
# mkdir -p extracted
# cd extracted
# cat ../initramfs.cpio | cpio -idmv
# cd ..
# cp -r ../../FS/Task1/test_read extracted/testfiles
# cp /home/luowen/syshw/Memory/Task2/test.sh extracted/testfiles/5_2
# cp  /home/luowen/syshw/Net/Task1/client extracted/testfiles
# cp  /home/luowen/syshw/Net/Task1/server extracted/testfiles

# copy_required_libraries /home/luowen/syshw/Syscall/Task2/test_vdso_all  ./extracted

# cp -rf /home/luowen/syshw/Syscall/kv_call /home/luowen/syshw/QemuTest/initramfs_work/extracted/testfiles/

# cp /home/luowen/syshw/Syscall/Task2/test_vdso_all extracted/testfiles/

# 回到解压目录
cd extracted

# 重新创建cpio归档
find . -print0 | cpio --null -ov --format=newc > ../new_initramfs.cpio

# 返回工作目录并压缩
cd ..
gzip -c new_initramfs.cpio > new_initramfs.cpio.gz
