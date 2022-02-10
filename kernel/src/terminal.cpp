#include "terminal.h"

CharDev vcs;
Terminal term;

void Terminal::prompt() {

}

bool leftShiftDown;
bool rightShiftDown;
bool mod;
void Terminal::update(bool isKBIntr, uint8_t scancode) {
  if (isKBIntr) {
    if (mod) {
      if (scancode == Up) {
        rendr.clChar(cur->x * 8, cur->y * 16, 0x00000000);
        rendr.putChar(*(char*)(cur->x + (cur->y * cur->xm) + (char*)vcs.bufferAddr), *(uint32_t*)((cur->x + (cur->y * cur->xm)) * 4 + (uint32_t*)vcs.colorAddr));
        cur->up();
        rendr.putChar('_', 0xffffffff);
      } else if (scancode == Down) {
        rendr.clChar(cur->x * 8, cur->y * 16, 0x00000000);
        rendr.putChar(*(char*)(cur->x + (cur->y * cur->xm) + (char*)vcs.bufferAddr), *(uint32_t*)((cur->x + (cur->y * cur->xm)) * 4 + (uint32_t*)vcs.colorAddr));
        cur->down();
        rendr.putChar('_', 0xffffffff);
      } else if (scancode == Left) {
        rendr.clChar(cur->x * 8, cur->y * 16, 0x00000000);
        rendr.putChar(*(char*)(cur->x + (cur->y * cur->xm) + (char*)vcs.bufferAddr), *(uint32_t*)((cur->x + (cur->y * cur->xm)) * 4 + (uint32_t*)vcs.colorAddr));
        cur->left();
        rendr.putChar('_', 0xffffffff);
      } else if (scancode == Right) {
        rendr.clChar(cur->x * 8, cur->y * 16, 0x00000000);
        rendr.putChar(*(char*)(cur->x + (cur->y * cur->xm) + (char*)vcs.bufferAddr), *(uint32_t*)((cur->x + (cur->y * cur->xm)) * 4 + (uint32_t*)vcs.colorAddr));
        cur->right();
        rendr.putChar('_', 0xffffffff);
      }
      mod = false;
      return;
    }
    switch (scancode) {
      case 0xe0:
        mod = true;
        return;
      case LeftShift:
        leftShiftDown = true;
        return;
      case LeftShift + 0x80:
        leftShiftDown = false;
        break;
      case RightShift:
        rightShiftDown = true;
        return;
      case RightShift + 0x80:
        rightShiftDown = false;
        break;
      case Enter:
        rendr.clChar(cur->x * 8, cur->y * 16, 0x00000000);
        cur->newLine();
        rendr.putChar('_', 0xffffffff);
        return;
      case BackSpace:
        rendr.clChar(cur->x * 8, cur->y * 16, 0x00000000);
        rendr.putChar(*(char*)(cur->x + (cur->y * cur->xm) + (char*)vcs.bufferAddr), *(uint32_t*)((cur->x + (cur->y * cur->xm)) * 4 + (uint32_t*)vcs.colorAddr));
        cur->back();
        rendr.delChar(cur->x * 8, cur->y * 16, 0x00000000);
        rendr.putChar('_', 0xffffffff);
        return;
      case Spacebar:
        rendr.clChar(cur->x * 8, cur->y * 16, 0x00000000);
        cur->right();
        rendr.putChar('_', 0xffffffff);
        return;
    }
    if (lookup(scancode, leftShiftDown | rightShiftDown) == 0) return;
    rendr.clChar(cur->x * 8, cur->y * 16, 0x00000000);
    rendr.printChar(lookup(scancode, leftShiftDown | rightShiftDown), 0xffffffff);
    rendr.putChar('_', 0xffffffff);
  } else {

  }
}

void Terminal::run() {
  
}