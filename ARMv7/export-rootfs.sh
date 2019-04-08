cd rootfs/ && sudo find . -print0 2>/dev/null | sudo cpio -0ovH newc | xz --check=crc32 --lzma2=dict=512KiB -9 > ../rootfs.cpio.xz
cd ..
mkimage -A arm -T ramdisk -C lzma -d rootfs.cpio.xz rootfs.cpio.uboot 
