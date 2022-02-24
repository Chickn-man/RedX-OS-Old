#include "renderer.h"

Renderer rendr;

/*
** Plots a pixel at x, y.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
** Color is argb hex value.. (  aarrggbb)
**                           (0x00000000)
*/
void Renderer::plotPixel(unsigned int x, unsigned int y, unsigned int color) {
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
void Renderer::drawLine(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  int a = y2 - y1;
  int b = x2 - x1;
  int length = sqrt(b *  b + a * a);
  for (double i = 0; i < (unsigned int)(length * 2); i++) {
    if (((x1 + (unsigned int)(b * (i / (unsigned int)(length * 2)))) * 4 + ((y1 + (unsigned int)(a * (i / (unsigned int)(length * 2)))) * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
      *(unsigned int*)((x1 + (unsigned int)(b * (i / (unsigned int)(length * 2)))) * 4 + ((y1 + (unsigned int)(a * (i / (unsigned int)(length * 2)))) * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
    }
  }
}

/*
** Draws a solid rectangle from x1, y1 to x2, y2.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
** Color is argb hex value. (  aarrggbb)
**                          (0x00000000)
*/
void Renderer::rect(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      if ((x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(unsigned int*)(x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
      }
    }
  }
}

/*
** Clears the screen.
** buffer is GOP frame buffer.
** 4 is bytes Per Pixel.
*/
void Renderer::cls() {
  for (int x = 0; x <= buffer->Width - 1; x++) {
    for (int y = 0; y <= buffer->Height - 1; y++) {
      if ((x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(uint32_t*)(x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = 0x00000000;
      }
    }
  }
  for (uint16_t i = 0; i < cur.xm * cur.ym; i++) {
    vcs.bufferAddr[i] = 0;
  }
}

void Renderer::fill(uint32_t color) {
  for (int x = 0; x <= buffer->Width - 1; x++) {
    for (int y = 0; y <= buffer->Height - 1; y++) {
      if ((x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(uint32_t*)(x * 4 + (y * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
      }
    }
  }
}

/*
** Puts a character in the frame buffer at x, y.
** buffer is GOP frame buffer.
** Color is argb hex value. (  aarrggbb)
**                          (0x00000000)
*/
void Renderer::printChar(char chr, unsigned int x, unsigned int y, unsigned int color) {
  if (chr == 0x00) return;
  unsigned int* pixPtr = (unsigned int*)buffer->BaseAddr;
  char* fontPtr = (char*)font->buffer + (chr * font->header->charsize);
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      if ((*fontPtr & (0b10000000 >> (c - x))) > 0) {
        if ((c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
          *(unsigned int*)(c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
        }
      }
    }
    fontPtr++;
  }
}

void Renderer::printChar(char chr, unsigned int color) {
  *(char*)(cur.x + (cur.y * (cur.xm + 1)) + (char*)vcs.bufferAddr) = chr;
  *(uint32_t*)((cur.x + (cur.y * (cur.xm + 1))) * 4 + (char*)vcs.colorAddr) = color;
  if (cur.x > cur.xm) {
    cur.newLine();
  }
  printChar(chr, cur.x * 8, cur.y * 16, color);
  cur.x++;
}

void Renderer::putChar(char chr, unsigned int color) {
  if (cur.x > cur.xm) {
    cur.newLine();
  }
  printChar(chr, cur.x * 8, cur.y * 16, color);
}

void Renderer::delChar(unsigned int x, unsigned int y, unsigned int color) {
  *(char*)(cur.x + (cur.y * (cur.xm + 1)) + (char*)vcs.bufferAddr) = 0x00;
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      if ((c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(unsigned int*)(c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
      }
    }
  }
}

void Renderer::clChar(unsigned int x, unsigned int y, unsigned int color) {
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      if ((c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
        *(unsigned int*)(c * 4 + (u * buffer->ppsl * 4) + (char*)buffer->BaseAddr) = color;
      }
    }
  }
}

void Renderer::printString(const char* str, unsigned int color) {
  char* chr = (char*)str;
  
  while (*chr != 0) {
    if (cur.x > cur.xm) {
      cur.newLine();
    }
    printChar(*chr, color);
    chr++;
  }

}

void Renderer::print(const char* str, unsigned int color) {
  char* chr = (char*)str;
  uint8_t length = 0;
  while (*chr != 0) {
    length++;
    chr++;
  }
  if (cur.xm - cur.x < length) {
    cur.newLine();
  }

  chr = (char*)str;
  while (*chr != 0) {
    if (cur.x > cur.xm) {
      cur.newLine();
    } 
    printChar(*chr, color);
    chr++;
  }

}