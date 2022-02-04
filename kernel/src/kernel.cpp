#include <stdint.h>
#include "string.h"
#include "basicRenderer.h"
#include "memory.h"

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
  
  /* init */
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
  Cursor cursor;
  cursor.x = 0;
  cursor.y = 0;
  cursor.xm = roundd(parameters->buffer->ppsl / 8);
  cursor.ym = roundd(parameters->buffer->Height / 16);
  Cursor* cur = &cursor;

  // Initialize allacator
  pageAllocator allocator;
  allocator.readMap(parameters->map, parameters->mapSize, parameters->descSize);

  // allocate ram for kernel
  uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
  uint64_t kernelPages = (uint64_t)kernelSize / 4096 + 1;
  allocator.locks(&_KernelStart, kernelPages);

  gop.cls();
  
  // RedX
  double xsize = 0.40;
  gop.drawLine(parameters->buffer->Width / 2 - parameters->buffer->Height * xsize, parameters->buffer->Height / 2 - parameters->buffer->Height * xsize, parameters->buffer->Width / 2 + parameters->buffer->Height * xsize, parameters->buffer->Height / 2 + parameters->buffer->Height * xsize, colors[0]);
  gop.drawLine(parameters->buffer->Width / 2 - parameters->buffer->Height * xsize, parameters->buffer->Height / 2 + parameters->buffer->Height * xsize, parameters->buffer->Width / 2 + parameters->buffer->Height * xsize, parameters->buffer->Height / 2 - parameters->buffer->Height * xsize, colors[0]);
  
  // Welcome
  gop.printString(cur, "Welcome to RedX OS!", colors[0]);
  /*for (uint8_t i = 0; i < cur->ym - 7; i++) {
    cur->newLine();
  }*/
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
  
  // Memory map info
  cur->newLine();
  gop.printString(cur, "-System Info-", colors[4]);
  cur->newLine();
  uint64_t mapEntries = parameters->mapSize / parameters->descSize;
toString((uint64_t)parameters->buffer->Height);
  gop.printString(cur, cat("Screen Resolution: ", toString((uint64_t)parameters->buffer->Width)), colors[6]);
  gop.printString(cur, cat("x", toString((uint64_t)parameters->buffer->Height)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Ram Size: ", toString(getMemorySize(parameters->map, mapEntries, parameters->descSize))), colors[6]);
  cur->newLine();

  /*gop.printString(cur, toString(allocator.getFreeMem() / 1024), colors[6]);
  cur->newLine();
  gop.printString(cur, toString(allocator.getUsedMem() / 1024), colors[6]);
  cur->newLine();
  gop.printString(cur, toString(allocator.getResdMem() / 1024), colors[6]);
  cur->newLine();*/

  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
  return;
}