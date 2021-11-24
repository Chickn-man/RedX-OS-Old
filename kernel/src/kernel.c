#include <stddef.h>
#include <stdint.h>

typedef struct {
  void* BaseAddr;
  size_t Size;
  unsigned int Width;
  unsigned int Height;
  unsigned int ppsl; //Pixels per scan line
} Framebuffer;

double sqrt(double number);
int roundd(double x);

void main(Framebuffer* framebuffer) {
  unsigned int BytesPerPixel = 4;
  const unsigned int colors[] = {
    0xffff0000,
    0xffffc000,
    0xffffff00,
    0xff00ff00,
    0xff0000ff,
    0xffff00ff,
  };
  //cls(framebuffer,  BytesPerPixel);
  //rect(framebuffer, BytesPerPixel, 0, 0, 960, 540, 0x00000000);
  drawLine(framebuffer, BytesPerPixel, 0, 0, framebuffer->Width - 1, framebuffer->Height - 1, colors[0]);
  drawLine(framebuffer, BytesPerPixel, 0, framebuffer->Height - 1, framebuffer->Width - 1, 0, colors[0]);
  //halt cpu
  for (;;) {
    asm volatile("hlt");
  }
}

void plotPixel(Framebuffer* buffer, unsigned int BPP, unsigned int x, unsigned int y, unsigned int color) {
  *(unsigned int*)(x * BPP + (y * buffer->ppsl * BPP) + buffer->BaseAddr) = color;
}

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

void cls(Framebuffer* buffer, unsigned int BPP) {
  for (int x = 0; x < roundd(buffer->Width); x++) {
    for (int y = 0; y < roundd(buffer->Height); y++) {
      plotPixel(buffer, BPP, x, y, 0x00000000);
    }
  }
}

void rect(Framebuffer* buffer, unsigned int BPP,  unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int color) {
  for (int x = x1; x <= x2; x++) {
    for (int y = y1; y <= y2; y++) {
      plotPixel(buffer, BPP, x, y, color);
    }
  }
}
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

double rand() {
  return 0;
}

int roundd(double x) {
    if (x < 0.0)
        return (int)(x - 0.5);
    else
        return (int)(x + 0.5);
}