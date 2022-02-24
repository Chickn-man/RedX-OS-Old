#include "keyboard.h"

const char ascii[] = {
   0 ,  0 , '1', '2',
  '3', '4', '5', '6',
  '7', '8', '9', '0',
  '-', '=',  0 ,  0 ,
  'q', 'w', 'e', 'r',
  't', 'y', 'u', 'i',
  'o', 'p', '[', ']',
   0 ,  0 , 'a', 's',
  'd', 'f', 'g', 'h',
  'j', 'k', 'l', ';',
  '\'','`',  0 , '\\',
  'z', 'x', 'c', 'v',
  'b', 'n', 'm', ',',
  '.', '/',  0 , '*',
   0 , ' '
};

char lookup(uint8_t scancode, bool uppercase) {
  if (scancode > 58) return 0;
  
  if (uppercase) {
    switch (ascii[scancode]) {
      case '0':
        return ascii[scancode] - 7;
      case '2':
        return ascii[scancode] + 14;
      case '6':
        return ascii[scancode] + 40;
      case '8':
        return ascii[scancode] - 14;
      case '`':
        return ascii[scancode] + 30;
      case ';':
        return ascii[scancode] - 1;
      case '\'':
        return ascii[scancode] - 17;
    }

    if (scancode < 7) return ascii[scancode] - 16;

    if (scancode < 11) return ascii[scancode] - 17;

    return ascii[scancode] - 32;
  }
  return ascii[scancode];
}

/*bool leftShiftDown;
bool rightShiftDown;
bool mod;
void handleKeyBoard(uint8_t scancode) {
  if (mod) {
    if (scancode == Up) {
      rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
      cur.up();
      rendr.printChar('_', 0xffffffff);
      cur.left();
    } else if (scancode == Down) {
      rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
      cur.down();
      rendr.printChar('_', 0xffffffff);
      cur.left();
    } else if (scancode == Left) {
      rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
      cur.left();
      rendr.printChar('_', 0xffffffff);
      cur.left();
    } else if (scancode == Right) {
      rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
      cur.right();
      rendr.printChar('_', 0xffffffff);
      cur.left();
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
      rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
      cur.newLine();
      rendr.printChar('_', 0xffffffff);
      cur.left();
      return;
    case BackSpace:
      rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
      cur.back();
      rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
      rendr.printChar('_', 0xffffffff);
      cur.left();
      return;
    case Spacebar:
      rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
      cur.right();
      rendr.printChar('_', 0xffffffff);
      cur.left();
      return;
  }

  if (lookup(scancode, leftShiftDown | rightShiftDown) == 0) return;
  rendr.delChar(cur.x * 8, cur.y * 16, 0x00000000);
  rendr.printChar(lookup(scancode, leftShiftDown | rightShiftDown), 0xffffffff);
  rendr.printChar('_', 0xffffffff);
  cur.left();
}*/