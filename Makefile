.PHONY: all image boot mount umount i686 x86_64 run-i686 run-x86_64 clean

IMAGE_FILE=`pwd`/dist/bluemoon.img
ROOTFS=`pwd`/rootfs
LODEVICE=/dev/loop0


all:
	@echo "USAGE: make image|boot|mount|umount|i686|x86_64|run-i686|run-x86_64"

image:
	@IMAGE_FILE="$(IMAGE_FILE)" make --no-print-directory -C src/tools/mkosimage
	@if [ ! -f "$(IMAGE_FILE)" ]; then mkdir -p "$(dir $(IMAGE_FILE))" && bin/mkosimage --file="$(IMAGE_FILE)" --size=10; fi

boot: image
	@IMAGE_FILE="$(IMAGE_FILE)" ROOTFS="$(ROOTFS)" make --no-print-directory -C src/boot install

i686: boot
	@echo "[ ] nothing yet"

x86_64: boot
	@echo "[ ] nothing yet"

mount: image
	@mkdir -p "$(ROOTFS)"
	@findmnt -M $(ROOTFS) || (sudo losetup -o1048576 "$(LODEVICE)" "$(IMAGE_FILE)" && sudo mount -tvfat -o noatime,uid=1000 /dev/loop0 "$(ROOTFS)")

umount:
	@(findmnt -M $(ROOTFS) && (sudo umount "$(IMAGE_FILE)" && sudo losetup -d "$(LODEVICE)")) || true

run-i686:
	@-qemu-system-i386 -m 32 -vga std -serial stdio -drive format=raw,file="$(IMAGE_FILE)"

run-x86_64:
	@-qemu-system-x86_64 -m 32 -vga std -serial stdio -drive format=raw,file="$(IMAGE_FILE)"

clean: umount
	@-rm -f "$(DISK_IMAGE)"
	@-make --no-print-directory -C src/boot clean
