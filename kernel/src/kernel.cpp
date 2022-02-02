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
  
  gop.printString(cur, "RedX OS", colors[0]);

  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }

  return;

}