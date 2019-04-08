dd if=rootfs.cpio.uboot bs=64 skip=1 of=rootfs.cpio.xz # Remove the U-Boot header
mkdir rootfs && cd rootfs
xz -dk ../rootfs.cpio.xz # Extract the archive
cat ../rootfs.cpio | sudo cpio -idm
