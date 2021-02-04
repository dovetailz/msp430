#ifndef read_elf_h
#define read_elf_h

#include <elf.h>

#include <filesystem>
#include <map>
#include <string>

struct Section {
  uint16_t addr;
  uint16_t offset;
  uint16_t size;
};

class ElfReader {
 public:
  ElfReader();
  ElfReader(std::string filepath);
  ~ElfReader();

 private:
  std::map<std::string, Section> section_map;
};

class ElfReaderException : public std::exception {
  std::string _msg;

 public:
  ElfReaderException(const std::string& msg) : _msg(msg) {}

  virtual const char* what() const noexcept override { return _msg.c_str(); }
};

#endif