OVMF_DIR = /home/luowen/software/edk2/Build/OvmfX64/DEBUG_GCC5/FV
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
#text file attributes
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
# cp /home/luowen/software/edk2/Build/MyPkg/DEBUG_GCC5/X64/MyPkg/RuntimeInfoDxe/RuntimeInfoDxe/OUTPUT/RuntimeInfoDxe.efi ./uefi
run-uefi:
	cp -f /home/luowen/software/edk2/Build/Mde/RELEASE_GCC5/X64/MyPkg/My/OUTPUT/hello.efi ./uefi/hello.efi 
	qemu-system-x86_64 \
			-nographic \
			-m 4G \
			-drive if=pflash,format=raw,unit=0,file="$(OVMF_DIR)/OVMF_CODE.fd",readonly=on \
			-drive if=pflash,format=raw,unit=1,file="$(OVMF_DIR)/OVMF_VARS.fd" \
			-drive file=fat:rw:./uefi,format=raw,if=ide,index=0 \