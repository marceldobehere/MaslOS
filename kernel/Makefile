
OSNAME = MaslOS

GNUEFI = ../gnu-efi
OVMFDIR = ../ovmfbin
LDS = kernel.ld
CC = gcc
ASMC = nasm
LD = ld

CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone -fno-omit-frame-pointer -fno-exceptions
ASMFLAGS = 
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)          
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm) 
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)




kernel: $(OBJS) link

$(OBJDIR)/interrupts/interrupts.o: $(SRCDIR)/interrupts/interrupts.cpp
	@ echo !==== COMPILING AAAAAAa $^
	@ mkdir -p $(@D)
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -fno-omit-frame-pointer -c $^ -o $@


$(OBJDIR)/kernelStuff/other_IO/pit/pit.o: $(SRCDIR)/kernelStuff/other_IO/pit/pit.cpp
	@ echo X==== COMPILING PIT $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@


$(OBJDIR)/kernelStuff/other_IO/ahci/ahci.o: $(SRCDIR)/kernelStuff/other_IO/ahci/ahci.cpp
	@ echo X==== COMPILING AHCI $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@


$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -O2 -c $^ -o $@




$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASMC) $(ASMFLAGS) $^ -f elf64 -o $@




link:
	@ echo !=== LINKING
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel.elf $(OBJS)
	@ echo DONE LINKING


setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildimg:
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512K count=100
	mformat -F -i $(BUILDDIR)/$(OSNAME).img ::
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel.elf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/zap-light16.psf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img src/external/test.mbif ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img src/external/background.mbif ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img src/external/mouse.mbzf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img src/external/window_icons.mbzf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img src/external/window_buttons.mbzf ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img src/external/boot.mbif ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img src/external/MButton.mbif ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img src/external/MButton_S.mbif ::

run:
	qemu-system-x86_64 -machine q35 -drive file=$(BUILDDIR)/$(OSNAME).img -drive file=$(BUILDDIR)/blank.img -m 1G -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none -soundhw pcspk
