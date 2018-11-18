.PHONY: all kernel install clean

# Tools
########################
ifeq ($(ARCH),i686)
  ASM=nasm -felf32
  KERNEL_FILE=kernel32.bin
else ifeq ($(ARCH),x86_64)
  ASM=nasm -felf64
  KERNEL_FILE=kernel64.bin
else
  $(error "USAGE: IMAGE_FILE=xxx ARCH=i686|x86_64 make [install]")
endif
CC      :=$(ARCH)-elf-gcc
CXX     :=$(ARCH)-elf-g++
LD      :=$(ARCH)-elf-ld
AR      :=$(ARCH)-elf-ar -rcs
OBJCOPY :=$(ARCH)-elf-objcopy
LIBGCC  :=$(shell $(CC) -print-libgcc-file-name 2>/dev/null)

# Compiler Flags
########################
INCLUDE  =-Iarch/$(ARCH)
CWARN    =-Wall -Wextra -Wattributes -Wbuiltin-macro-redefined -Wcast-align -Wconversion         \
          -Wdiv-by-zero -Wdouble-promotion -Wenum-compare -Wfloat-equal -Winit-self              \
          -Wint-to-pointer-cast -Wlogical-op -Wmissing-braces -Wmissing-field-initializers       \
          -Woverflow -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wsign-compare     \
          -Wtype-limits -Wuninitialized -Wwrite-strings                                          \
          -Wno-unused-parameter -Wno-unused-variable -Wno-multichar -Wno-unused-but-set-variable \
          -Wdeclaration-after-statement -Wimplicit-int -Wjump-misses-init -Wpointer-sign         \
          -Wpointer-to-int-cast -Wmissing-parameter-type
CXXWARN  =-Wall -Wextra -Wattributes -Wbuiltin-macro-redefined -Wcast-align -Wconversion         \
          -Wdiv-by-zero -Wdouble-promotion -Wenum-compare -Wfloat-equal -Winit-self              \
          -Wint-to-pointer-cast -Wlogical-op -Wmissing-braces -Wmissing-field-initializers       \
          -Woverflow -Wpointer-arith -Wredundant-decls -Wreturn-type -Wshadow -Wsign-compare     \
          -Wtype-limits -Wuninitialized -Wwrite-strings                                          \
          -Wno-unused-parameter -Wno-unused-variable -Wno-multichar -Wno-unused-but-set-variable \
          -Wc++0x-compat  -Wsign-promo
CFLAGS   =-fPIC -ffreestanding -masm=intel -std=c99 -O2 \
          -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
          -DARCH=$(ARCH) $(INCLUDE) -Werror $(CWARN)
CXXFLAGS =-fPIC -ffreestanding -masm=intel -std=c++14 -O2 \
          -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
          -fno-exceptions -fno-rtti -fno-stack-protector \
          -DARCH=$(ARCH) $(INCLUDE) -Werror $(CXXWARN)
LDFLAGS  =-nostdinc -nostdlib
CPPLINTFLAGS=--quiet --linelength=200 --root=src --filter=-legal/copyright,-build/include_subdir,-build/c++11,-readability/todo
CLINTFLAGS  =--quiet --linelength=200 --root=src --filter=-legal/copyright,-build/include_subdir,-build/c++11,-readability/todo,-readability/casting


all: kernel
	@:

