#include <stddef.h>
#include <stdint.h>

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

double sqrt(double number);
int roundd(double x);
void cls(Framebuffer* buffer, unsigned int BPP);
void drawLine(Framebuffer* buffer, unsigned int BPP, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color);
void printChar(Framebuffer* buffer, PSF_FONT* font, char chr, unsigned int x, unsigned int y, unsigned int color);
void printString(Framebuffer* buffer, PSF_FONT* font, Cursor* cursor, const char* str, unsigned int color);
extern "C" void main(Framebuffer* framebuffer, PSF_FONT* font) {
  
  /* init */
  unsigned int BytesPerPixel = framebuffer->Size / (framebuffer->ppsl * framebuffer->Height);
  const unsigned int colors[] = {
    0xffff0000,
    0xffffc000,
    0xffffff00,
    0xff00ff00,
    0xff0000ff,
    0xffff00ff,
  };

  Cursor cursor;
  cursor.x = 0;
  cursor.y = 0;
  cursor.xm = roundd(framebuffer->ppsl / 8);
  cursor.ym = roundd(framebuffer->Height / 16);
  Cursor* cur = &cursor;
  
  // RedX
  double xsize = 0.40;
  drawLine(framebuffer, BytesPerPixel, framebuffer->Width / 2 - framebuffer->Height * xsize, framebuffer->Height / 2 - framebuffer->Height * xsize, framebuffer->Width / 2 + framebuffer->Height * xsize, framebuffer->Height / 2 + framebuffer->Height * xsize, colors[0]);
  drawLine(framebuffer, BytesPerPixel, framebuffer->Width / 2 - framebuffer->Height * xsize, framebuffer->Height / 2 + framebuffer->Height * xsize, framebuffer->Width / 2 + framebuffer->Height * xsize, framebuffer->Height / 2 - framebuffer->Height * xsize, colors[0]);
  
  printString(framebuffer, font, cur, "RedX OS", colors[0]);
  //cls(framebuffer, BytesPerPixel);
  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }

  return;

}

/*
** Plots a pixel at x, y.
** buffer is GOP frame buffer.
** BPP is bytes Per Pixel.
** Color is argb hex value.. (  aarrggbb)
**                           (0x00000000)
*/
void plotPixel(Framebuffer* buffer, unsigned int BPP, unsigned int x, unsigned int y, unsigned int color ) {
  if ((x * BPP + (y * buffer->ppsl * BPP) + (char*)buffer->BaseAddr) < (char*)buffer->BaseAddr + buffer->Size) { 
    *(unsigned int*)(x * BPP + (y * buffer->ppsl * BPP) + (char*)buffer->BaseAddr) = color;
  }
}

/*
** Draws a line from x1, y1 to x2, y2.
** buffer is GOP frame buffer.
** BPP is bytes Per Pixel.
** Color is argb hex value.. (  aarrggbb)
**                           (0x00000000)
*/
void drawLine(Framebuffer* buffer, unsigned int BPP, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  int a = y2 - y1;
  int b = x2 - x1;
  int length = sqrt(b *  b + a * a);
  for (double i = 0; i < roundd(length * 2); i++) {
    plotPixel(buffer, BPP,
      x1 + roundd(b * (i / roundd(length * 2))),
      y1 + roundd(a * (i / roundd(length * 2))),
      color);
  }
}

/*
** Clears the screen.
** buffer is GOP frame buffer.
** BPP is bytes Per Pixel.
*/
void cls(Framebuffer* buffer, unsigned int BPP) {
  for (int x = 0; x < buffer->Width; x++) {
    for (int y = 0; y < buffer->Height; y++) {
      plotPixel(buffer, BPP, x, y, 0x00ff0000);
    }
  }
}

/*
** Draws a solid rectangle from x1, y1 to x2, y2.
** buffer is GOP frame buffer.
** BPP is bytes Per Pixel.
** Color is argb hex value. (  aarrggbb)
**                          (0x00000000)
*/
void rect(Framebuffer* buffer, unsigned int BPP,  unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      plotPixel(buffer, BPP, x, y, color);
    }
  }
}

/*
** Puts a character in the frame buffer at x, y.
** buffer is GOP frame buffer.
** Color is argb hex value. (  aarrggbb)
**                          (0x00000000)
*/
void printChar(Framebuffer* buffer, PSF_FONT* font, char chr, unsigned int x, unsigned int y, unsigned int color) {
  unsigned int* pixPtr = (unsigned int*)buffer->BaseAddr;
  char* fontPtr = (char*)font->buffer + (chr * font->header->charsize);
  for (unsigned long u = y; u < y + 16; u++) {
    for (unsigned long c = x; c < x + 8; c++) {
      if ((*fontPtr & (0b10000000 >> (c - x))) > 0) {
        plotPixel(buffer, 4, c, u, color);
      }
    }
    fontPtr++;
  }
}

void printString(Framebuffer* buffer, PSF_FONT* font, Cursor* cursor, const char* str, unsigned int color) {
  char* chr = (char*)str; 
 
  while (*chr != 0) {
    printChar(buffer, font, *chr, cursor->x * 8, cursor->y * 16, color);
    cursor->x++;
    if (cursor->x > cursor->xm) {
      cursor->x = 0;
      if (cursor->y < cursor->ym) {
        cursor->y++;
      }
    }  
    chr++;
  }

}

// sqare root
double sqrt(double number) {
  double temp = 0;
  double root = number / 2;
  int i = 0;
  while (root != temp && i < 10000) {
    temp = root;
    root = (number / temp + temp) / 2;
    i++;
  }
  return root;
}

/*
** returns a random number between 1 and 0
*/
double rand() {
  return 0;
}

// round to int
int roundd(double x) {
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}
