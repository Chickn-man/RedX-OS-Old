#include <stddef.h>
#include <stdint.h>
#include "math.h"
#include "basicRenderer.h"

extern "C" void main(Framebuffer* framebuffer, PSF_FONT* font) {
  
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
  gop.buffer = framebuffer;
  gop.font = font;

  /* Term cursor */
  Cursor cursor;
  cursor.x = 0;
  cursor.y = 0;
  cursor.xm = roundd(framebuffer->ppsl / 8);
  cursor.ym = roundd(framebuffer->Height / 16);
  Cursor* cur = &cursor;

  gop.cls();
  
  // RedX
  double xsize = 0.40;
  gop.drawLine(framebuffer->Width / 2 - framebuffer->Height * xsize, framebuffer->Height / 2 - framebuffer->Height * xsize, framebuffer->Width / 2 + framebuffer->Height * xsize, framebuffer->Height / 2 + framebuffer->Height * xsize, colors[0]);
  gop.drawLine(framebuffer->Width / 2 - framebuffer->Height * xsize, framebuffer->Height / 2 + framebuffer->Height * xsize, framebuffer->Width / 2 + framebuffer->Height * xsize, framebuffer->Height / 2 - framebuffer->Height * xsize, colors[0]);
  
  // Welcome
  gop.printString(cur, "Welcome to RedX OS!", colors[0]);
  /*for (uint8_t i = 0; i < cur->ym - 7; i++) {
    cur->newLine();
  }*/
  // GOP info
  cur->newLine();
  cur->newLine();
  gop.printString(cur, "-GOP info-", colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer base address: ", toHString((uint64_t)framebuffer->BaseAddr)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer size: ", toHString((uint32_t)framebuffer->Size)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer width: ", toString((uint64_t)framebuffer->Width)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer height: ", toString((uint64_t)framebuffer->Height)), colors[6]);
  cur->newLine();
  gop.printString(cur, cat("Framebuffer pixels per scan line: ", toString((uint64_t)framebuffer->ppsl)), colors[6]);
  cur->newLine();
  
  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
  return;
}