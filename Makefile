.PHONY: all image image-delete image-ls boot i686 i686-run i686-debug i686-gdb x86_64 x86_64-run x86_64-debug x86_64-gdb clean

IMAGE_FILE=`pwd`/dist/bluemoon.img
# QEMU_FLAGS=-m 32 -vga std -serial stdio
QEMU_FLAGS=-m 32 -vga cirrus -serial stdio
# QEMU_FLAGS=-m 32 -nographic


all:
	@echo "USAGE: make image|image-delete|image-ls"
	@echo "       make boot"
	@echo "       make i686|i686-run|i686-debug|i686-gdb"
	@echo "       make x86_64|x86_64-run|x86_64-debug|x86_64-gdb"
	@echo "       make clean"

image:
	@IMAGE_FILE="$(IMAGE_FILE)" make --no-print-directory -C src/tools/mkosimage
	@if [ ! -f "$(IMAGE_FILE)" ]; then \
	  mkdir -p "$(dir $(IMAGE_FILE))" && \
	  bin/mkosimage --file="$(IMAGE_FILE)" --size=10 && \
	  mformat -t 36 -i $(IMAGE_FILE)@@1M  -h 16 -s 32 -H 2048 :: ; \
	fi

image-delete:
	@-rm -f "$(IMAGE_FILE)"

image-ls: image
	@mdir -i $(IMAGE_FILE)@@1M ::

boot: image
	@IMAGE_FILE="$(IMAGE_FILE)" make --no-print-directory -C src/boot install

i686:
	@IMAGE_FILE="$(IMAGE_FILE)" ARCH=i686 make --no-print-directory -C src/kernel install
# initrd, TODO: implement file packer
	@if [ ! -f dist/initrd32 ]; then \
	  dd if=/dev/zero of=dist/initrd32 bs=4k count=1 > /dev/null 2>&1; \
	  mdel -i $(IMAGE_FILE)@@1M ::INITRD32.BIN > /dev/null 2>&1 || true; \
	  mcopy -i $(IMAGE_FILE)@@1M dist/initrd32 ::INITRD32.BIN; \
	fi

x86_64:
	@IMAGE_FILE="$(IMAGE_FILE)" ARCH=x86_64 make --no-print-directory -C src/kernel install
# initrd, TODO: implement file packer
	@if [ ! -f dist/initrd64 ]; then \
	  dd if=/dev/zero of=dist/initrd64 bs=4k count=1 > /dev/null 2>&1; \
	  mdel -i $(IMAGE_FILE)@@1M ::INITRD64.BIN > /dev/null 2>&1 || true; \
	  mcopy -i $(IMAGE_FILE)@@1M dist/initrd64 ::INITRD64.BIN; \
	fi

i686-run:
	@echo "Starting qemu..."
	@echo "[I] Press CTRL-A, then X to quit."
	@-qemu-system-i386 $(QEMU_FLAGS) -drive format=raw,file="$(IMAGE_FILE)"

x86_64-run:
	@echo "Starting qemu..."
	@echo "[I] Press CTRL-A, then X to quit."
	@-qemu-system-x86_64 $(QEMU_FLAGS) -drive format=raw,file="$(IMAGE_FILE)"

i686-debug:
	@echo "Starting qemu..."
	@echo "[I] Press CTRL-A, then X to quit."
	@echo "[I] Started in debug mode, run make debug-i686 on another terminal."
	@-qemu-system-i386 -s $(QEMU_FLAGS) -drive format=raw,file="$(IMAGE_FILE)"

x86_64-debug:
	@echo "Starting qemu..."
	@echo "[I] Press CTRL-A, then X to quit."
	@echo "[I] Started in debug mode, run make debug-x86_64 on another terminal."
	@-qemu-system-x86_64 -s $(QEMU_FLAGS) -drive format=raw,file="$(IMAGE_FILE)"

i686-gdb:
	@i686-elf-gdb \
	  --eval-command="set disassembly-flavor intel" \
	  --eval-command="set history save on" \
	  --eval-command="set architecture i386" \
	  --eval-command="target remote localhost:1234" \
	  --symbols=src/kernel/.build/i686/kernel32.sym

x86_64-gdb:
	@x86_64-elf-gdb \
	  --eval-command="set disassembly-flavor intel" \
	  --eval-command="set history save on" \
	  --eval-command="set architecture i386:x86-64" \
	  --eval-command="target remote localhost:1234" \
	  --symbols=src/kernel/.build/x86_64/kernel64.sym

clean:
	@-make --no-print-directory -C src/boot clean
	@-ARCH=i686 make --no-print-directory -C src/kernel clean
	@-ARCH=x86_64 make --no-print-directory -C src/kernel clean
