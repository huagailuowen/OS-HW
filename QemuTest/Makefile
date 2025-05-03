QEMU_TEST_DIR = $(PWD)
LINUX_DIR = ./../linux-5.15.178

QEMU_NCPU ?= 4

qemu:
	qemu-system-x86_64 \
		-kernel $(LINUX_DIR)/arch/x86/boot/bzImage \
		-initrd ./initramfs_work/new_initramfs.cpio.gz \
		-nographic \
		-m 4G \
		-append "init=/init console=ttyS0"

qemu-ext4:
	qemu-system-x86_64 \
    -kernel $(LINUX_DIR)/arch/x86/boot/bzImage \
    -drive format=raw,file=rootfs.img,if=ide \
    -append "init=/init root=/dev/sda rw console=ttyS0 nokaslr" \
    -nographic \
    -m 4G

ext4-image:
	bash create_ext4_image.sh
# 以下是备用命令行
# qemu-system-x86_64 \
# 	-kernel $(LINUX_DIR)/arch/x86/boot/bzImage \
# 	-drive format=raw,file=rootfs.img,if=ide \
# 	-append "init=/init root=/dev/sda rw console=ttyS0 nokaslr" \
# 	-nographic \
# 	-m 4G