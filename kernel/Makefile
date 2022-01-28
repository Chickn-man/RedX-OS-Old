
OSNAME = RedX

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kernel.ld
CC = gcc
LD = ld

CFLAGS = -ffreestanding -fshort-wchar
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi
FONTFILE := font.psf

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)          
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
DIRS = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@ -lm

link:
	@ echo !==== LINKING $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel $(OBJS)

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildimg:
	rm $(BUILDDIR)/$(OSNAME).img
	dd if=/dev/zero of=$(BUILDDIR)/$(OSNAME).img bs=512 count=93750
	mformat -i $(BUILDDIR)/$(OSNAME).img
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI
	mmd -i $(BUILDDIR)/$(OSNAME).img ::/EFI/BOOT
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BOOTEFI) ::/EFI/BOOT/BOOTX64.EFI
	mcopy -i $(BUILDDIR)/$(OSNAME).img startup.nsh ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(BUILDDIR)/kernel ::
	mcopy -i $(BUILDDIR)/$(OSNAME).img $(FONTFILE) ::

flashsda:
	sudo mount /dev/sda1 /mnt
	make flash
	sudo umount /dev/sda1

flashsdb:
	sudo mount /dev/sdb1 /mnt
	make flash
	sudo umount /dev/sdb1

flashsdc:
	sudo mount /dev/sdc1 /mnt
	make flash
	sudo umount /dev/sdc1

flash:
	sudo mkdir -p /mnt/EFI/BOOT
	sudo cp $(BOOTEFI) /mnt/EFI/BOOT/BOOTX64.EFI
	sudo cp startup.nsh /mnt
	sudo cp $(BUILDDIR)/kernel /mnt
	sudo cp $(FONTFILE) /mnt/font.psf

run:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME).img -m 1G -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none
