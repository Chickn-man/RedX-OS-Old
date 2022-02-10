#pragma once
#include "memory/memory.h"
#include "string.h"
#include "basicRenderer.h"
#include "renderer.h"

extern const unsigned int colors[];

struct KernelParameters {
  Framebuffer* buffer;
  PSF_FONT* font;
  EFI_MEMORY_DESCRIPTOR* map;
  uint64_t mapSize;
  uint64_t descSize;
};

void welcome(KernelParameters* parameters);

void panic(const char* message);