#include "processor.h"

#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>
#include <ctime>

using namespace std::literals;
using clock_type = std::chrono::high_resolution_clock;

Processor::Processor() {
  PC = &R0;
  SP = &R1;
  SR = &R2;
  GC1 = &R2.val;
  GC2 = &R3;

  register_map.emplace(0, &R0);
  register_map.emplace(1, &R1);
  register_map.emplace(2, &R2.val);
  register_map.emplace(3, &R3);
  register_map.emplace(4, &R4);
  register_map.emplace(5, &R5);
  register_map.emplace(6, &R6);
  register_map.emplace(7, &R7);
  register_map.emplace(8, &R8);
  register_map.emplace(9, &R9);
  register_map.emplace(10, &R10);
  register_map.emplace(11, &R11);
  register_map.emplace(12, &R12);
  register_map.emplace(13, &R13);
  register_map.emplace(14, &R14);
  register_map.emplace(15, &R15);

  op_map.emplace("rrc", &Processor::op_rrc);
  op_map.emplace("swpb", &Processor::op_swpb);
  op_map.emplace("rra", &Processor::op_rra);
  op_map.emplace("sxt", &Processor::op_sxt);
  op_map.emplace("push", &Processor::op_push);
  op_map.emplace("call", &Processor::op_call);
  op_map.emplace("reti", &Processor::op_reti);
  op_map.emplace("jne", &Processor::op_jne_jnz);
  op_map.emplace("jeq", &Processor::op_jeq_jz);
  op_map.emplace("jnc", &Processor::op_jnc_jlo);
  op_map.emplace("jc", &Processor::op_jc_jhs);
  op_map.emplace("jn", &Processor::op_jn);
  op_map.emplace("jge", &Processor::op_jge);
  op_map.emplace("jle", &Processor::op_jle);
  op_map.emplace("jmp", &Processor::op_jmp);
  op_map.emplace("mov", &Processor::op_mov);
  op_map.emplace("add", &Processor::op_add);
  op_map.emplace("addc", &Processor::op_addc);
  op_map.emplace("subc", &Processor::op_subc);
  op_map.emplace("sub", &Processor::op_sub);
  op_map.emplace("cmp", &Processor::op_cmp);
  op_map.emplace("dadd", &Processor::op_dadd);
  op_map.emplace("bit", &Processor::op_bit);
  op_map.emplace("bic", &Processor::op_bic);
  op_map.emplace("bis", &Processor::op_bis);
  op_map.emplace("xor", &Processor::op_xor);
  op_map.emplace("and", &Processor::op_and);
}

void Processor::SetMemory(Memory* mem_ptr) {
  mem = mem_ptr;
  int_reset();
}

bool CheckBits(uint16_t a, uint16_t b, uint16_t bit) {
  auto v1 = (a >> bit) & 0xF;
  auto v2 = (b >> bit) & 0xF;
  return (v1 & v2);
}

// Processor::OP Processor::GetOpCodeFunc() {
//   for (auto opcode : op_map) {
//     if (current_instruction & opcode.first) {
//       if ((current_instruction >> 12) == 1) {
//         if (!CheckBits(current_instruction, opcode.first, 12)) {
//           continue;
//         }
//         if (!CheckBits(current_instruction, opcode.first, 8)) {
//           continue;
//         }
//         if (!CheckBits(current_instruction, opcode.first, 4)) {
//           continue;
//         }

//         auto val = current_instruction & 0xF000;
//         if (opcode.first < (current_instruction & 0xF000)) {
//           continue;
//         }
//         return opcode.second;
//       } else if (((current_instruction >> 12) == 2) ||
//                  ((current_instruction >> 12) == 3)) {
//         if (!CheckBits(current_instruction, opcode.first, 12)) {
//           continue;
//         }
//         if (!CheckBits(current_instruction, opcode.first, 8)) {
//           continue;
//         }

//         auto val = current_instruction & 0xFF00;
//         if (opcode.first < (current_instruction & 0xFF00)) {
//           continue;
//         }
//         return opcode.second;
//       }

