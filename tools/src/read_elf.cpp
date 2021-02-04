#include "read_elf.h"

#include <fstream>
#include <iostream>

ElfReader::ElfReader() {}

ElfReader::ElfReader(std::string filepath) {
  if (!std::filesystem::exists(filepath)) {
    throw(ElfReaderException("File does not exist"));
  }

  std::ifstream elf_file(filepath, std::ios::binary | std::ios::ate);
  std::streamsize size = elf_file.tellg();
  elf_file.seekg(0, std::ios::beg);

  Elf32_Ehdr header;
  if (elf_file.read((char*)&header, sizeof(Elf32_Ehdr))) {
    for (auto letter : header.e_ident) {
      std::cout << letter;
    }
  }
}

ElfReader::~ElfReader() {}
