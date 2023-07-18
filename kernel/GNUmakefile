# This is the name that our final kernel executable will have.
# Change as needed.
override KERNEL := kernel.elf

# Convenience macro to reliably declare overridable command variables.
define DEFAULT_VAR =
    ifeq ($(origin $1),default)
        override $(1) := $(2)
    endif
    ifeq ($(origin $1),undefined)
        override $(1) := $(2)
    endif
endef

# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
#$(eval $(call DEFAULT_VAR,CC,cc))
CC = gcc

# Same thing for "ld" (the linker).
$(eval $(call DEFAULT_VAR,LD,ld))

# User controllable CFLAGS.
CFLAGS ?= -g -O2 -pipe -Wall -Wextra

# User controllable preprocessor flags. We set none by default.
CPPFLAGS ?=

# User controllable nasm flags.
NASMFLAGS ?= -F dwarf -g

# User controllable linker flags. We set none by default.
LDFLAGS ?=

# Internal C flags that should not be changed by the user.

override CFLAGS +=       \
    -std=c14             \
    -ffreestanding       \
    -fno-stack-protector \
    -fno-stack-check     \
    -fno-lto             \
    -fno-pie             \
    -fno-pic             \
    -m64                 \
    -march=x86-64        \
    -mabi=sysv           \
    -mno-80387           \
    -mno-mmx             \
    -mno-sse             \
    -mno-sse2            \
    -mno-red-zone        \
    -mcmodel=kernel      \
    -MMD                 \
    -I.
    
CFLAGS2 = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fpermissive -Wno-pmf-conversions -w
CFLAGS3 = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -w

#-ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions -fcommon

# Internal linker flags that should not be changed by the user.
override LDFLAGS +=         \
    -nostdlib               \
    -static                 \
    -Bsymbolic              \
    -m elf_x86_64           \
    -z max-page-size=0x1000 \
    -T linker.ld

# Check if the linker supports -no-pie and enable it if it does
ifeq ($(shell $(LD) --help 2>&1 | grep 'no-pie' >/dev/null 2>&1; echo $$?),0)
    override LDFLAGS += -no-pie
endif

# Internal nasm flags that should not be changed by the user.
override NASMFLAGS += \
    -f elf64

# Use find to glob all *.c, *.S, and *.asm files in the directory and extract the object names.


override CFILES    := $(shell find . -type f -name '*.c')
override CPPFILES  := $(shell find . -type f -name '*.cpp')
override ASFILES   := $(shell find . -type f -name '*.S')
override NASMFILES := $(shell find . -type f -name '*.asm')

override OBJ         := $(CFILES:.c=.o) $(CPPFILES:.cpp=.o) $(ASFILES:.S=.o) $(NASMFILES:.asm=_asm.o)
override HEADER_DEPS := $(CFILES:.c=.d) $(CPPFILES:.cpp=.d) $(ASFILES:.S=.d)

# Default target.
.PHONY: all
all: $(KERNEL)

limine.h:
	curl https://raw.githubusercontent.com/limine-bootloader/limine/trunk/limine.h -o $@

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@

# Include header dependencies.
-include $(HEADER_DEPS)

# Compilation rules for *.asm (nasm) files.
%gdt/gdt_asm.o: %gdt/gdt.asm
	@ echo !==== COMPILING $^
	@ echo !==== COMPILING $^
	nasm $(NASMFLAGS) $^ -f elf64 -o $@


%interrupts/interrupts.o: %interrupts/interrupts.cpp
	@ echo !==== COMPILING $^
	@ echo !==== COMPILING $^
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -fno-omit-frame-pointer -c $< -o $@
	
%ahci/ahci.o: %ahci/ahci.cpp
	@ echo !==== COMPILING $^
	@ echo !==== COMPILING $^
	$(CC) $(CFLAGS2) -c $< -o $@

#$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -fno-omit-frame-pointer -c $< -o $@

%pit/pit.o: %pit/pit.cpp
	@ echo !==== COMPILING $^
	@ echo !==== COMPILING $^
	$(CC) $(CFLAGS2) -c $< -o $@
	

	
kernel/src/WindowStuff/WindowManager/%.o: kernel/src/WindowStuff/WindowManager/%.cpp
	$(CC) $(CFLAGS2) -O2 -c $< -o $@	
	
%/rendering/BasicRenderer.o: %/rendering/BasicRenderer.cpp
	$(CC) $(CFLAGS2) -O1 -c $< -o $@	
	
%/rendering/VirtualRenderer.o: %/rendering/VirtualRenderer.cpp
	$(CC) $(CFLAGS2) -O1 -c $< -o $@	



%/test/testTask.o: %/test/testTask.cpp
	$(CC) $(CFLAGS2) -c $^ -o $@ -lm



# Compilation rules for *.cpp files.
%.o: %.cpp
	$(CC) $(CFLAGS2) -c $^ -o $@
	
# Compilation rules for *.cpp files.
%.o: %.c
	$(CC) $(CFLAGS3) -c $^ -o $@



# Compilation rules for *.c files.
%.o: %.c limine.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
	
	
# Compilation rules for *.S files.
%.o: %.S limine.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@



# Compilation rules for *.asm (nasm) files.
%.o: %.asm
	nasm $(NASMFLAGS) $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ) $(HEADER_DEPS)

.PHONY: distclean
distclean: clean
	rm -f limine.h