//       auto val = current_instruction & 0xF000;
//       if (opcode.first < (current_instruction & 0xF000)) {
//         continue;
//       }
//       return opcode.second;
//     }
//   }
//   //std::cout << std::hex << "0x" << current_instruction << std::endl;
//   throw(ProcessorException("Undefined Opcode"));
// }

Processor::OP Processor::GetOpCodeFunc() {
  auto opcode = static_cast<uint8_t>(current_instruction >> 12);
  if (opcode == 0xF) {
    return op_map["and"];
  }
  if (opcode == 0xE) {
    return op_map["xor"];
  }
  if (opcode == 0xD) {
    return op_map["bis"];
  }
  if (opcode == 0xC) {
    return op_map["bic"];
  }
  if (opcode == 0xB) {
    return op_map["bit"];
  }
  if (opcode == 0xA) {
    return op_map["dadd"];
  }
  if (opcode == 0x9) {
    return op_map["cmp"];
  }
  if (opcode == 0x8) {
    return op_map["sub"];
  }
  if (opcode == 0x7) {
    return op_map["subc"];
  }
  if (opcode == 0x6) {
    return op_map["addc"];
  }
  if (opcode == 0x5) {
    return op_map["add"];
  }
  if (opcode == 0x4) {
    return op_map["mov"];
  }
  if (opcode == 0x3) {
    auto opcode_sec = (current_instruction >> 8) & 0x0F;
    if ((opcode_sec & 0x8) && (opcode_sec & 0x4)) {
      return op_map["jmp"];
    }
    if (opcode_sec & 0x8) {
      return op_map["jle"];
    }
    if (opcode_sec & 0x4) {
      return op_map["jge"];
    }
    if (opcode_sec & 0x0) {
      return op_map["jn"];
    }
  }
  if (opcode == 0x2) {
    auto opcode_sec = (current_instruction >> 8) & 0x0F;
    if ((opcode_sec & 0x8) && (opcode_sec & 0x4)) {
      return op_map["jc"];
    }
    if (opcode_sec & 0x8) {
      return op_map["jnc"];
    }
    if (opcode_sec & 0x4) {
      return op_map["jeq"];
    }
    if (opcode_sec & 0x0) {
      return op_map["jne"];
    }
  }
  if (opcode == 0x1) {
    auto val = current_instruction & 0x0FFF;
    if (val >= 0x300) {
      return op_map["reti"];
    }
    if (val >= 0x280) {
      return op_map["call"];
    }
    if (val >= 0x200) {
      return op_map["push"];
    }
    if (val >= 0x180) {
      return op_map["sxt"];
    }
    if (val >= 0x100) {
      return op_map["rra"];
    }
    if (val >= 0x080) {
      return op_map["swpb"];
    }
    if (val >= 0x0) {
      return op_map["rrc"];
    }
  }
  if (opcode == 0x0) {
    std::cout<<std::hex<<+opcode<<std::endl;
    throw(ProcessorException("Undefined Opcode"));
  }
  std::cout<<std::hex<<+opcode<<std::endl;
  throw(ProcessorException("Undefined Opcode"));
}

void PrintTime() {
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
auto duration = now.time_since_epoch();

typedef std::chrono::duration<int, std::ratio_multiply<std::chrono::hours::period, std::ratio<8>
>::type> Days; /* UTC: +8:00 */

Days days = std::chrono::duration_cast<Days>(duration);
    duration -= days;
auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    duration -= milliseconds;
auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    duration -= microseconds;
auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

std::cout <<std::dec<< hours.count() << ":"
          << minutes.count() << ":"
          << seconds.count() << ":"
          << milliseconds.count() << ":"
          << microseconds.count() << ":"
          << nanoseconds.count() << std::endl;
}

