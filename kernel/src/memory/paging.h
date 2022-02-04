#pragma once
#include <stdint.h>
#include "memory.h"

extern uint8_t __stack_chk_fail;

struct dirEntry {
  bool P : 1; // present
  bool RW : 1; // read/write
  bool US : 1;  // user/super
  bool WT : 1; // write though
  bool CD : 1; // cache disabled
  bool A : 1; // accessed
  bool I0 : 1; // ignore 0
  bool LP : 1; // larger pages
  bool I1 : 1; // ignore 1
  uint8_t AV : 3; // availible
  uint64_t AD : 52; // address
};

struct table {
  dirEntry entries[512];
}__attribute__((aligned(0x1000)));

class pMapIndexer {
  public:
  pMapIndexer(uint64_t virtAddr);
  uint64_t PDP_i;
  uint64_t PD_i;
  uint64_t PT_i;
  uint64_t P_i;
};

class pTableMan {
  public:
  pTableMan(table* PML4 /*level four address*/);
  table* PML4;
  void map(void* V, void* P);
};