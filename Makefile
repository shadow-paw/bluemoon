.PHONY: all image delete-image image-ls boot i686 x86_64 run-i686 run-x86_64 clean

IMAGE_FILE=`pwd`/dist/bluemoon.img
# QEMU_FLAGS=-m 32 -vga std -serial stdio
# QEMU_FLAGS=-m 32 -vga cirrus -serial stdio
QEMU_FLAGS=-m 32 -nographic


all:
	@echo "USAGE: make image|delete-image|image-ls|boot|i686|x86_64|run-i686|run-x86_64"

image:
	@IMAGE_FILE="$(IMAGE_FILE)" make --no-print-directory -C src/tools/mkosimage
	@if [ ! -f "$(IMAGE_FILE)" ]; then \
	  mkdir -p "$(dir $(IMAGE_FILE))" && \
	  bin/mkosimage --file="$(IMAGE_FILE)" --size=10 && \
	  mformat -t 36 -i $(IMAGE_FILE)@@1M  -h 16 -s 32 -H 2048 :: ; \
	fi

delete-image:
	@-rm -f "$(IMAGE_FILE)"

image-ls: image
	@mdir -i $(IMAGE_FILE)@@1M ::

boot: image
	@IMAGE_FILE="$(IMAGE_FILE)" make --no-print-directory -C src/boot install

i686:
	@IMAGE_FILE="$(IMAGE_FILE)" ARCH=i686 make --no-print-directory -C src/kernel install

x86_64:
	@IMAGE_FILE="$(IMAGE_FILE)" ARCH=x86_64 make --no-print-directory -C src/kernel install

run-i686:
	@-qemu-system-i386 $(QEMU_FLAGS) -drive format=raw,file="$(IMAGE_FILE)"

run-x86_64:
	@-qemu-system-x86_64 $(QEMU_FLAGS) -drive format=raw,file="$(IMAGE_FILE)"

clean:
	@-make --no-print-directory -C src/boot clean
	@-ARCH=i686 make --no-print-directory -C src/kernel clean
	@-ARCH=x86_64 make --no-print-directory -C src/kernel clean
