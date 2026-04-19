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

        Bus bus;

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
        // add a, r8
        // adc a, r8
        // sub a, r8
        // sbc a, r8
        // and a, r8
        // xor a, r8
        // or a, r8
        // cp a, r8

        // BLOCK 3
        // add a, imm8
        // adc a, imm8
        // sub a, imm8
        // sbc a, imm8
        // and a, imm8
        // xor a, imm8
        // or a, imm8
        // cp a, imm8



};

#endif