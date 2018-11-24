.PHONY: all kernel install lint test clean

# Tools
########################
ifeq ($(ARCH),i686)
  ASM=nasm -felf32
  KERNEL_FILE=kernel32.bin
else ifeq ($(ARCH),x86_64)
  ASM=nasm -felf64
  KERNEL_FILE=kernel64.bin
else
  $(error "USAGE: IMAGE_FILE=xxx ARCH=i686|x86_64 PLATFORM=pc make [install]")
endif

ifndef PLATFORM
  $(error "USAGE: IMAGE_FILE=xxx ARCH=i686|x86_64 PLATFORM=pc make [install]")
endif

CROSS_CC      :=$(ARCH)-elf-gcc
CROSS_CXX     :=$(ARCH)-elf-g++
CROSS_LD      :=$(ARCH)-elf-ld
CROSS_AR      :=$(ARCH)-elf-ar -rcs
CROSS_OBJCOPY :=$(ARCH)-elf-objcopy
LIBGCC        :=$(shell $(CROSS_CC) -print-libgcc-file-name 2>/dev/null)
LIBC_PATH     :=../../libc/$(ARCH)-elf/

# Compiler Flags
########################
INCLUDE  =-I. -I$(LIBC_PATH)include -Iarch/$(ARCH) -I$(DRIVER_PATH)
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
          $(INCLUDE) -Werror $(CWARN) \
          -DARCH=$(ARCH) -DPLATFORM=$(PLATFORM)
CXXFLAGS =-fPIC -ffreestanding -masm=intel -std=c++14 -O2 \
          -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
          -fno-exceptions -fno-rtti -fno-stack-protector \
          $(INCLUDE) -Werror $(CXXWARN) \
          -DARCH=$(ARCH) -DPLATFORM=$(PLATFORM)
LDFLAGS  =-nostdinc -nostdlib
CPPLINTFLAGS=--quiet --linelength=200 --root=src \
             --filter=-legal/copyright,-build/include_subdir,-build/c++11,-readability/todo,-build/include_what_you_use
CLINTFLAGS  =--quiet --linelength=200 --root=src \
             --filter=-legal/copyright,-build/include_subdir,-build/c++11,-readability/todo,-build/include_what_you_use,-readability/casting


all: kernel
	@:

