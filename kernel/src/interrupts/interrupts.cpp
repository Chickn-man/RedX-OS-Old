#include "interrupts.h"

void IDTEntry::setOffset(uint64_t offset){
    offset0 = (uint16_t)(offset & 0x000000000000ffff);
    offset1 = (uint16_t)((offset & 0x00000000ffff0000) >> 16);
    offset2 = (uint32_t)((offset & 0xffffffff00000000) >> 32);
}

uint64_t IDTEntry::getOffset(){ 
    uint64_t offset = 0;
    offset |= (uint64_t)offset0;
    offset |= (uint64_t)offset1 << 16;
    offset |= (uint64_t)offset2 << 32;
    return offset;
}
__attribute__((interrupt)) void pageFaultHandler(struct interruptFrame* frame) {
  panic("Page Fault");
  
  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}

__attribute__((interrupt)) void doubleFaultHandler(struct interruptFrame* frame) {
  panic("Double Fault");

  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}

__attribute__((interrupt)) void genProcFaultHandler(struct interruptFrame* frame) {
  panic("General Protection Fault");

  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}

__attribute__((interrupt)) void keyboardHandler(struct interruptFrame* frame) {
  uint8_t scancode = inb(0x60);
  //rendr.printString(toHString(scancode), 0xffffffff);
  term.update(true, scancode);
  endMaster();
}

void remapPIC(){
    uint8_t a1, a2; 

    a1 = inb(PIC1_DATA);
    ioWait();
    a2 = inb(PIC2_DATA);
    ioWait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    ioWait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    ioWait();

    outb(PIC1_DATA, 0x20);
    ioWait();
    outb(PIC2_DATA, 0x28);
    ioWait();

    outb(PIC1_DATA, 4);
    ioWait();
    outb(PIC2_DATA, 2);
    ioWait();

    outb(PIC1_DATA, ICW4_8086);
    ioWait();
    outb(PIC2_DATA, ICW4_8086);
    ioWait();

    outb(PIC1_DATA, a1);
    ioWait();
    outb(PIC2_DATA, a2);
}

void endMaster() {
  outb(PIC1_COMMAND, PIC_EOI);
}

void endSlave() {
  outb(PIC2_COMMAND, PIC_EOI);
  outb(PIC1_COMMAND, PIC_EOI);
}