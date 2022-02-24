
OSNAME = RedX

GNUEFI = ../gnu-efi
OVMFDIR = ../OVMFbin
LDS = kernel.ld
CC = gcc
AS = nasm
LD = ld

CFLAGS = -ffreestanding -fshort-wchar -mno-red-zone
ASFLAGS =
LDFLAGS = -T $(LDS) -static -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := lib
BUILDDIR = bin
BOOTEFI := $(GNUEFI)/x86_64/bootloader/main.efi
FONTFILE := font.psf

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
ASSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%.o, $(ASSRC))
DIRS = $(wildcard $(SRCDIR)/*)

kernel: $(OBJS) link
$(OBJDIR)/interrupts/interrupts.o: $(SRCDIR)/interrupts/interrupts.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) -mno-red-zone -mgeneral-regs-only -ffreestanding -c $^ -o $@ -lm1048488

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@ -lm

$(OBJDIR)/%.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(AS) $(ASFLAGS) $^ -f elf64 -o $@

link:
	@ echo !==== LINKING $^
	$(LD) $(LDFLAGS) -o $(BUILDDIR)/kernel $(OBJS)

setup:
	@mkdir $(BUILDDIR)
	@mkdir $(SRCDIR)
	@mkdir $(OBJDIR)

buildiso:
	-@ rm $(BUILDDIR)/$(OSNAME).iso
	-@ rm $(BUILDDIR)/$(OSNAME)-fat.iso
	-@ rm $(BUILDDIR)/$(OSNAME)-mbr
	@ dd if=../bootloader/bin of=$(BUILDDIR)/$(OSNAME)-mbr bs=512
	@ dd if=/dev/zero bs=512 count=93750 >> $(BUILDDIR)/$(OSNAME)-fat.iso
	@ parted $(BUILDDIR)/$(OSNAME)-fat.iso mktable msdos
	@ parted $(BUILDDIR)/$(OSNAME)-fat.iso mkpart primary fat32 1048488B 45M
	@ mkfs.fat -F32 $(BUILDDIR)/$(OSNAME)-fat.iso
	@ mmd -i $(BUILDDIR)/$(OSNAME)-fat.iso ::/EFI
	@ mmd -i $(BUILDDIR)/$(OSNAME)-fat.iso ::/EFI/BOOT
	@ mcopy -i $(BUILDDIR)/$(OSNAME)-fat.iso $(BOOTEFI) ::/EFI/BOOT/BOOTX64.EFI
	@ mcopy -i $(BUILDDIR)/$(OSNAME)-fat.iso startup.nsh ::
	@ mcopy -i $(BUILDDIR)/$(OSNAME)-fat.iso $(BUILDDIR)/kernel ::
	@ mcopy -i $(BUILDDIR)/$(OSNAME)-fat.iso $(FONTFILE) ::
	@ dd if=$(BUILDDIR)/$(OSNAME)-mbr of=$(BUILDDIR)/$(OSNAME).iso bs=512
	@ dd if=$(BUILDDIR)/$(OSNAME)-fat.iso bs=512 >> $(BUILDDIR)/$(OSNAME).iso

flashsda:
	sudo dd if=../bootloader/bin of=/dev/sda
	sudo parted /dev/sda mklabel msdos
	sudo parted /dev/sda mkpart primary fat32 1 4G
	sudo mkfs.fat -F32 /dev/sda1
	- sudo mount /dev/sda1 /mnt
	make flash
	sudo umount /dev/sda1

flashsdb:
	sudo dd if=../bootloader/bin of=/dev/sdb
	sudo parted /dev/sdb mklabel msdos
	sudo parted /dev/sdb mkpart primary fat32 1 4G
	sudo mkfs.fat -F32 /dev/sdb1
	- sudo mount /dev/sdb1 /mnt
	make flash
	sudo umount /dev/sdb1

flashsdc:
	sudo dd if=../bootloader/bin of=/dev/sdc
	sudo parted /dev/sdc mklabel msdos
	sudo parted /dev/sdc mkpart primary fat32 1 4G
	sudo mkfs.fat -F32 /dev/sdc1
	- sudo mount /dev/sdc1 /mnt
	make flash
	sudo umount /dev/sdc1

flash:
	sudo mkdir -p /mnt/EFI/BOOT
	sudo cp $(BOOTEFI) /mnt/EFI/BOOT/BOOTX64.EFI
	sudo cp startup.nsh /mnt
	sudo cp $(BUILDDIR)/kernel /mnt
	sudo cp $(FONTFILE) /mnt/font.psf

run:
	qemu-system-x86_64 -drive file=$(BUILDDIR)/$(OSNAME)-fat.iso -m 256M -cpu qemu64 -accel kvm -drive if=pflash,format=raw,unit=0,file="$(OVMFDIR)/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="$(OVMFDIR)/OVMF_VARS-pure-efi.fd" -net none -d int -M smm=off -no-reboot -no-shutdown