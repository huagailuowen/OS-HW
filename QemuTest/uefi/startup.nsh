#acpi.efi
fs0:
load -nc RuntimeInfoDxe.efi
echo "Waiting for RuntimeInfoDxe to initialize..."
fs0:\linux_runtimeinfo.efi initrd=initramfs.cpio.gz root=/dev/ram0 console=ttyS0
