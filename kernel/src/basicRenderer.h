#pragma once
#include <stddef.h>
#include "math.h"

typedef struct {
  void* BaseAddr;
  size_t Size;
  unsigned int Width;
  unsigned int Height;
  unsigned int ppsl; //Pixels per scan line
} Framebuffer;

typedef struct {
  unsigned char magic[2];
  unsigned char mode;
  unsigned char charsize;
} PSF_HEADER;

typedef struct {
  PSF_HEADER* header;
  void* buffer;
} PSF_FONT;

typedef struct {
  unsigned int x;
  unsigned int y;
  unsigned int xm;
  unsigned int ym;
} Cursor;

class basicRenderer {
  public:
  Framebuffer* buffer;
  PSF_FONT* font;
  void plotPixel(unsigned int x, unsigned int y, unsigned int color);
  void drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
  void rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
  void cls();
  void printChar(char chr, unsigned int x, unsigned int y, unsigned int color);
  void printString(Cursor* cursor, const char* str, unsigned int color);
};