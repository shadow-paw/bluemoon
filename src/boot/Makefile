.PHONY: all mbr install-mbr vbr install-vbr boot install-boot install clean

BUILD_DIR=.build
AS=nasm


all: mbr
	@:

build-dir:
	@mkdir -p $(BUILD_DIR)

install: install-mbr install-vbr install-boot
	@:

#################################################
# MBR
#################################################
MBR_ASM=mbr.asm
MBR_BIN=$(BUILD_DIR)/mbr.bin

mbr: build-dir $(MBR_BIN)
	@:
$(MBR_BIN): $(MBR_ASM)
	@echo "[NASM] $< -> $@"
	@$(AS) -f bin $< -o $@
install-mbr: mbr
	@echo "[COPY] $(MBR_BIN)"
	@dd if="$(MBR_BIN)" of="$(IMAGE_FILE)" bs=446 count=1 conv=notrunc > /dev/null 2>&1
	@sync

#################################################
# VBR
#################################################
VBR_ASM=vbr-fat16.asm
VBR_BIN=$(BUILD_DIR)/vbr-fat16.bin

vbr: build-dir $(VBR_BIN)
	@:
$(VBR_BIN): $(VBR_ASM)
	@echo "[NASM] $< -> $@"
	@$(AS) -f bin $< -o $@
install-vbr: vbr
	@echo "[COPY] $(VBR_BIN)"
# NOTE: 1048638 = 2048(start sector) x 512 + 62
	@dd if="$(VBR_BIN)" of="$(IMAGE_FILE)" bs=1 skip=62 seek=1048638 count=450 conv=notrunc > /dev/null 2>&1

#################################################
# Boot Loader
#################################################
BOOT_ASM=boot.asm
BOOT_BIN=$(BUILD_DIR)/boot.bin

boot: build-dir $(BOOT_BIN)
	@:
$(BOOT_BIN): $(BOOT_ASM)
	@echo "[NASM] $< -> $@"
	@$(AS) -f bin $< -o $@
install-boot: boot
	@echo "[COPY] $(BOOT_BIN)"
	@cp "$(BOOT_BIN)" "$(ROOTFS)/"
	@sync

clean:
	@-rm -f "$(MBR_BIN)" "$(VBR_BIN)" "$(BOOT_BIN)"