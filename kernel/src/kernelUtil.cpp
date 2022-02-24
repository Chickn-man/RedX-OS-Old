#include "kernelUtil.h"

const unsigned int colors[] = {
  0xff000000,
  0xffff0000,
  0xffffc000,
  0xffffff00,
  0xff00ff00,
  0xff0000ff,
  0xffff00ff,
  0xffffffff,
};

KernelParameters* parameters;

void welcome() {
  // Red X
  double xsize = 0.40;
  rendr.drawLine(parameters->buffer->Width / 2 - parameters->buffer->Height * xsize, parameters->buffer->Height / 2 - parameters->buffer->Height * xsize, parameters->buffer->Width / 2 + parameters->buffer->Height * xsize, parameters->buffer->Height / 2 + parameters->buffer->Height * xsize, colors[1]);
  rendr.drawLine(parameters->buffer->Width / 2 - parameters->buffer->Height * xsize, parameters->buffer->Height / 2 + parameters->buffer->Height * xsize, parameters->buffer->Width / 2 + parameters->buffer->Height * xsize, parameters->buffer->Height / 2 - parameters->buffer->Height * xsize, colors[1]);
  
  // welcome
  rendr.printString("Welcome to RedX OS!", colors[1]);

  // GOP info
  cur.newLine();
  cur.newLine();
  rendr.printString("-GOP Info-", colors[5]);
  cur.newLine();
  rendr.printString(cat("Framebuffer Base Address: ", toHString((uint64_t)parameters->buffer->BaseAddr)), colors[7]);
  cur.newLine();
  rendr.printString(cat("Framebuffer Size: ", toHString((uint32_t)parameters->buffer->Size)), colors[7]);
  cur.newLine();
  rendr.printString(cat("Framebuffer Width: ", toString((uint64_t)parameters->buffer->Width)), colors[7]);
  cur.newLine();
  rendr.printString(cat("Framebuffer Height: ", toString((uint64_t)parameters->buffer->Height)), colors[7]);
  cur.newLine();
  rendr.printString(cat("Framebuffer Pixels Per Scan Line: ", toString((uint64_t)parameters->buffer->ppsl)), colors[7]);
  cur.newLine();
  
  // Print system info
  cur.newLine();
  rendr.printString( "-System Info-", colors[5]);
  cur.newLine();
  rendr.printString(cat("Screen Resolution: ", toString((uint64_t)parameters->buffer->Width)), colors[7]);
  rendr.printString("x", colors[7]);
  rendr.printString(toString((uint64_t)parameters->buffer->Height), colors[7]);
  cur.newLine();
  rendr.printString(cat("Ram Size: ", toString((double)(allocator.getFreeMem() + allocator.getUsedMem())  / 1073741824, 2)), colors[7]);
  rendr.printString("GiB", colors[7]);
  cur.newLine();
}

void panic(const char* message) {
  basicRender.cls();
  cur.reset();
  basicRender.printString( "[ ", 0xffffffff);
  basicRender.printString( "PANIC", 0xffff0000);
  basicRender.printString( " ] ", 0xffffffff);
  basicRender.printString(message, 0xffffffff);
}