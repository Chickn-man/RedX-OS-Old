#pragma once
#include <stddef.h>
#include "math.h"

struct Framebuffer {
  void* BaseAddr;
  size_t Size;
  unsigned int Width;
  unsigned int Height;
  unsigned int ppsl; //Pixels per scan line
};

struct PSF_HEADER {
  unsigned char magic[2];
  unsigned char mode;
  unsigned char charsize;
};

struct PSF_FONT {
  PSF_HEADER* header;
  void* buffer;
};

class cursor {
  public:
  unsigned int x;
  unsigned int y;
  unsigned int xm;
  unsigned int ym;
  void reset();
  void newLine();
};

class basicRenderer {
  public:
  Framebuffer* buffer;
  PSF_FONT* font;
  void plotPixel(unsigned int x, unsigned int y, unsigned int color);
  void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
  void rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
  void cls();
  void printChar(char chr, unsigned int x, unsigned int y, unsigned int color);
  void printString(cursor* cur, const char* str, unsigned int color);
  void print(cursor* cur, const char* str, unsigned int color);
};