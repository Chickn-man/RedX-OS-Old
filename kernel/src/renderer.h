#pragma once
#include <stddef.h>
#include "basicRenderer.h"
#include "math.h"
#include "terminal.h"

class Renderer {
  public:
  Framebuffer* buffer;
  PSF_FONT* font;
  void plotPixel(unsigned int x, unsigned int y, unsigned int color);
  void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
  void rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
  void cls();
  void fill(uint32_t color);
  void printChar(char chr, unsigned int x, unsigned int y, unsigned int color);
  void printChar(char chr, unsigned int color);
  void putChar(char chr, unsigned int color);
  void delChar(unsigned int x, unsigned int y, unsigned int color);
  void clChar(unsigned int x, unsigned int y, unsigned int color);
  void printString(const char* str, unsigned int color);
  void print(const char* str, unsigned int color);
};

extern Renderer rendr;