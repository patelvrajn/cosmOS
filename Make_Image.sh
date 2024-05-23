# Skeleton of script provided by: https://wiki.osdev.org/Bootable_Disk
# This script needs sudo to properly execute!

# Creates a file full of zero bytes.
dd if=/dev/zero of=cosmos.img bs=1048576 count=128

# Formats the file with GPT disk image data - creating 
# an EFI System partition aligned to the 2048 sector.
sgdisk -og cosmos.img
sgdisk -n 1:2048:67584 -c 1:"EFI System Partition" -t 1:ef00 cosmos.img

# Associates the image file with a loopback device which allows the file
# to be the target of device operations such as mkfs. Note that /dev/loop69
# was choosen as the loopback device as it was significantly higher numerically
# than any unavailable loopback device on my system seen by losetup -a. 
losetup -o 1048576 --sizelimit 33554432 /dev/loop69 cosmos.img

# Creates the FAT 16 filesystem on the device.
mkfs.vfat -F 16 /dev/loop69

# Create a directory and mount the filesystem to the directory.
mkdir mount_point69
mount /dev/loop69 mount_point69

# Copy over bootloader and kernel image to EFI/BOOT.
mkdir -p mount_point69/EFI/BOOT
cp bootloader/BOOTX64.EFI mount_point69/EFI/BOOT
cp kernel/kernel.bin mount_point69/EFI/BOOT

# Unmount and remove directory.
umount mount_point69
rmdir mount_point69

# Deattach the loopback device.
losetup -d /dev/loop69
