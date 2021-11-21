#include <stddef.h>
#include <stdint.h>

typedef struct {
  void* BaseAddr;
  size_t Size;
  unsigned int Width;
  unsigned int Height;
  unsigned int ppsl; //Pixels per scan line
} Framebuffer;

void main(Framebuffer* framebuffer) {
  unsigned int BytesPerPixel = 4;
  cls(framebuffer,  BytesPerPixel);
  for (unsigned int x = 0; x < framebuffer->Width / 2; x++) {
    plotPixel(framebuffer, BytesPerPixel, x, 50, 0xffffb000);
  }
  for (;;) {
    asm volatile("hlt");
  }
}

void plotPixel(Framebuffer* buffer, unsigned int BPP, unsigned int x, unsigned int y, unsigned int color) {
  *(unsigned int*)(x * BPP + (y * buffer->ppsl * BPP) + buffer->BaseAddr) = color;
}

void cls(Framebuffer* buffer, unsigned int BPP) {
  for (unsigned int x = 0; x < buffer->Width; x++) {
    for (unsigned int y = 0; y < buffer->Height; y++) {
      plotPixel(buffer, BPP, x, y, 0x00000000);
    }
  }
}
