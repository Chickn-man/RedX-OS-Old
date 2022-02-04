#include <stdint.h>
#include "string.h"
#include "basicRenderer.h"
#include "memory/memory.h"
#include "memory/paging.h"

struct KernelParameters {
  Framebuffer* buffer;
  PSF_FONT* font;
  EFI_MEMORY_DESCRIPTOR* map;
  uint64_t mapSize;
  uint64_t descSize;
};

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" void main(KernelParameters* parameters) {
  
  /* var init */
  uint64_t mapEntries = parameters->mapSize / parameters->descSize;
  const unsigned int colors[] = {
    0xffff0000,
    0xffffc000,
    0xffffff00,
    0xff00ff00,
    0xff0000ff,
    0xffff00ff,
    0xffffffff,
  };

  /* basic gop renderer init */
  basicRenderer gop;
  gop.buffer = parameters->buffer;
  gop.font = parameters->font;

  /* Term cursor */
  cursor curT;
  curT.x = 0;
  curT.y = 0;
  curT.xm = roundd(parameters->buffer->ppsl / 8);
  curT.ym = roundd(parameters->buffer->Height / 16);
  cursor* cur = &curT;

  // Initialize allocator
  gop.printString(cur, "initializing page allocator", colors[6]);
  cur->newLine();
  allocator = pageAllocator();
  allocator.readMap(parameters->map, parameters->mapSize, parameters->descSize);

  // allocate ram for kernel
  gop.printString(cur, "allocating ram for kernel", colors[6]);
  cur->newLine();
  uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
  uint64_t kernelPages = (uint64_t)(kernelSize / 4096) + 1;
  allocator.locks(&_KernelStart, kernelPages);

  // Initialize page manager
  gop.printString(cur, "initializing page manager", colors[6]);
  cur->newLine();
  table* PML4 = (table*)allocator.getPage();
  set(PML4, 0, 0x1000);
  pTableMan pageTableMan = pTableMan(PML4);

  gop.printString(cur, "maping kernel memory", colors[6]);
  cur->newLine();
  for (uint64_t i = 0; i < getMemorySize(parameters->map, mapEntries, parameters->descSize); i += 0x1000) {
    pageTableMan.map((void*)i, (void*)i);
  }
  gop.printString(cur, "maping framebuffer memory", colors[6]);
  cur->newLine();
  allocator.locks(parameters->buffer->BaseAddr, parameters->buffer->Size / 0x1000 + 1);
  for (uint64_t i = (uint64_t)parameters->buffer->BaseAddr; i < (uint64_t)parameters->buffer->BaseAddr + (uint64_t)parameters->buffer->Size + 0x1000; i += 0x1000) {
    pageTableMan.map((void*)i, (void*)i);
  }

  asm ("mov %0, %%cr3" : : "r" (PML4));

  cur->reset();
  gop.cls();
  
  // RedX
  double xsize = 0.40;
  gop.drawLine(parameters->buffer->Width / 2 - parameters->buffer->Height * xsize, parameters->buffer->Height / 2 - parameters->buffer->Height * xsize, parameters->buffer->Width / 2 + parameters->buffer->Height * xsize, parameters->buffer->Height / 2 + parameters->buffer->Height * xsize, colors[0]);
  gop.drawLine(parameters->buffer->Width / 2 - parameters->buffer->Height * xsize, parameters->buffer->Height / 2 + parameters->buffer->Height * xsize, parameters->buffer->Width / 2 + parameters->buffer->Height * xsize, parameters->buffer->Height / 2 - parameters->buffer->Height * xsize, colors[0]);
  
  // Welcome
  gop.printString(cur, "Welcome to RedX OS!", colors[0]);

  // GOP info
  cur->newLine();
  cur->newLine();
  gop.printString(cur, "-GOP Info-", colors[4]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer Base Address: ", toHString((uint64_t)parameters->buffer->BaseAddr)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer Size: ", toHString((uint32_t)parameters->buffer->Size)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer Width: ", toString((uint64_t)parameters->buffer->Width)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer Height: ", toString((uint64_t)parameters->buffer->Height)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer Pixels Per Scan Line: ", toString((uint64_t)parameters->buffer->ppsl)), colors[6]);
  cur->newLine();
  
  // Print system info
  cur->newLine();
  gop.printString(cur, "-System Info-", colors[4]);
  cur->newLine();
toString((uint64_t)parameters->buffer->Height);
  gop.printString(cur, cat("Screen Resolution: ", toString((uint64_t)parameters->buffer->Width)), colors[6]);
  gop.printString(cur, "x", colors[6]);
  gop.printString(cur, toString((uint64_t)parameters->buffer->Height), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Ram Size: ", toString((double)(allocator.getFreeMem() + allocator.getUsedMem())  / 1073741824, 2)), colors[6]);
  gop.printString(cur, "GiB", colors[6]);
  cur->newLine();

  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}