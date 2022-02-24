#include "kernelUtil.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern "C" void main(KernelParameters* kParameters) {
  
  asm ("cli");

  /* var init */
  parameters = kParameters;
  uint64_t mapEntries = parameters->mapSize / parameters->descSize;
  VAI = pageAlign((void*)(getMemorySize(parameters->map, mapEntries, parameters->descSize) + parameters->buffer->Size));

  /* basic renderer init */
  basicRender.buffer = parameters->buffer;
  basicRender.font = parameters->font;
  rendr.buffer = parameters->buffer;
  rendr.font = parameters->font;

  /* Term cursor */
  cur.x = 0;
  cur.y = 0;
  cur.xm = roundd(parameters->buffer->ppsl / 8) - 1;
  cur.ym = roundd(parameters->buffer->Height / 16) - 1;

  mouse.x = parameters->buffer->ppsl / 2;
  mouse.y = parameters->buffer->Height / 2;
  mouse.xm = basicRender.buffer->ppsl - 1;
  mouse.ym = basicRender.buffer->Height - 1;

  // Load gdt
  basicRender.printString("loading gdt", colors[7]);
  cur.newLine();
  GDTDesc gdtDesc;
  gdtDesc.size = sizeof(GDT) - 1;
  gdtDesc.offset = (uint64_t)&globalDescrptorTable;
  loadGDT(&gdtDesc);

  // Initialize allocator
  basicRender.printString("initializing page allocator", colors[7]);
  cur.newLine();
  allocator = pageAllocator();
  allocator.readMap(parameters->map, parameters->mapSize, parameters->descSize);

  basicRender.printString("setting up interrupt handlers", colors[7]);
  cur.newLine();

  // allocate ram for kernel
  basicRender.printString("locking pages for kernel & frame buffer", colors[7]);
  cur.newLine();
  uint64_t kernelSize = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
  uint64_t kernelPages = (uint64_t)(kernelSize / 4096) + 1;
  allocator.locks(&_KernelStart, kernelPages);
  
  // Initialize page manager
  basicRender.printString("initializing page manager", colors[7]);
  cur.newLine();
  table* PML4 = (table*)allocator.getPage();
  set(PML4, 0, 0x1000);
  pTableMan pageTableMan = pTableMan(PML4);
  //pageTableMan.map((void*)PML4, (void*)PML4);

  basicRender.printString("maping memory", colors[7]);
  cur.newLine();
  /*for (uint64_t i = parameters->entryAddr + kernelSize + 0x1000; i < kernelSize + 0x1000; i += 0x1000) {
    pageTableMan.map((void*)i, (void*)i);
  }*/
  for (uint64_t i = 0; i < getMemorySize(parameters->map, mapEntries, parameters->descSize); i += 0x1000) {
    pageTableMan.map((void*)i, (void*)i);
  }
  basicRender.printString("maping framebuffer memory", colors[7]);
  cur.newLine();
  allocator.locks(parameters->buffer->BaseAddr, ((parameters->buffer->Size + 0x1000) / 0x1000) + 1);
  for (uint64_t i = (uint64_t)parameters->buffer->BaseAddr; i < (uint64_t)parameters->buffer->BaseAddr + (uint64_t)parameters->buffer->Size + 0x1000; i += 0x1000) {
    pageTableMan.map((void*)i, (void*)i);
  }

  basicRender.printString("loading page table", colors[7]);
  cur.newLine();
  asm ("mov %0, %%cr3" : : "r" (PML4));

  idtr.limit = 0x0fff;
  idtr.offset = (uint64_t)allocator.getPage();
  //pageTableMan.map((void*)idtr.offset, (void*)idtr.offset);

  vcs.size = (uint64_t)(cur.xm * cur.ym);
  vcs.bufferAddr = (char*)pageTableMan.getPool(vcs.size * sizeof(char));
  vcs.colorAddr = (uint32_t*)pageTableMan.getPool(vcs.size * sizeof(uint32_t));
  uint32_t mBuffer[256];

  ps2MouseInit();

  setGate((void*)pageFaultHandler, 0xe, intr, 0x08);
  setGate((void*)doubleFaultHandler, 0x8, intr, 0x08);
  setGate((void*)genProcFaultHandler, 0xd, intr, 0x08);
  setGate((void*)keyboardHandler, 0x21, intr, 0x08);
  setGate((void*)mouseHandler, 0x2c, trap, 0x08);

  asm ("lidt %0" : : "m" (idtr));

  basicRender.printString("remaping pic", colors[7]);
  cur.newLine();
  remapPIC();

  outb(PIC1_DATA, 0b11111001);
  outb(PIC2_DATA, 0b11101111);

  cur.reset();
  rendr.cls();
  
  // welcome
  welcome();

  asm ("sti");

  uint32_t x = mouse.x;
  uint32_t y = mouse.y;
  for (uint32_t a = 0; a < 16; a++) {
    for (uint32_t b = 0; b < 16; b++) {
      mBuffer[b + a * 16] = *(unsigned int*)((x + b) * 4 + ((y + a) * parameters->buffer->ppsl * 4) + (char*)parameters->buffer->BaseAddr);
    }
  }
  
  // halt cpu
  for (;;) {
    asm ("cli");

    processMPacket();

    for (uint32_t a = 0; a < 16; a++) {
      for (uint32_t b = 0; b < 16; b++) {
        *(unsigned int*)((x + b) * 4 + ((y + a) * parameters->buffer->ppsl * 4) + (char*)parameters->buffer->BaseAddr) = mBuffer[b + a * 16];
      }
    }

    for (uint32_t a = 0; a < 16; a++) {
      for (uint32_t b = 0; b < 16; b++) {
        mBuffer[b + a * 16] = *(unsigned int*)((mouse.x + b) * 4 + ((mouse.y + a) * parameters->buffer->ppsl * 4) + (char*)parameters->buffer->BaseAddr);
      }
    }
    //basicRender.plotPixel(mouse.x, mouse.y, 0xffffffff);
    basicRender.drawMouse(mouse.x, mouse.y, 0xffffffff);

    x = cur.x;
    y = cur.y;
    cur.x = cur.xm - 16;
    cur.y = cur.ym - 1;
    basicRender.rect(cur.x * 8, cur.y * 16, cur.xm * 8 + 7, cur.ym * 16 + 15, 0x00000000);
    basicRender.printString("mouse: ", 0xffffffff);
    basicRender.printString(toString((uint64_t)mouse.x), colors[7]);
    basicRender.printString(", ", colors[7]);
    basicRender.printString(toString((uint64_t)mouse.y), colors[7]);
    cur.x = cur.xm - 16;
    cur.y = cur.ym;
    basicRender.rect(cur.x * 8, cur.y * 16, cur.xm * 8 + 7, cur.ym * 16 + 15, 0x00000000);
    basicRender.printString("cursor: ", 0xffffffff);
    basicRender.printString(toString((uint64_t)x), colors[7]);
    basicRender.printString(", ", colors[7]);
    basicRender.printString(toString((uint64_t)y), colors[7]);
    cur.x = x;
    cur.y = y;
    
    x = mouse.x;
    y = mouse.y;
    asm ("sti");
    asm volatile("hlt");
  }
}