#include "memory.h"

#include <algorithm>
#include <filesystem>
#include <iostream>

#include "read_elf.h"

Memory::Memory() {
  for (uint32_t addr = 0; addr < Memory::MEM_SIZE; addr++) {
    mem[addr] = 0xFF;
  }
}

Memory::~Memory() {}

uint8_t Memory::GetUint8(MemAddr addr) { return mem[addr]; }

uint16_t Memory::GetUint16(MemAddr addr) {
  uint16_t val = static_cast<uint16_t>(mem[addr]) << 8;
  val = val | static_cast<uint16_t>(mem[addr + 1]);
  return val;
}

void Memory::SetUint8(MemAddr addr, uint8_t val) { mem[addr] = val; }

void Memory::SetUint16(MemAddr addr, uint16_t val) {
  CheckBounds(addr);
  auto msb = val >> 8;
  auto lsb = val & 0x00FF;
  mem[addr] = msb;
  mem[addr + 1] = lsb;
}

void Memory::CheckBounds(MemAddr addr) {
  if (addr % 2 == 0) {
    return;
  }
  std::string error = std::to_string(addr);
  error += " is not word aligned";
  throw MemoryException(error);
}

void Memory::LoadFile(std::string filepath) {
  try {
    ElfReader elf_reader(filepath);
  } catch (ElfReaderException& e) {
    std::cout << e.what() << std::endl;
  }
}