#include "processor.h"
#include <iostream>

Processor::Processor(){
  PC = &R0;
  SP = &R1;
  SR = &R2;
  GC1 = &R2.val;
  GC2 = &R3;

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

void Processor::Step() {
  auto instruction = FetchInstruction(*PC);
  for(auto op_code : op_map) {
    if(instruction & op_code.first) {
      // Execute instruction
      (this->*op_code.second)();
      break;
    }
  }
}

uint16_t Processor::FetchInstruction(uint16_t PC) {
  if(PC <= PERIPH_MAX) {
    throw(ProcessorException("Tried to fetch instruction from peripheral address space"));
  }
  return(mem->GetUint16(PC));
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

Processor::~Processor() {}
