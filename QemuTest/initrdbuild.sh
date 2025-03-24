
# mkdir -p ./initramfs_work
# cp initramfs.cpio.gz ./initramfs_work
cd ./initramfs_work
# gunzip -k initramfs.cpio.gz
# mkdir -p extracted
# cd extracted
# cat ../initramfs.cpio | cpio -idmv
# cd ..
cp -r ../../Syscall/kv_call/* extracted/testfiles

# 回到解压目录
cd extracted

# 重新创建cpio归档
find . -print0 | cpio --null -ov --format=newc > ../new_initramfs.cpio

# 返回工作目录并压缩
cd ..
gzip -c new_initramfs.cpio > new_initramfs.cpio.gz
