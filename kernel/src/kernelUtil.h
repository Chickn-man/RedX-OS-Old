#pragma once
#include "string.h"
#include "basicRenderer.h"
#include "renderer.h"
#include <stdint.h>
#include "memory/memory.h"
#include "memory/paging.h"
#include "gdt/gdt.h"
#include "interrupts/interrupts.h"
#include "io/io.h"
#include "io/hid/mouse.h"

extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

extern const unsigned int colors[];

struct KernelParameters {
  Framebuffer* buffer;
  PSF_FONT* font;
  EFI_MEMORY_DESCRIPTOR* map;
  uint64_t mapSize;
  uint64_t descSize;
  uint64_t entryAddr;
};

void welcome();

void panic(const char* message);

extern KernelParameters* parameters;