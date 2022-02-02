#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

typedef struct {
  void* BaseAddr;
  size_t Size;
  unsigned int Width;
  unsigned int Height;
  unsigned int ppsl; //Pixels per scan line
} Framebuffer;

#define PSF_MAGIC0 0x36
#define PSF_MAGIC1 0x04

typedef struct {
  unsigned char magic[2];
  unsigned char mode;
  unsigned char charsize;
} PSF_HEADER;

typedef struct {
  PSF_HEADER* header;
  void* buffer;
} PSF_FONT;

EFI_FILE* loadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

PSF_FONT* loadPSFFont(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
  EFI_FILE* font = loadFile(Directory, Path, ImageHandle, SystemTable);
  if (font == NULL) return NULL;

  PSF_HEADER* header;
  SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF_HEADER), (void**)&header);
  UINTN size = sizeof(PSF_HEADER);
  font->Read(font, &size, header);

  if (header->magic[0] != PSF_MAGIC0 || header->magic[1] != PSF_MAGIC1) {
    return NULL;
  }

  UINTN bufferSize = header->charsize * 256;
  if (header->mode == 1) { // 512 glyph mode
    bufferSize = header->charsize * 512;
  }

  void* buffer; {
    font->SetPosition(font, sizeof(PSF_HEADER));
    SystemTable->BootServices->AllocatePool(EfiLoaderData, bufferSize, (void**)&buffer);
    font->Read(font, &bufferSize, buffer);
  }
  
  PSF_FONT* finalFont;
  SystemTable->BootServices->AllocatePool(EfiLoaderData, sizeof(PSF_FONT), (void**)&finalFont);

  finalFont->header = header;
  finalFont->buffer = buffer;
  return finalFont;

}

Framebuffer framebuffer;
Framebuffer* GOPInit() {
  EFI_GUID gopGuid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
  EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
  EFI_STATUS status;

  status = uefi_call_wrapper(BS->LocateProtocol, 3, &gopGuid, NULL, (void**)&gop);
  if (EFI_ERROR(status)) {
    Print(L"Unable to Locate GOP\n\r");
    return NULL;
  } else {
    Print(L"GOP Located\n\r");
  }
  
  framebuffer.BaseAddr = (void*)gop->Mode->FrameBufferBase;
  framebuffer.Size = gop->Mode->FrameBufferSize;
  framebuffer.Width = gop->Mode->Info->HorizontalResolution;
  framebuffer.Height = gop->Mode->Info->VerticalResolution;
  framebuffer.ppsl = gop->Mode->Info->PixelsPerScanLine;

  return &framebuffer;
}

EFI_FILE* loadFile(EFI_FILE* Directory, CHAR16* Path, EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
  EFI_FILE* LoadedFile;

  EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
  SystemTable->BootServices->HandleProtocol(ImageHandle, &gEfiLoadedImageProtocolGuid, (void**)&LoadedImage);

  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
  SystemTable->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&FileSystem);

  if (Directory == NULL) {
    FileSystem->OpenVolume(FileSystem, &Directory);
  }

  EFI_STATUS s = Directory->Open(Directory, &LoadedFile, Path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);

  if (s != EFI_SUCCESS) {
    return NULL;
  }
  return LoadedFile;
}

int memcmp(const void* aptr, const void* bptr, size_t n) {
  const unsigned char* a = aptr, *b = bptr;
    for (size_t i = 0; i < n; i++) {
    if (a[i] < b[i]) return -1;
    else if (a[i] > b[i]) return 1;
  }
  return 0;
}

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
	InitializeLib(ImageHandle, SystemTable);
  Print(L"Loading Kernel...\n\r");

  EFI_FILE* Kernel = loadFile(NULL, L"kernel", ImageHandle, SystemTable);
  if (Kernel == NULL) {
    Print(L"ERR: Kernel Not Found\n\r");
  } else {
    Print(L"Kernel Found\n\r");
  }

  Elf64_Ehdr header; {
    UINTN FileInfoSize;
    EFI_FILE_INFO* FileInfo;
    Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, NULL);
    SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (void**)&FileInfo);
    Kernel->GetInfo(Kernel, &gEfiFileInfoGuid, &FileInfoSize, (void**)&FileInfo);

    UINTN size = sizeof(header);
    Kernel->Read(Kernel, &size, &header);
  }

  if (
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||
		header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
	) {
    Print(L"Bad Kernel\n\r");
  } else {
    Print(L"Kernel Verified\n\r");
  }

  Elf64_Phdr* phdrs; {
    Kernel->SetPosition(Kernel, header.e_phoff);
    UINTN size = header.e_phnum * header.e_phentsize;
    SystemTable->BootServices->AllocatePool(EfiLoaderData, size, (void**)&phdrs);
    Kernel->Read(Kernel, &size, phdrs);  
  }
  
  for (
    Elf64_Phdr* phdr = phdrs;
    (char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
    phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
  ) {
    switch (phdr->p_type) {
      case PT_LOAD: {
        int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
        Elf64_Addr segment = phdr->p_paddr;
        SystemTable->BootServices->AllocatePages(AllocateAddress, EfiLoaderData, pages, &segment);
      
        Kernel->SetPosition(Kernel, phdr->p_offset);
        UINTN size = phdr->p_filesz;
        Kernel->Read(Kernel, &size, (void*)segment);
        break;
      }
    }
  }
  void (*KernelMain)(Framebuffer*, PSF_FONT*) = ((__attribute__((sysv_abi)) void (*)(Framebuffer*, PSF_FONT*) ) header.e_entry);
  Print(L"Kernel Loaded\n\r");

  // Pre init
  PSF_FONT* font = loadPSFFont(NULL, L"font.psf", ImageHandle, SystemTable);
  if (font == NULL) {
    Print(L"Font is invalid or not found.\n\r");
  } else {
    Print(L"Font found and loaded.\n\rChar size = %d\n\r", font->header->charsize);
  }

  Framebuffer* buffer = GOPInit();
  unsigned int BytesPerPixel = 4;//buffer->Size / (buffer->ppsl * buffer->Height);
  Print(L"Base: 0x%x\n\rSize: 0x%x\n\rWidth: %d\n\rHeight: %d\n\rPixels Per Scan Line: %d\n\rBytes Per Pixel: %d\n\r",
  buffer->BaseAddr,
  buffer->Size,
  buffer->Width,
  buffer->Height,
  buffer->ppsl,
  BytesPerPixel
  );
  
  //jump to kernel
  KernelMain(buffer, font);
  return EFI_SUCCESS;  // Exit the UEFI application
}