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

double sqrt(double number);
int roundd(double x);
void cls(Framebuffer* buffer, unsigned int BPP);

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
  void (*KernelMain)(Framebuffer* ) = ((__attribute__((sysv_abi)) void (*)(Framebuffer*) ) header.e_entry);
  Print(L"Kernel Loaded\n\r");

  //Pre init
  Framebuffer* newBuffer = GOPInit();

  Print(L"Base: 0x%x\n\rSize: 0x%x\n\rWidth: %d\n\rHeight: %d\n\rPixels Per Scan Line: %d\n\r",
  newBuffer->BaseAddr,
  newBuffer->Size,
  newBuffer->Width,
  newBuffer->Height,
  newBuffer->ppsl
  );

  cls(newBuffer, 4);
  
  //jump to kernel
  KernelMain(newBuffer);
  
  return EFI_SUCCESS;  // Exit the UEFI application
}

void plotPixel(Framebuffer* buffer, unsigned int BPP, unsigned int x, unsigned int y, unsigned int color) {
  *(unsigned int*)(x * BPP + (y * buffer->ppsl * BPP) + buffer->BaseAddr) = color;
}

void drawLine(Framebuffer* buffer, unsigned int BPP, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  int a = y2 - y1;
  int b = x2 - x1;
  int length = sqrt(b *  b + a * a);
  for (double i = 0; i < roundd(length * 2); i++) {
    plotPixel(buffer, BPP,
      x1 + roundd(b * (i / roundd(length * 2))),
      y1 + roundd(a * (i / roundd(length * 2))),
      color);
  }
}

void cls(Framebuffer* buffer, unsigned int BPP) {
  for (unsigned int x = 0; x < buffer->Width; x++) {
    for (unsigned int y = 0; y < buffer->Height; y++) {
      plotPixel(buffer, BPP, x, y, 0x00000000);
    }
  }
}

void rect(Framebuffer* buffer, unsigned int BPP,  unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  for (unsigned int x = x1; x <= x2; x++) {
    for (unsigned int y = y1; y <= y2; y++) {
      plotPixel(buffer, BPP, x, y, color);
    }
  }
}
double sqrt(double number) {
  double temp = 0;
  double root = number / 2;
  int i = 0;
  while (root != temp && i < 10000) {
    temp = root;
    root = (number / temp + temp) / 2;
    i++;
  }
  return root;
}

double rand() {
  return 0;
}

int roundd(double x) {
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}