#pragma once
#include <stdint.h>
#include "basicRenderer.h"
#include "io/hid/keyboard.h"
#include "renderer.h"

struct CharDev {
  char* bufferAddr;
  uint32_t* colorAddr;
  uint16_t size;
};

class Terminal {
  public:
  char cmd[64];
  void prompt();
  void update(bool isKBIntr, uint8_t scancode);
  void run();
};

extern CharDev vcs;
extern Terminal term;