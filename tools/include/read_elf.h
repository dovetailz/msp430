#ifndef read_elf_h
#define read_elf_h

#include <elf.h>

#include <filesystem>
#include <map>
#include <optional>
#include <string>
#include <vector>

class ElfReader {
 public:
  ElfReader();
  ElfReader(std::string filepath);
  ~ElfReader();

  Elf32_Ehdr header;
  Elf32_Shdr section_header;
  std::optional<Elf32_Shdr> symbol_section;
  Elf32_Sym symbol;

 private:
  std::vector<Elf32_Shdr> sections;
  std::vector<Elf32_Sym> symbols;
  std::map<std::string, Elf32_Shdr> section_map;
  uint8_t magic[4]{0x7f, 0x45, 0x4c, 0x46};
};

class ElfReaderException : public std::exception {
  std::string _msg;

 public:
  ElfReaderException(const std::string& msg) : _msg(msg) {}

  virtual const char* what() const noexcept override { return _msg.c_str(); }
};

#endif