#include <stdint.h>
#include "string.h"
#include "basicRenderer.h"
#include "memory/memory.h"
#include "memory/paging.h"
#include "gdt/gdt.h"
#include "interrupts/interrupts.h"
#include "kernelUtil.h"
#include "io/io.h"
#include "renderer.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" void main(KernelParameters* parameters) {
  
  /* var init */
  uint64_t mapEntries = parameters->mapSize / parameters->descSize;
  VAI = (void*)(getMemorySize(parameters->map, mapEntries, parameters->descSize) + parameters->buffer->Size);

  /* basic renderer init */
  basicRender.buffer = parameters->buffer;
  basicRender.font = parameters->font;
  rendr.buffer = parameters->buffer;
  rendr.font = parameters->font;

  /* Term cursor */
  cursor curT;
  curT.x = 0;
  curT.y = 0;
  curT.xm = roundd(parameters->buffer->ppsl / 8) - 1;
  curT.ym = roundd(parameters->buffer->Height / 16) - 1;
  cur = &curT;

  // Load gdt
  basicRender.printString("loading gdt", colors[7]);
  cur->newLine();
  GDTDesc gdtDesc;
  gdtDesc.size = sizeof(GDT) - 1;
  gdtDesc.offset = (uint64_t)&globalDescrptorTable;
  loadGDT(&gdtDesc);

  // Initialize allocator
  basicRender.printString("initializing page allocator", colors[7]);
  cur->newLine();
  allocator = pageAllocator();
  allocator.readMap(parameters->map, parameters->mapSize, parameters->descSize);

  basicRender.printString("setting up interrupt handlers", colors[7]);
  cur->newLine();

  // allocate ram for kernel
  basicRender.printString("locking pages for kernel & frame buffer", colors[7]);
  cur->newLine();
  uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
  uint64_t kernelPages = (uint64_t)(kernelSize / 4096) + 1;
  allocator.locks(&_KernelStart, kernelPages);
  
  // Initialize page manager
  basicRender.printString("initializing page manager", colors[7]);
  cur->newLine();
  table* PML4 = (table*)allocator.getPage();
  set(PML4, 0, 0x1000);
  pTableMan pageTableMan = pTableMan(PML4);

  basicRender.printString("maping memory", colors[7]);
  cur->newLine();
  for (uint64_t i = 0; i < getMemorySize(parameters->map, mapEntries, parameters->descSize); i += 0x1000) {
    pageTableMan.map((void*)i, (void*)i);
  }
  basicRender.printString("maping framebuffer memory", colors[7]);
  allocator.locks(parameters->buffer->BaseAddr, ((parameters->buffer->Size + 0x1000) / 0x1000) + 1);
  for (uint64_t i = (uint64_t)parameters->buffer->BaseAddr; i < (uint64_t)parameters->buffer->BaseAddr + (uint64_t)parameters->buffer->Size + 0x1000; i += 0x1000) {
    pageTableMan.map((void*)i, (void*)i);
  }

  basicRender.printString("loading page table", colors[7]);
  cur->newLine();
  asm ("mov %0, %%cr3" : : "r" (PML4));

  IDTR idtr;
  idtr.limit = 0x0fff;
  idtr.offset = (uint64_t)allocator.getPage();
  IDTEntry* pageFault = (IDTEntry*)(idtr.offset + 0xe * sizeof(IDTEntry));
  pageFault->setOffset((uint64_t)pageFaultHandler);
  pageFault->type_attr = intr;
  pageFault->selector = 0x08;

  IDTEntry* doubleFault = (IDTEntry*)(idtr.offset + 0x8 * sizeof(IDTEntry));
  doubleFault->setOffset((uint64_t)doubleFaultHandler);
  doubleFault->type_attr = intr;
  doubleFault->selector = 0x08;

  IDTEntry* genProcFault = (IDTEntry*)(idtr.offset + 0xd * sizeof(IDTEntry));
  genProcFault->setOffset((uint64_t)genProcFaultHandler);
  genProcFault->type_attr = intr;
  genProcFault->selector = 0x08;

  IDTEntry* keyboardIntr = (IDTEntry*)(idtr.offset + 0x21 * sizeof(IDTEntry));
  keyboardIntr->setOffset((uint64_t)keyboardHandler);
  keyboardIntr->type_attr = intr;
  keyboardIntr->selector = 0x08;

  asm ("lidt %0" : : "m" (idtr));

  basicRender.printString("remaping pic", colors[7]);
  cur->newLine();
  remapPIC();

  outb(PIC1_DATA, 0b11111101);
  outb(PIC2_DATA, 0b11111101);
  
  asm ("sti");

  vcs.size = (uint64_t)(cur->xm * cur->ym);
  vcs.bufferAddr = (char*)&vcs.buffer;
  vcs.colorAddr = (uint32_t*)&vcs.color;
  //vcs.bufferAddr = (char*)pageTableMan.getPool(vcs.size);
  //vcs.colorAddr = (uint32_t*)pageTableMan.getPool(vcs.size * 4);

  cur->reset();
  rendr.cls();
  
  // welcome
  welcome(parameters);
  
  /*int* test = (int*)0x8000000000;
  *test = 2;*/
  
  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}