##############################################################
#
# KERNEL
#
##############################################################
KERNEL_TMP      :=.build/$(ARCH)/
KERNEL_C        :=$(sort $(wildcard *.c))
KERNEL_CPP      :=$(sort $(wildcard *.cpp))
KERNEL_ARCH_ASM :=$(sort $(wildcard arch/$(ARCH)/*.asm))
KERNEL_ARCH_C   :=$(sort $(wildcard arch/$(ARCH)/*.c))
KERNEL_ARCH_CPP :=$(sort $(wildcard arch/$(ARCH)/*.cpp))
KERNEL_HEADER   :=$(sort $(wildcard *.h) $(wildcard arch/$(ARCH)/*.h))
KERNEL_OBJ      :=$(patsubst %.c,    $(KERNEL_TMP)%_c.o,  $(KERNEL_C))     \
	          $(patsubst %.cpp,  $(KERNEL_TMP)%_cpp.o,$(KERNEL_CPP))   \
                  $(patsubst arch/$(ARCH)/%.asm,  $(KERNEL_TMP)$(ARCH)-%_a.o,  $(KERNEL_ARCH_ASM))  \
	          $(patsubst arch/$(ARCH)/%.c,    $(KERNEL_TMP)$(ARCH)-%_c.o,  $(KERNEL_ARCH_C))    \
	          $(patsubst arch/$(ARCH)/%.cpp,  $(KERNEL_TMP)$(ARCH)-%_cpp.o,$(KERNEL_ARCH_CPP))
KERNEL_DEP    :=$(patsubst %.o,%.d,$(KERNEL_OBJ))
KERNEL_BIN    :=$(KERNEL_TMP)$(KERNEL_FILE)
KERNEL_SYM    :=$(KERNEL_BIN:.bin=.sym)
KERNEL_USELIB :=$(LIBGCC)

-include $(KERNEL_DEP)

kernel: $(KERNEL_BIN)
$(KERNEL_BIN): $(KERNEL_TMP) $(KERNEL_OBJ) $(KERNEL_USELIB) Makefile
	@echo "[LINK] $(notdir $@)"
	@$(LD) $(LDFLAGS) -Tarch/$(ARCH)/kernel.ld -o $@ $(KERNEL_OBJ) $(KERNEL_USELIB)
	@$(OBJCOPY) --only-keep-debug $@ $(KERNEL_SYM)
	@$(OBJCOPY) --strip-debug --strip-unneeded $@
$(KERNEL_TMP):
	@mkdir -p $@
$(KERNEL_TMP)$(ARCH)-%_a.o: arch/$(ARCH)/%.asm Makefile
	@echo [ASM ] $<
	@$(ASM) -Iarch/$(ARCH)/ -MD $(KERNEL_TMP)$(ARCH)-$*_a.d -MP $< -o $@
$(KERNEL_TMP)$(ARCH)-%_c.o: arch/$(ARCH)/%.c Makefile
	@echo "[CC  ] $<"
	@$(CC) $(CFLAGS) -I. -MD -MP -c $< -o $@
$(KERNEL_TMP)$(ARCH)-%_cpp.o: arch/$(ARCH)/%.cpp Makefile
	@echo "[CXX ] $<"
	@$(CXX) $(CXXFLAGS) -I. -MD -MP -c $< -o $@
$(KERNEL_TMP)%_c.o: %.c Makefile
	@echo "[CC  ] $<"
	@$(CC) $(CFLAGS) -MD -MP -c $< -o $@
$(KERNEL_TMP)%_cpp.o: %.cpp Makefile
	@echo "[CXX ] $<"
	@$(CXX) $(CXXFLAGS) -MD -MP -c $< -o $@


##############################################################
install: kernel
	@echo "[COPY] $(notdir $(KERNEL_BIN))"
	@mdel -i "$(IMAGE_FILE)@@1M" ::$(shell echo $(notdir $(KERNEL_BIN)) | tr a-z A-Z) > /dev/null 2>&1 || true
	@mcopy -i "$(IMAGE_FILE)@@1M" $(KERNEL_BIN) ::$(shell echo $(notdir $(KERNEL_BIN)) | tr a-z A-Z)

##############################################################
lint:
	@echo "[LINT] $(notdir $(KERNEL_BIN))"
	@cpplint $(CLINTFLAGS) $(KERNEL_C) $(KERNEL_ARCH_C)
	@cpplint $(CPPLINTFLAGS) $(KERNEL_HEADER) $(KERNEL_CPP)

##############################################################
clean:
	@rm -f $(KERNEL_OBJ) $(KERNEL_DEP)
