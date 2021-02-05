#include "memory.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
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
  ElfReader elf_reader(filepath);
  auto sections = elf_reader.GetSections();
  if (!sections.has_value()) {
    throw(ElfReaderException("No sections found in file"));
  }
  for (auto section : sections.value()) {
    std::cout << "Loading " << section.first << " into memory" << std::endl;

    std::ifstream elf_file(filepath, std::ios::binary | std::ios::ate);
    std::streamsize size = elf_file.tellg();
    auto mem_addr = section.second.sh_offset;
    auto mem_size = section.second.sh_size;
    elf_file.seekg(mem_addr, std::ios::beg);
    char* buffer = new char[mem_size];
    elf_file.read(buffer, mem_size);
    for (int x = 0; x < mem_size; x++) {
      mem[x + section.second.sh_addr] = buffer[x];
    }
  }
  //  for(int x = 0; x < MEM_SIZE; x+=2) {
  //   std::cout<<"0x"<<std::hex<<std::setfill('0')<<std::setw(4)<<std::right<<x<<
  //   ": ";
  //   std::cout<<std::hex<<std::setfill('0')<<std::setw(2)<<std::right<<+mem[x];
  //   std::cout<<std::hex<<std::setfill('0')<<std::setw(2)<<std::right<<+mem[x+1]
  //   << std::endl;;
  // }
}