void Processor::Cycle() {
  std::cout <<"CYCLE"<<std::endl;
  int x = 0;
  while (true) {
    if (x == 800) {
      x = 0;
      Step();
      std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
    std::this_thread::sleep_for(std::chrono::microseconds(1));

    // PrintTime();
    // std::this_thread::sleep_for(std::chrono::microseconds(1));
    // PrintTime();
    // std::cout<<std::endl;
    x++;
  }
}

void Processor::Step() {
  // std::cout<<std::hex<<"PC: 0x"<<*PC<<std::endl;
  current_instruction = FetchInstruction(*PC);
  // //std::cout << "PC: 0x" << std::hex << *PC << " : 0x" <<
  // current_instruction
  //           << std::endl;
  (this->*GetOpCodeFunc())();
  // for(auto reg : register_map) {
  //   //std::cout << reg.first << ": " << *reg.second<<std::endl;
  // }
  // mem->DisplayMem();
  *PC += 2;
}

uint16_t Processor::FetchInstruction(uint16_t PC) {
  if (PC <= PERIPH_MAX) {
    throw(ProcessorException(
        "Tried to fetch instruction from peripheral address space"));
  }
  return (mem->GetUint16(PC));
}

void Processor::int_reset() {
  // Configure RST/NMI pin
  // Switch IO pins to input mode
  // Initialize peripherals

  // Set status register
  SR->val = 0;

  // Reset Program Counter
  *PC = mem->GetUint16(RESET_VECTOR);
}

ADDRESSING_MODE Processor::GetAddressingMode(REG reg, uint8_t ax) {
  if (reg == REG::AS) {
    if (ax == 0b00) {
      return ADDRESSING_MODE::REGISTER;
    }
    if (ax == 0b01) {
      return ADDRESSING_MODE::INDEXED;
    }
    if (ax == 0b01) {
      return ADDRESSING_MODE::SYMBOLIC;
    }
    if (ax == 0b01) {
      return ADDRESSING_MODE::ABSOLUTE;
    }
    if (ax == 0b10) {
      return ADDRESSING_MODE::INDIRECT_REG;
    }
    if (ax == 0b11) {
      return ADDRESSING_MODE::INDIRECT_AUTO;
    }
    if (ax == 0b11) {
      return ADDRESSING_MODE::IMMEDIATE;
    }
  }

  if (reg == REG::AD) {
    if (ax == 0b0) {
      return ADDRESSING_MODE::REGISTER;
    }
    if (ax == 0b1) {
      return ADDRESSING_MODE::INDEXED;
    }
    if (ax == 0b1) {
      return ADDRESSING_MODE::SYMBOLIC;
    }
    if (ax == 0b1) {
      return ADDRESSING_MODE::ABSOLUTE;
    }
  }
  return ADDRESSING_MODE::NONE;
}

bool Processor::CheckConstantGenerator(uint16_t reg, uint16_t as,
                                       uint16_t* val) {
  if ((reg == 2) && (as == 0b00)) {
    return false;
  }
  if ((reg == 2) && (as == 0b01)) {
    return false;
  }
  if ((reg == 2) && (as == 0b10)) {
    *val = 4;
    return true;
  }
  if ((reg == 2) && (as == 0b11)) {
    *val = 8;
    return true;
  }
  if ((reg == 3) && (as == 0b00)) {
    *val = 0;
    return true;
  }
  if ((reg == 3) && (as == 0b01)) {
    *val = 1;
    return true;
  }
  if ((reg == 3) && (as == 0b10)) {
    *val = 2;
    return true;
  }
  if ((reg == 3) && (as == 0b11)) {
    *val = 0xFFFF;
    return true;
  }
  return false;
}

std::pair<uint16_t, uint16_t> Processor::GetAsAd() {
  uint8_t as{};
  uint8_t ad{};

  uint16_t as_val{};
  uint16_t ad_val{};

  bool constant_generator = false;
  uint16_t constant;

  if (current_format == FORMAT::FORMAT1) {
    auto instruction = Format1();
    instruction.val = current_instruction;
    as = instruction.as;
    ad = instruction.ad;

    // std::cout<<"Source Reg: "<<+instruction.s_reg<<std::endl;
    // std::cout<<"AS: "<<+instruction.as<<std::endl;

    if (instruction.s_reg == 2) {
      constant_generator = CheckConstantGenerator(2, as, &constant);
    } else if (instruction.s_reg == 3) {
      constant_generator = CheckConstantGenerator(3, as, &constant);
    }

  } else if (current_format == FORMAT::FORMAT2) {
    auto instruction = Format2();
    instruction.val = current_instruction;
    as = 0;
    ad = instruction.ad;
  } else {
    return {};
  }
  auto as_mode = GetAddressingMode(REG::AS, as);
  auto ad_mode = GetAddressingMode(REG::AD, ad);

  // std::cout<<"ADDRESS_MODE: "<<GetModeString(as_mode)<<std::endl;
  // std::cout<<"ADDRESS_MODE: "<<GetModeString(ad_mode)<<std::endl;

  if (constant_generator) {
    // std::cout<<"CONSTANT"<<std::endl;
    as_val = constant;
  } else if ((as_mode == ADDRESSING_MODE::INDEXED) ||
             (as_mode == ADDRESSING_MODE::SYMBOLIC) ||
             (as_mode == ADDRESSING_MODE::ABSOLUTE) ||
             (as_mode == ADDRESSING_MODE::IMMEDIATE) ||
             (as_mode == ADDRESSING_MODE::INDIRECT_AUTO)) {
    *PC += 2;
    as_val = mem->GetUint16(*PC);
  }

  if ((ad_mode == ADDRESSING_MODE::INDEXED) ||
      (ad_mode == ADDRESSING_MODE::SYMBOLIC) ||
      (ad_mode == ADDRESSING_MODE::ABSOLUTE) ||
      (ad_mode == ADDRESSING_MODE::IMMEDIATE) ||
      (ad_mode == ADDRESSING_MODE::INDIRECT_AUTO)) {
    *PC += 2;
    ad_val = mem->GetUint16(*PC);
  }
  return std::pair<uint16_t, uint16_t>(as_val, ad_val);
}

std::string Processor::GetModeString(ADDRESSING_MODE addr) {
  if (addr == ADDRESSING_MODE::ABSOLUTE) {
    return "ABSOLUTE";
  }
  if (addr == ADDRESSING_MODE::IMMEDIATE) {
    return "IMMEDIATE";
  }
  if (addr == ADDRESSING_MODE::INDEXED) {
    return "INDEXED";
  }
  if (addr == ADDRESSING_MODE::INDIRECT_AUTO) {
    return "INDIRECT_AUTO";
  }
  if (addr == ADDRESSING_MODE::INDIRECT_REG) {
    return "INDIRECT_REG";
  }
  if (addr == ADDRESSING_MODE::REGISTER) {
    return "REGISTER";
  }
  if (addr == ADDRESSING_MODE::SYMBOLIC) {
    return "SYMBOLIc";
  }

  return "NONE";
}

Processor::~Processor() {}

void Processor::op_add() {
  // std::cout << "ADD" << std::endl;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  int16_t val;
  int32_t carry;
  bool store_mem = false;

  auto byte = (instruction.byte_word == 1) ? true : false;
  auto next = GetAsAd();

  src = &next.first;
  dst = &next.second;

  if (next.first == 0) {
    src = register_map[instruction.s_reg];
  }

  if (next.second == 0) {
    dst = register_map[instruction.d_reg];
  } else {
    store_mem = true;
  }

  SR->carry = 0;
  SR->negative = 0;
  SR->overflow = 0;
  SR->zero = 0;

  if (byte) {
    auto source = static_cast<int8_t>(*src);
    auto dest = static_cast<int8_t>(*dst);

    val = dest + source;
    carry = dest + source;

    if ((dest > 0) && (source > 0) && (val < 0)) {
      SR->overflow = 0;
    } else if ((dest < 0) && (source < 0) && (val > 0)) {
      SR->overflow = 0;
    }

    if (carry > 256) {
      SR->carry = 1;
    }

    if (store_mem) {
      mem->SetUint8(*dst, val);
    } else {
      *dst = val;
    }
  } else {
    auto source = static_cast<int16_t>(*src);
    auto dest = static_cast<int16_t>(*dst);

    val = dest + source;
    carry = dest + source;

    if ((dest > 0) && (source > 0) && (val < 0)) {
      SR->overflow = 0;
    } else if ((dest < 0) && (source < 0) && (val > 0)) {
      SR->overflow = 0;
    }

    if (carry > 65536) {
      SR->carry = 1;
    }

    if (store_mem) {
      mem->SetUint16(*dst, val);
    } else {
      *dst = val;
    }
  }

  if (val < 0) {
    SR->negative = 1;
  } else if (val == 0) {
    SR->zero = 1;
  }
};
void Processor::op_addc(){};
void Processor::op_and(){};
void Processor::op_bic(){};

void Processor::op_bis() {
  // std::cout << "BIT SET" << std::endl;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  bool store_mem = false;

  auto byte = (instruction.byte_word == 1) ? true : false;
  auto next = GetAsAd();

  src = &next.first;
  dst = &next.second;

  if (next.first == 0) {
    src = register_map[instruction.s_reg];
  }

  if (next.second == 0) {
    dst = register_map[instruction.d_reg];
  } else {
    store_mem = true;
  }

  if (byte) {
    if (store_mem) {
      auto val = *src | mem->GetUint16(*dst);
      mem->SetUint8(*dst, static_cast<uint8_t>(val));
    } else {
      auto val = *src | *dst;
      *dst = static_cast<uint8_t>(val);
    }
  } else {
    if (store_mem) {
      auto val = *src | mem->GetUint16(*dst);
      mem->SetUint16(*dst, val);
    } else {
      auto val = *src | *dst;
      *dst = val;
    }
  }
};

void Processor::op_bit(){};

void Processor::op_call() {
  // std::cout << "CALL" << std::endl;
  current_format = FORMAT::FORMAT2;
  auto instruction = Format2();
  uint16_t* dst;

  *SP = *SP - 2;
  *SP = *PC;

  auto next = GetAsAd();

  dst = &next.second;

  if (next.second == 0) {
    dst = register_map[instruction.ds_reg];
  }

  instruction.val = current_instruction;

  *PC = *dst;
};

void Processor::op_cmp() {
  // std::cout<<"CMP"<<std::endl;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  int16_t val;
  int32_t carry;
  bool store_mem = false;

  auto byte = (instruction.byte_word == 1) ? true : false;
  auto next = GetAsAd();

  src = &next.first;
  dst = &next.second;

  if (next.first == 0) {
    src = register_map[instruction.s_reg];
  }

  if (next.second == 0) {
    dst = register_map[instruction.d_reg];
  }

  SR->carry = 0;
  SR->negative = 0;
  SR->overflow = 0;
  SR->zero = 0;

  if (byte) {
    auto not_src = static_cast<uint8_t>(~*src);
    not_src += 1;
    auto val = static_cast<uint8_t>(*dst) + not_src;
    auto sign = static_cast<int8_t>(val);
    auto bit1 = *dst >> 7;
    auto bit2 = not_src >> 7;
    auto bit3 = val >> 7;

    if (val == 0) {
      SR->zero = 1;
    } else if (sign < 0) {
      SR->negative = 1;
    } else if ((bit1 == 1) && (bit2 == 1) && (bit3 == 0)) {
      SR->overflow = 1;
    } else if ((bit1 == 0) && (bit2 == 0) && (bit3 == 1)) {
      SR->overflow = 1;
    }
  } else {
    auto not_src = static_cast<uint16_t>(~*src);
    not_src += 1;
    auto val = *dst + not_src;
    // std::cout<<"0x"<<std::hex<<val<<std::endl;
    auto sign = static_cast<int16_t>(val);
    auto bit1 = *dst >> 15;
    auto bit2 = not_src >> 15;
    auto bit3 = val >> 15;

    if (val == 0) {
      SR->zero = 1;
    } else if (sign < 0) {
      SR->negative = 1;
    } else if ((bit1 == 1) && (bit2 == 1) && (bit3 == 0)) {
      SR->overflow = 1;
    } else if ((bit1 == 0) && (bit2 == 0) && (bit3 == 1)) {
      SR->overflow = 1;
    }
  }

  // if(byte) {
  //   auto source = static_cast<int8_t>(*src);
  //   auto dest = static_cast<int8_t>(*dst);

  //   val = dest - source;

  //   if((dest > 0) && ((!source + 1) > 0) && (val < 0)) {
  //     SR->overflow = 1;
  //   } else if ((dest < 0) && ((!source + 1) < 0) && (val > 0)) {
  //     SR->overflow = 1;
  //   }

  //   if(carry > 256) {
  //     SR->carry = 1;
  //   }
  // } else {
  //   auto source = static_cast<int16_t>(*src);
  //   auto dest = static_cast<int16_t>(*dst);

  //   val = dest - source;
  //   carry = dest + !source + 1;

  //   if((dest > 0) && ((!source + 1) > 0) && (val < 0)) {
  //     SR->overflow = 1;
  //   } else if ((dest < 0) && ((!source + 1) < 0) && (val > 0)) {
  //     SR->overflow = 1;
  //   }

  //   if(carry > 65536) {
  //     SR->carry = 1;
  //   }
  // }

  // if(val < 0) {
  //   SR->negative = 1;
  // } else if(val == 0) {
  //   SR->zero = 1;
  // }
};
void Processor::op_dadd(){};
void Processor::op_jc_jhs(){};
void Processor::op_jeq_jz(){};

void Processor::op_jge() {
  // std::cout<<"JGE"<<std::endl;
  current_format = FORMAT::JUMP;
  auto instruction = Jump();
  instruction.val = current_instruction;
  // std::cout<<+SR->negative<<" : "<<+SR->overflow<<std::endl;
  if ((SR->negative ^ SR->overflow) == 0) {
    *PC = *PC + 2 * instruction.offset;
  }
};

void Processor::op_jle(){};
void Processor::op_jmp() {
  // std::cout << "JUMP" << std::endl;
  current_format = FORMAT::JUMP;
  auto instruction = Jump();
  instruction.val = current_instruction;
  *PC = *PC + 2 * instruction.offset;
};
void Processor::op_jn(){};
void Processor::op_jnc_jlo(){};
void Processor::op_jne_jnz(){};

void Processor::op_mov() {
  // std::cout << "MOVE" << std::endl;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  bool store_mem = false;

  auto byte = (instruction.byte_word == 1) ? true : false;
  auto next = GetAsAd();

  src = &next.first;
  dst = &next.second;

  if (next.first == 0) {
    src = register_map[instruction.s_reg];
  }

  if (next.second == 0) {
    dst = register_map[instruction.d_reg];
  } else {
    store_mem = true;
  }

  if (byte) {
    if (store_mem) {
      mem->SetUint8(*dst, static_cast<uint8_t>(*src));
    } else {
      *dst = static_cast<uint8_t>(*src);
    }
  } else {
    if (store_mem) {
      mem->SetUint16(*dst, *src);
    } else {
      *dst = *src;
    }
  }
};

void Processor::op_push(){};
void Processor::op_push_b(){};
void Processor::op_reti(){};
void Processor::op_rra(){};
void Processor::op_rra_b(){};
void Processor::op_rrc(){};
void Processor::op_rrc_b(){};
void Processor::op_sub(){};
void Processor::op_subc(){};
void Processor::op_swpb(){};
void Processor::op_sxt(){};
void Processor::op_xor() {
  // std::cout<<"XOR"<<std::endl;
  current_format = FORMAT::FORMAT1;
  auto instruction = Format1();
  instruction.val = current_instruction;
  uint16_t* src;
  uint16_t* dst;
  bool store_mem = false;

  auto byte = (instruction.byte_word == 1) ? true : false;
  auto next = GetAsAd();

  src = &next.first;
  dst = &next.second;

  if (next.first == 0) {
    src = register_map[instruction.s_reg];
  }

  if (next.second == 0) {
    dst = register_map[instruction.d_reg];
  } else {
    store_mem = true;
  }

  if (byte) {
    if (store_mem) {
      auto val = *src ^ mem->GetUint16(*dst);
      mem->SetUint8(*dst, static_cast<uint8_t>(val));
    } else {
      auto val = *src ^ *dst;
      *dst = static_cast<uint8_t>(val);
    }
  } else {
    if (store_mem) {
      auto val = *src ^ mem->GetUint16(*dst);
      mem->SetUint16(*dst, val);
    } else {
      auto val = *src ^ *dst;
      *dst = val;
    }
  }
};