##############################################################
#
# KERNEL BUILTIN DRIVERS
#
##############################################################
DRIVER_PATH      :=../driver/
DRIVER_BUILTIN   :=serial
DRIVER_BUILTIN_A :=$(wildcard $(addprefix $(DRIVER_PATH),$(addsuffix /*.a,$(DRIVER_BUILTIN))))
builtin-drivers: $(DRIVER_BUILTIN)
	@:
$(DRIVER_BUILTIN): FORCE
	@ARCH=$(ARCH) make --no-print-directory -C $(DRIVER_PATH)$@

##############################################################
#
# KERNEL
#
##############################################################
KERNEL_TMP          :=.build/$(ARCH)/
KERNEL_CORE_ASM     :=$(sort $(wildcard *.asm))
KERNEL_CORE_C       :=$(sort $(wildcard *.c))
KERNEL_CORE_CPP     :=$(sort $(wildcard *.cpp))
KERNEL_ARCH_ASM     :=$(sort $(wildcard arch/$(ARCH)/*.asm))
KERNEL_ARCH_C       :=$(sort $(wildcard arch/$(ARCH)/*.c))
KERNEL_ARCH_CPP     :=$(sort $(wildcard arch/$(ARCH)/*.cpp))
KERNEL_PLATFORM_ASM :=$(sort $(wildcard platform/$(PLATFORM)/*.asm))
KERNEL_PLATFORM_C   :=$(sort $(wildcard platform/$(PLATFORM)/*.c))
KERNEL_PLATFORM_CPP :=$(sort $(wildcard platform/$(PLATFORM)/*.cpp))
KERNEL_HEADER       :=$(sort $(wildcard *.h) \
                             $(wildcard std/*.h) \
                             $(wildcard arch/$(ARCH)/*.h) \
                             $(wildcard platform/$(PLATFORM)/*.h) \
                             $(wildcard ddk/*.h))
KERNEL_OBJ :=$(patsubst %.asm, $(KERNEL_TMP)%_a.o,   $(KERNEL_CORE_ASM)) \
             $(patsubst %.c,   $(KERNEL_TMP)%_c.o,   $(KERNEL_CORE_C))   \
	     $(patsubst %.cpp, $(KERNEL_TMP)%_cpp.o, $(KERNEL_CORE_CPP)) \
             $(patsubst arch/$(ARCH)/%.asm, $(KERNEL_TMP)$(ARCH)-%_a.o,   $(KERNEL_ARCH_ASM)) \
	     $(patsubst arch/$(ARCH)/%.c,   $(KERNEL_TMP)$(ARCH)-%_c.o,   $(KERNEL_ARCH_C))   \
	     $(patsubst arch/$(ARCH)/%.cpp, $(KERNEL_TMP)$(ARCH)-%_cpp.o, $(KERNEL_ARCH_CPP)) \
             $(patsubst platform/$(PLATFORM)/%.asm, $(KERNEL_TMP)$(PLATFORM)-%_a.o,   $(KERNEL_PLATFORM_ASM)) \
	     $(patsubst platform/$(PLATFORM)/%.c,   $(KERNEL_TMP)$(PLATFORM)-%_c.o,   $(KERNEL_PLATFORM_C))   \
	     $(patsubst platform/$(PLATFORM)/%.cpp, $(KERNEL_TMP)$(PLATFORM)-%_cpp.o, $(KERNEL_PLATFORM_CPP)) \
	     $(patsubst $(DRIVER_PATH)/%.cpp, $(KERNEL_TMP)driver-%_cpp.o, $(DRIVER_BUILTIN_CPP)) \
	     $(patsubst $(DRIVER_PATH)/%.c, $(KERNEL_TMP)driver-%_c.o, $(DRIVER_BUILTIN_C))

KERNEL_DEP    :=$(patsubst %.o,%.d,$(KERNEL_OBJ))
KERNEL_BIN    :=$(KERNEL_TMP)$(KERNEL_FILE)
KERNEL_SYM    :=$(KERNEL_BIN:.bin=.sym)
KERNEL_USELIB :=$(LIBGCC)

KERNEL_TEST_CPP  :=$(sort $(wildcard test/*.cpp))
KERNEL_TEST_PROG :=$(patsubst test/%.cpp,$(KERNEL_TMP)%.test,$(KERNEL_TEST_CPP))

-include $(KERNEL_DEP)

kernel: $(KERNEL_BIN)
$(KERNEL_BIN): $(KERNEL_TMP) $(KERNEL_OBJ) builtin-drivers Makefile
	@echo "[LINK] $(notdir $@)"
	@$(CROSS_LD) $(LDFLAGS) -Tarch/$(ARCH)/kernel.ld \
	  -o $@ $(KERNEL_OBJ) \
	  $(DRIVER_BUILTIN_A) \
	  $(KERNEL_USELIB) \
	  -L$(LIBC_PATH)lib -lc -lg -lm -lnosys
	@$(CROSS_OBJCOPY) --only-keep-debug $@ $(KERNEL_SYM)
	@$(CROSS_OBJCOPY) --strip-debug --strip-unneeded $@
$(KERNEL_TMP):
	@mkdir -p $@
$(KERNEL_TMP)%_a.o: %.asm Makefile
	@echo [ASM ] $<
	@$(ASM) -MD $(KERNEL_TMP)-$*_a.d -MP $< -o $@
$(KERNEL_TMP)%_c.o: %.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_CC) $(CFLAGS) -MD -MP -c $< -o $@
$(KERNEL_TMP)%_cpp.o: %.cpp Makefile
	@echo "[CXX ] $<"
	@$(CROSS_CXX) $(CXXFLAGS) -MD -MP -c $< -o $@
$(KERNEL_TMP)$(ARCH)-%_a.o: arch/$(ARCH)/%.asm Makefile
	@echo [ASM ] $<
	@$(ASM) -Iarch/$(ARCH)/ -MD $(KERNEL_TMP)$(ARCH)-$*_a.d -MP $< -o $@
$(KERNEL_TMP)$(ARCH)-%_c.o: arch/$(ARCH)/%.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_CC) $(CFLAGS) -MD -MP -c $< -o $@
$(KERNEL_TMP)$(ARCH)-%_cpp.o: arch/$(ARCH)/%.cpp Makefile
	@echo "[CXX ] $<"
	@$(CROSS_CXX) $(CXXFLAGS) -MD -MP -c $< -o $@
$(KERNEL_TMP)$(PLATFORM)-%_a.o: platform/$(PLATFORM)/%.asm Makefile
	@echo [ASM ] $<
	@$(ASM) -Iplatform/$(PLATFORM)/ -MD $(KERNEL_TMP)$(PLATFORM)-$*_a.d -MP $< -o $@
$(KERNEL_TMP)$(PLATFORM)-%_c.o: platform/$(PLATFORM)/%.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_CC) $(CFLAGS) -MD -MP -c $< -o $@
$(KERNEL_TMP)$(PLATFORM)-%_cpp.o: platform/$(PLATFORM)/%.cpp Makefile
	@echo "[CXX ] $<"
	@$(CROSS_CXX) $(CXXFLAGS) -MD -MP -c $< -o $@
$(KERNEL_TMP)driver-%_c.o: $(DRIVER_PATH)%.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_CC) $(CFLAGS) -MD -MP -c $< -o $@
$(KERNEL_TMP)driver-%_cpp.o: $(DRIVER_PATH)%.cpp Makefile
	@echo "[CXX ] $<"
	@$(CROSS_CXX) $(CXXFLAGS) -MD -MP -c $< -o $@


##############################################################
install: kernel
	@echo "[COPY] $(notdir $(KERNEL_BIN))"
	@mdel -i "$(IMAGE_FILE)@@1M" ::$(shell echo $(notdir $(KERNEL_BIN)) | tr a-z A-Z) > /dev/null 2>&1 || true
	@mcopy -i "$(IMAGE_FILE)@@1M" $(KERNEL_BIN) ::$(shell echo $(notdir $(KERNEL_BIN)) | tr a-z A-Z)

##############################################################
lint:
	@echo "[LINT] $(notdir $(KERNEL_BIN))"
	@cpplint $(CLINTFLAGS) $(KERNEL_C) $(KERNEL_ARCH_C) $(KERNEL_PLATFORM_C)
	@cpplint $(CPPLINTFLAGS) $(KERNEL_HEADER) $(KERNEL_CPP) $(KERNEL_ARCH_CPP) $(KERNEL_PLATFORM_CPP)


$(KERNEL_TMP)%.test: test/%.cpp FORCE
	@echo "[CXX ] $<"
	@$(CXX) -std=c++14 -O2 -lstdc++ -I. $< -o $@

test: $(KERNEL_TMP) $(KERNEL_TEST_PROG)
	@rc=0; count=0; \
	for file in $(KERNEL_TEST_PROG); do \
	    echo "[TEST] $$file"; \
	    if ! ./$$file; then rc=`expr $$rc + 1`; fi; \
	    count=`expr $$count + 1`; \
	done; \
	echo "Tests executed: $$count  Tests failed: $$rc"; \
	if [ "$$rc" != "0" ]; then false; fi


##############################################################
clean:
	@rm -f $(KERNEL_OBJ) $(KERNEL_DEP)

FORCE:

