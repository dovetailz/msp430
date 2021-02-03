#include "memory.h"

uint8_t Memory::GetUint8(MemAddr addr) { return mem[addr]; }

uint16_t Memory::GetUint16(MemAddr addr) {
  uint16_t val = static_cast<uint16_t>(mem[addr]) << 8;
  val |= mem[addr + 1];
  return mem[addr];
}