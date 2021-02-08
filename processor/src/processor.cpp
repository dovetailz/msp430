#include "processor.h"

#include <iostream>

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

  op_map.emplace(0x1000, &Processor::op_rrc);
  op_map.emplace(0x1040, &Processor::op_rrc_b);
  op_map.emplace(0x1080, &Processor::op_swpb);
  op_map.emplace(0x1100, &Processor::op_rra);
  op_map.emplace(0x1140, &Processor::op_rra_b);
  op_map.emplace(0x1180, &Processor::op_sxt);
  op_map.emplace(0x1200, &Processor::op_push);
  op_map.emplace(0x1240, &Processor::op_push_b);
  op_map.emplace(0x1280, &Processor::op_call);
  op_map.emplace(0x1300, &Processor::op_reti);
  op_map.emplace(0x2000, &Processor::op_jne_jnz);
  op_map.emplace(0x2400, &Processor::op_jeq_jz);
  op_map.emplace(0x2800, &Processor::op_jnc_jlo);
  op_map.emplace(0x2C00, &Processor::op_jc_jhs);
  op_map.emplace(0x3000, &Processor::op_jn);
  op_map.emplace(0x3400, &Processor::op_jge);
  op_map.emplace(0x3800, &Processor::op_jle);
  op_map.emplace(0x3C00, &Processor::op_jmp);
  op_map.emplace(0x4000, &Processor::op_mov);
  op_map.emplace(0x5000, &Processor::op_add);
  op_map.emplace(0x6000, &Processor::op_addc);
  op_map.emplace(0x7000, &Processor::op_subc);
  op_map.emplace(0x8000, &Processor::op_sub);
  op_map.emplace(0x9000, &Processor::op_cmp);
  op_map.emplace(0xA000, &Processor::op_dadd);
  op_map.emplace(0xB000, &Processor::op_bit);
  op_map.emplace(0xC000, &Processor::op_bic);
  op_map.emplace(0xD000, &Processor::op_bis);
  op_map.emplace(0xE000, &Processor::op_xor);
  op_map.emplace(0xF000, &Processor::op_and);
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

Processor::OP Processor::GetOpCodeFunc() {
  for (auto opcode : op_map) {
    if (current_instruction & opcode.first) {
      if ((current_instruction >> 12) == 1) {
        if (!CheckBits(current_instruction, opcode.first, 12)) {
          continue;
        }
        if (!CheckBits(current_instruction, opcode.first, 8)) {
          continue;
        }
        if (!CheckBits(current_instruction, opcode.first, 4)) {
          continue;
        }

        auto val = current_instruction & 0xF000;
        if (opcode.first < (current_instruction & 0xF000)) {
          continue;
        }
        return opcode.second;
      } else if (((current_instruction >> 12) == 2) ||
                 ((current_instruction >> 12) == 3)) {
        if (!CheckBits(current_instruction, opcode.first, 12)) {
          continue;
        }
        if (!CheckBits(current_instruction, opcode.first, 8)) {
          continue;
        }

        auto val = current_instruction & 0xFF00;
        if (opcode.first < (current_instruction & 0xFF00)) {
          continue;
        }
        return opcode.second;
      }

      auto val = current_instruction & 0xF000;
      if (opcode.first < (current_instruction & 0xF000)) {
        continue;
      }
      return opcode.second;
    }
  }
  std::cout << std::hex << "0x" << current_instruction << std::endl;
  throw(ProcessorException("Undefined Opcode"));
}

void Processor::Step() {
  current_instruction = FetchInstruction(*PC);
  std::cout << "PC: 0x" << std::hex << *PC << " : 0x" << current_instruction
            << std::endl;
  (this->*GetOpCodeFunc())();
  // for(auto reg : register_map) {
  //   std::cout << reg.first << ": " << *reg.second<<std::endl;
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
    *val = 0;
    return true;
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

  if (constant_generator) {
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

void Processor::op_add() { std::cout << "ADD" << std::endl; };
void Processor::op_addc(){};
void Processor::op_and(){};
void Processor::op_bic(){};

void Processor::op_bis() {
  std::cout << "BIT SET" << std::endl;
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
  std::cout << "CALL" << std::endl;
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

void Processor::op_cmp(){};
void Processor::op_dadd(){};
void Processor::op_jc_jhs(){};
void Processor::op_jeq_jz(){};
void Processor::op_jge(){};
void Processor::op_jle(){};
void Processor::op_jmp() { std::cout << "JUMP" << std::endl; };
void Processor::op_jn(){};
void Processor::op_jnc_jlo(){};
void Processor::op_jne_jnz(){};

void Processor::op_mov() {
  std::cout << "MOVE" << std::endl;
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
void Processor::op_xor(){};
