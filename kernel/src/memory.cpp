#include "memory.h"

const char* EFI_MEMORY_TYPE_STRINGS[] {
  "EfiReservedMemoryType",
  "EfiLoaderCode",
  "EfiLoaderData",
  "EfiBootServicesCode",
  "EfiBootServicesData",
  "EfiRuntimeServicesCode",
  "EfiRuntimeServicesData",
  "EfiConventionalMemory",
  "EfiUnusableMemory",
  "EfiACPIReclaimMemory",
  "EfiACPIMemoryNVS",
  "EfiMemoryMappedIO",
  "EfiMemoryMappedIOPortSpace",
  "EfiPalCode",
};

uint64_t getMemorySize(EFI_MEMORY_DESCRIPTOR* map, uint64_t entries, uint64_t descSize) {
  static uint64_t memSizeBytes = 0;
  if (memSizeBytes > 0) return memSizeBytes;

  for (int i = 0; i < entries; i++) {
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)map + (i * descSize));
    memSizeBytes += desc->pages * 4096;
  }

  return memSizeBytes;
}

uint64_t freeMem;
uint64_t reservedMem;
uint64_t usedMem;
bool initialized = false;

void pageAllocator::readMap(EFI_MEMORY_DESCRIPTOR* map, size_t size, size_t descSize) {
  if (initialized) return;
  initialized = true;

  uint64_t entries = size / descSize;

  void* largestSeg = NULL;
  size_t largestSegSize = 0;

  for (int i = 0; i < entries; i++) {
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)map + (i * descSize));
    if (desc->type == 7) {
      if (desc->pages * 4096 > largestSegSize) {
        largestSeg = desc->physAddr;
        largestSegSize = desc->pages * 4096;
      }
    } 
  }

  uint64_t memSize = getMemorySize(map, entries, descSize);
  freeMem = memSize;

  uint64_t bitmapSize = memSize / 4096 / 8 + 1;

  bitmapInit(bitmapSize, largestSeg);

  locks(pageBitmap.buffer, pageBitmap.size / 4096 + 1);

  for (int i = 0; i < entries; i++) {
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)((uint64_t)map + (i * descSize));
    if (desc->type != 7) {
      reserves(desc->physAddr, desc->pages);
    } 
  }

}

void pageAllocator::bitmapInit(size_t size, void* bufferAddr) {
  pageBitmap.size = size;
  pageBitmap.buffer = (uint8_t*)bufferAddr;

  for (int i = 0; i < size; i++) {
    *(uint8_t*)(pageBitmap.buffer + i) = 0;
  }
}

void* pageAllocator::getPage() {
  for (uint64_t i = 0; i < pageBitmap.size * 8; i++) {
    if (pageBitmap[i] == true) continue;
    lock((void*)(i * 4096));
    return (void*)(i * 4096);
  }

  return (void*)1; // TODO swap ram
}

void pageAllocator::free(void* addr) {
  uint64_t index = (uint64_t)addr / 4096;
  if (pageBitmap[index] == false) return;
  pageBitmap.set(index, false);
  freeMem += 4096;
  usedMem -= 4096;
}

void pageAllocator::frees(void* addr, uint64_t count) {
  for (int i = 0; i < count; i++) {
    free((void*)((uint64_t)addr + (i * 4096)));
  }
}

void pageAllocator::lock(void* addr) {
  uint64_t index = (uint64_t)addr / 4096;
  if (pageBitmap[index] == true) return;
  pageBitmap.set(index, true);
  freeMem -= 4096;
  usedMem += 4096;
}

void pageAllocator::locks(void* addr, uint64_t count) {
  for (int i = 0; i < count; i++) {
    lock((void*)((uint64_t)addr + (i * 4096)));
  }
}

uint64_t pageAllocator::getFreeMem() {
  return freeMem;
}

uint64_t pageAllocator::getUsedMem() {
  return usedMem;
}

uint64_t pageAllocator::getResdMem() {
  return reservedMem;
}

void pageAllocator::release(void* addr) {
  uint64_t index = (uint64_t)addr / 4096;
  if (pageBitmap[index] == false) return;
  pageBitmap.set(index, false);
  freeMem += 4096;
  reservedMem -= 4096;
}

void pageAllocator::releases(void* addr, uint64_t count) {
  for (int i = 0; i < count; i++) {
    release((void*)((uint64_t)addr + (i * 4096)));
  }
}

void pageAllocator::reserve(void* addr) {
  uint64_t index = (uint64_t)addr / 4096;
  if (pageBitmap[index] == true) return;
  pageBitmap.set(index, true);
  freeMem -= 4096;
  reservedMem += 4096;
}

void pageAllocator::reserves(void* addr, uint64_t count) {
  for (int i = 0; i < count; i++) {
    reserve((void*)((uint64_t)addr + (i * 4096)));
  }
}