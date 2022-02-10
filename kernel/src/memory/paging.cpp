#include "paging.h"

uint8_t __stack_chk_fail = 0;

pMapIndexer::pMapIndexer(uint64_t virtAddr) {
  virtAddr >>= 12;
  P_i = virtAddr & 0x1ff;
  virtAddr >>= 9;
  PT_i = virtAddr & 0x1ff;
  virtAddr >>= 9;
  PD_i = virtAddr & 0x1ff;
  virtAddr >>= 9;
  PDP_i = virtAddr & 0x1ff;
}

pTableMan::pTableMan(table* LFAD) {
  this->PML4 = LFAD;
}

void pTableMan::map(void* virtualMemory, void* physicalMemory){
  pMapIndexer indexer = pMapIndexer((uint64_t)virtualMemory);
  dirEntry PDE;

  PDE = PML4->entries[indexer.PDP_i];
  table* PDP;
  if (!PDE.P){
    PDP = (table*)allocator.getPage();
    set(PDP, 0, 0x1000);
    PDE.AD = (uint64_t)PDP >> 12;
    PDE.P = true;
    PDE.RW = true;
    PML4->entries[indexer.PDP_i] = PDE;
  }
  else
  {
    PDP = (table*)((uint64_t)PDE.AD << 12);
  }
  
  
  PDE = PDP->entries[indexer.PD_i];
  table* PD;
  if (!PDE.P){
    PD = (table*)allocator.getPage();
    set(PD, 0, 0x1000);
    PDE.AD = (uint64_t)PD >> 12;
    PDE.P = true;
    PDE.RW = true;
    PDP->entries[indexer.PD_i] = PDE;
  }
  else
  {
    PD = (table*)((uint64_t)PDE.AD << 12);
  }

  PDE = PD->entries[indexer.PT_i];
  table* PT;
  if (!PDE.P){
    PT = (table*)allocator.getPage();
    set(PT, 0, 0x1000);
    PDE.AD = (uint64_t)PT >> 12;
    PDE.P = true;
    PDE.RW = true;
    PD->entries[indexer.PT_i] = PDE;
  }
  else
  {
    PT = (table*)((uint64_t)PDE.AD << 12);
  }

  PDE = PT->entries[indexer.P_i];
  PDE.AD = (uint64_t)physicalMemory >> 12;
  PDE.P = true;
  PDE.RW = true;
  PT->entries[indexer.P_i] = PDE;
}

void* VAI;
void* pTableMan::getPool(uint64_t size) {
  void* ret = VAI;
  uint16_t pages = (uint16_t)((size / 4096) + 1);
  for (uint16_t i = 0; i < pages; i++) {
    map((void*)((uint64_t)VAI + (i * 4096)), allocator.getPage());
    VAI = (void*)((uint64_t)VAI + 4096);
  }
  return ret;
}