.PHONY: all driver lint clean

CROSS_CC      :=$(ARCH)-elf-gcc
CROSS_CXX     :=$(ARCH)-elf-g++
CROSS_LD      :=$(ARCH)-elf-ld
CROSS_AR      :=$(ARCH)-elf-ar -rcs
CROSS_OBJCOPY :=$(ARCH)-elf-objcopy
LIBGCC    :=$(shell $(CROSS_CC) -print-libgcc-file-name 2>/dev/null)
LIBC_PATH :=../../../libc/$(ARCH)-elf/

# Compiler Flags
########################
INCLUDE  =-I$(LIBC_PATH)include -I../../kernel/
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
CFLAGS   =-fPIC -ffreestanding -masm=intel -std=c11 -O2 \
          -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
          $(INCLUDE) -Werror $(CWARN) \
          -DARCH=$(ARCH)
CXXFLAGS =-fPIC -ffreestanding -masm=intel -std=c++17 -O2 \
          -mno-mmx -mno-sse -mno-sse2 -mno-sse3 -mno-3dnow \
          -fno-exceptions -fno-rtti -fno-stack-protector \
          $(INCLUDE) -Werror $(CXXWARN) \
          -DARCH=$(ARCH)
LDFLAGS  =-nostdinc -nostdlib
CPPLINTFLAGS=--quiet --linelength=200 --root=src \
             --filter=-legal/copyright,-build/include_subdir,-build/c++11,-readability/todo,-build/include_what_you_use
CLINTFLAGS  =--quiet --linelength=200 --root=src \
             --filter=-legal/copyright,-build/include_subdir,-build/c++11,-readability/todo,-build/include_what_you_use,-readability/casting


all: driver
	@:

##############################################################
#
# DRIVER
#
##############################################################
DRIVER_NAME   :=$(shell basename $(CURDIR))
DRIVER_TMP    :=.build/$(ARCH)/
DRIVER_C      :=$(sort $(wildcard *.c))
DRIVER_CPP    :=$(sort $(wildcard *.cpp))
DRIVER_HEADER :=$(sort $(wildcard *.h))
DRIVER_OBJ    :=$(patsubst %.c,   $(DRIVER_TMP)%_c.o,   $(DRIVER_C))   \
	        $(patsubst %.cpp, $(DRIVER_TMP)%_cpp.o, $(DRIVER_CPP))
DRIVER_DEP    :=$(patsubst %.o,%.d,$(DRIVER_OBJ))
DRIVER_A      :=$(DRIVER_NAME).a
DRIVER_SO     :=$(DRIVER_NAME).so
DRIVER_USELIB :=$(LIBGCC)

-include $(DRIVER_DEP)

driver: $(DRIVER_A)
# $(DRIVER_SO)
$(DRIVER_A): $(DRIVER_TMP) $(DRIVER_OBJ) Makefile
	@echo "[LINK] $(notdir $@)"
	@$(CROSS_AR) -o $@ $(DRIVER_OBJ)
$(DRIVER_TMP):
	@mkdir -p $@
$(DRIVER_TMP)%_c.o: %.c Makefile
	@echo "[CC  ] $<"
	@$(CROSS_CC) $(CFLAGS) -MD -MP -c $< -o $@
$(DRIVER_TMP)%_cpp.o: %.cpp Makefile
	@echo "[CXX ] $<"
	@$(CROSS_CXX) $(CXXFLAGS) -MD -MP -c $< -o $@


##############################################################
lint:
	@echo "[LINT] $(notdir $(DRIVER_A))"
	@cpplint $(CLINTFLAGS) $(DRIVER_C)
	@cpplint $(CPPLINTFLAGS) $(DRIVER_HEADER) $(DRIVER_CPP)


##############################################################
clean:
	@rm -f $(DRIVER_OBJ) $(DRIVER_DEP) $(DRIVER_A)

FORCE:

