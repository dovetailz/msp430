#ifndef memory_h
#define memory_h

#include <cstdint>

typedef uint16_t MemAddr;

class Memory {
  Memory();
  ~Memory();

 public:
  uint8_t GetUint8(MemAddr addr);
  uint16_t GetUint16(MemAddr addr);

 private:
  uint8_t mem[0xFFFF];
};

#endif