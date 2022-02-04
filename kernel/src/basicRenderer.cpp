#include "basicRenderer.h"
#include "math.h"

void cursor::reset() {
  this->x = 0;
  this->y = 0;
}

void cursor::newLine() {
  if (this->y <= this->ym) {
    this->x = 0;
    this->y++;
  }
}

/*
** Plots a pixel at x, y.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
** Color is argb hex value.. (  aarrggbb)
**                           (0x00000000)
*/
void basicRenderer::plotPixel(unsigned int x, unsigned int y, unsigned int color) {
  if ((x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
    *(unsigned int*)(x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
  }
}

/*
** Draws a line from x1, y1 to x2, y2.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
** Color is argb hex value.. (  aarrggbb)
**                           (0x00000000)
*/
void basicRenderer::drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  int a = y2 - y1;
  int b = x2 - x1;
  int length = sqrt(b *  b + a * a);
  for (double i = 0; i < roundd(length * 2); i++) {
    plotPixel(x1 + roundd(b * (i / roundd(length * 2))), y1 + roundd(a * (i / roundd(length * 2))), color);
  }
}

/*
** Draws a solid rectangle from x1, y1 to x2, y2.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
** Color is argb hex value. (  aarrggbb)
**                          (0x00000000)
*/
void basicRenderer::rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      plotPixel(x, y, color);
    }
  }
}

/*
** Clears the screen.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
*/
void basicRenderer::cls() {
  rect(0, 0, buffer->Width, buffer->Height, 0x00000000);
}

/*
** Puts a character in the frame buffer at x, y.
** buffer is GOP frame buffer.
** Color is argb hex value. (  aarrggbb)
**                          (0x00000000)
*/
void basicRenderer::printChar(char chr, unsigned int x, unsigned int y, unsigned int color) {
  unsigned int* pixPtr = (unsigned int*)buffer->BaseAddr;
  char* fontPtr = (char*)font->buffer + (chr * font->header->charsize);
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      if ((*fontPtr & (0b10000000 >> (c - x))) > 0) {
        plotPixel(c, u, color);
      }
    }
    fontPtr++;
  }
}

void basicRenderer::printString(cursor* cur, const char* str, unsigned int color) {
  char* chr = (char*)str;
  
  while (*chr != 0) {
    if (cur->x > cur->xm) {
      cur->newLine();
    }
    printChar(*chr, cur->x * 8, cur->y * 16, color);
    cur->x++;
    chr++;
  }

}

void basicRenderer::print(cursor* cur, const char* str, unsigned int color) {
  char* chr = (char*)str;
  uint8_t length = 0;
  while (*chr != 0) {
    length++;
    chr++;
  }
  if (cur->xm - cur->x < length) {
    cur->newLine();
  }

  chr = (char*)str;
  while (*chr != 0) {
    if (cur->x > cur->xm) {
      cur->newLine();
    } 
    printChar(*chr, cur->x * 8, cur->y * 16, color);
    cur->x++;
    chr++;
  }

}