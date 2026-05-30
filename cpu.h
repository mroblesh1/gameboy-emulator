#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <stack>

#include "bus.h"

class Z80 {
    private:
        // Registers (A, B, C, D, E, F, H, L)
        // The GB is little-endian, must arrange lower bytes first
        // So C, E, L, F are lo bytes, must go first
        // A, B, D, H are hi bytes, must go second
        struct Register {
            union {
                // Use this to specify the value for the 2 registers
                uint16_t reg;
                // Use this to specify the value for 1 register
                struct {
                    uint8_t lo;
                    uint8_t hi;
                };
                struct {
                    uint8_t unused : 4; // Last 4 bits of register always 0 on Gameboy
                    uint8_t c : 1;  // carry flag
                    uint8_t h : 1;  // half-carry flag
                    uint8_t n : 1;  // subtract flag
                    uint8_t z : 1;  // zero flag
                    uint8_t a_reg;  // Reg A
                };
            };
        };

        // For the gameboy, registers can be addressed either alone (8-bits)
        // or in specific, 16-bit pairs (AF, BC, DE, HL)

        // The AF registers are special, standing for Accumulator & Flags

        // Stack pointer
        uint16_t sp;

        // Program Counter
        uint16_t pc;

        Bus* bus;

    public:
        Z80(Bus* data_bus);
        void cycle();

        void dumpRegs();
        void setFlags(int8_t z, int8_t n, int8_t h, int8_t c);

        // Move these to private when done testing in main
        
        Register af, bc, de, hl;

        //**** CPU Instructions ****//

        // BLOCK 0
        // nop
        // ld r16, imm16
        // ld [r16mem], a
        // ld a, [r16mem]
        // ld [imm16], sp

        void INC_r16(uint16_t* reg_a);  // inc r16
        void DEC_r16(uint16_t* reg_b);  // dec r16
        void ADD_r16(uint16_t* reg_a, uint16_t reg_b);  // add hl, r16

        void INC_r8(uint8_t* reg_a);    // inc r8
        void DEC_r8(uint8_t* reg_a);    // dec r8

        void LD_n8(uint8_t* reg_a, uint8_t imm8);   // ld r8, imm8

        // rlca
        // rrca
        // rla
        // rra
        // daa
        // cpl
        // scf
        // ccf

        // jr imm8
        // jr cond, imm8
        
        // stop

        
        // BLOCK 1
        void LD_r8(uint8_t* reg_a, uint8_t reg_b);  // ld r8, r8

        // halt

        // BLOCK 2
        void ADD_r8(uint8_t reg_a); // add a, r8
        void ADC_r8(uint8_t reg_a); // adc a, r8
        void SUB_r8(uint8_t reg_a); // sub a, r8
        void SBC_r8(uint8_t reg_a); // sbc a, r8
        void AND_r8(uint8_t reg_a); // and a, r8
        void XOR_r8(uint8_t reg_a); // xor a, r8
        void OR_r8(uint8_t reg_a); // or a, r8
        void CP_r8(uint8_t reg_a); // cp a, r8


        // BLOCK 3
        // add a, imm8
        // adc a, imm8
        // sub a, imm8
        // sbc a, imm8
        // and a, imm8
        // xor a, imm8
        // or a, imm8
        // cp a, imm8


        // PREFIX
        void RLC_r8(uint8_t* reg_a);    // 0x00 - 0x07: RLC r8
        void RRC_r8(uint8_t* reg_a);    // 0x08 - 0x0F: RRC r8
        void RL_r8(uint8_t* reg_a);     // 0x10 - 0x17: RL r8
        void RR_r8(uint8_t* reg_a);     // 0x18 - 0x1F: RR r8

        void SLA_r8(uint8_t* reg_a);    // 0x20 - 0x27: SLA r8
        void SRA_r8(uint8_t* reg_a);     // 0x28 - 0x2F: SRA r8
        void SWAP_r8(uint8_t* reg_a);   // 0x30 - 0x37: SWAP r8
        void SRL_r8(uint8_t* reg_a);    // 0x38 - 0x3F: SRL r8

        void BIT(uint8_t bit, uint8_t* reg_a);  // 0x40 - 0x7F: BIT u3, r8
        void RES(uint8_t bit, uint8_t* reg_a);  // 0x80 - 0xBF: RES u3, r8
        void SET(uint8_t bit, uint8_t* reg_a);  // 0xC0 - 0xFF: SET u3, r8

};

#endif