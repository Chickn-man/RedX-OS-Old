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
        basicRender.clChar(cur.x * 8, cur.y * 16, 0x00000000);
        basicRender.putChar(*(char*)(cur.x + (cur.y * (cur.xm + 1)) + (char*)vcs.bufferAddr), *(uint32_t*)((cur.x + (cur.y * (cur.xm + 1))) * 4 + (char*)vcs.colorAddr));
        cur.up();
        basicRender.putChar('_', 0xffffffff);
      } else if (scancode == Down) {
        basicRender.clChar(cur.x * 8, cur.y * 16, 0x00000000);
        basicRender.putChar(*(char*)(cur.x + (cur.y * (cur.xm + 1)) + (char*)vcs.bufferAddr), *(uint32_t*)((cur.x + (cur.y * (cur.xm + 1))) * 4 + (char*)vcs.colorAddr));
        cur.down();
        basicRender.putChar('_', 0xffffffff);
      } else if (scancode == Left) {
        basicRender.clChar(cur.x * 8, cur.y * 16, 0x00000000);
        basicRender.putChar(*(char*)(cur.x + (cur.y * (cur.xm + 1)) + (char*)vcs.bufferAddr), *(uint32_t*)((cur.x + (cur.y * (cur.xm + 1))) * 4 + (char*)vcs.colorAddr));
        cur.left();
        basicRender.putChar('_', 0xffffffff);
      } else if (scancode == Right) {
        basicRender.clChar(cur.x * 8, cur.y * 16, 0x00000000);
        basicRender.putChar(*(char*)(cur.x + (cur.y * (cur.xm + 1)) + (char*)vcs.bufferAddr), *(uint32_t*)((cur.x + (cur.y * (cur.xm + 1))) * 4 + (char*)vcs.colorAddr));
        cur.right();
        basicRender.putChar('_', 0xffffffff);
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
        basicRender.clChar(cur.x * 8, cur.y * 16, 0x00000000);
        cur.newLine();
        basicRender.putChar('_', 0xffffffff);
        return;
      case BackSpace:
        basicRender.clChar(cur.x * 8, cur.y * 16, 0x00000000);
        basicRender.putChar(*(char*)(cur.x + (cur.y * (cur.xm + 1)) + (char*)vcs.bufferAddr), *(uint32_t*)((cur.x + (cur.y * (cur.xm + 1))) * 4 + (char*)vcs.colorAddr));
        cur.left();
        rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
        basicRender.putChar('_', 0xffffffff);
        return;
      case Spacebar:
        basicRender.clChar(cur.x * 8, cur.y * 16, 0x00000000);
        cur.right();
        basicRender.putChar('_', 0xffffffff);
        return;
    }
    if (lookup(scancode, leftShiftDown | rightShiftDown) == 0) return;
    basicRender.clChar(cur.x * 8, cur.y * 16, 0x00000000);
    rendr.printChar(lookup(scancode, leftShiftDown | rightShiftDown), 0xffffffff);
    basicRender.putChar('_', 0xffffffff);
  } else {

  }
}

void Terminal::run() {
  
}