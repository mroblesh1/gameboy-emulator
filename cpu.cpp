#include "cpu.h"
#include "bus.h"

#include <cstdint>
#include <stack>
#include <iostream>
#include <iomanip>

#define HEX_FORMAT std::uppercase << std::setfill('0') << std::setw(2) << std::hex

#define START_ADDRESS 0
#define AF_INIT 0
#define BC_INIT 0
#define DE_INIT 0
#define HL_INIT 0


Z80::Z80(Bus* data_bus) {
    pc = START_ADDRESS;
    sp = 0;

    // Clear registers
    af.reg = AF_INIT;
    bc.reg = BC_INIT;
    de.reg = DE_INIT;
    hl.reg = HL_INIT;

    Bus* bus = data_bus;
}

void Z80::dumpRegs() {
    std::cout << "Register A: 0x" << HEX_FORMAT << (int) af.hi << std::endl;
    std::cout << "Register F: 0x" << HEX_FORMAT << (int) af.lo << std::endl;
    
    std::cout << "Register B: 0x" << HEX_FORMAT << (int) bc.hi << std::endl;
    std::cout << "Register C: 0x" << HEX_FORMAT << (int) bc.lo << std::endl;

    std::cout << "Register D: 0x" << HEX_FORMAT << (int) de.hi << std::endl;
    std::cout << "Register E: 0x" << HEX_FORMAT << (int) de.lo << std::endl;

    std::cout << "Register H: 0x" << HEX_FORMAT << (int) hl.hi << std::endl;
    std::cout << "Register L: 0x" << HEX_FORMAT << (int) hl.lo << std::endl;
}

void Z80::setFlags(int8_t z, int8_t n, int8_t h, int8_t c) {
    if (z != -1) {
        af.z = z;
    }
    if (n != -1) {
        af.n = n;
    }
    if (h != -1) {
        af.h = h;
    }
    if (c != -1) {
        af.c = c;
    }
    return;
}


void Z80::cycle() {
    // Fetch (read the instruction addressed by PC)
    // opcode = read_byte(pc);
    uint8_t opcode = 0x46;  // Replace with a call to read_byte

    // Increment PC after fetching
    ++pc;


    // Execute
    switch (opcode) {
        // 0x00: NOP
        // No Operation
        // 1 byte, 1 cycle
        case (0x00):
        {
            ;   // NOP
        }


        // 0x01 - 0x31 (iterating by MSB): LD r16, n16
        // Load imm16 into r16
        // 3 bytes, 12 cycles
        case (0x01):    // LD BC, n16
        {
            bc.reg = bus.read_word(pc);
            ++pc;
            ++pc;
        } break;
        case (0x11):    // LD DE, n16
        {
            de.reg = bus.read_word(pc);
            ++pc;
            ++pc;
        } break;
        case (0x21):    // LD HL, n16
        {
            hl.reg = bus.read_word(pc);
            ++pc;
            ++pc;
        } break;
        case (0x31):    // LD SP, n16
        {
            sp = bus.read_word(pc);
            ++pc;
            ++pc;
        } break;

        
        // 0x08: LD [r16], sp
        // Copy word in sp register into [r16] and [r16 + 1]
        // 3 bytes, 20 cycles
        case (0x08):    // LD [r16], sp
        {
            bus.write_word(bus.read_word(pc), sp);
            ++pc;
            ++pc;
        } break;
        

        // 0x02 - 0x32 (iterating by MSB): LD [r16mem], a
        // Load reg A into [r16mem]
        // 1 byte, 8 cycles
        case (0x02):    // LD [BC], A
        {
            bus.write_byte(bc.reg, af.hi);
        } break;
        case (0x12):    // LD [DE], A
        {
            bus.write_byte(de.reg, af.hi);
        } break;
        case (0x22):    // LD [HL+], A
        {
            bus.write_byte(hl.reg, af.hi);
            hl.reg++;
        } break;
        case (0x32):    // LD [HL-], A
        {
            bus.write_byte(hl.reg, af.hi);
            hl.reg--;
        } break;

        // 0x0A - 0x3A (iterating by MSB): LD a, [r16mem]
        // Load [r16mem] into reg A
        // 1 byte, 8 cycles
        case (0x0A):    // LD A, [BC]
        {
            af.hi = bus.read_byte(bc.reg);
        } break;
        case (0x1A):    // LD A, [DE]
        {
            af.hi = bus.read_byte(de.reg);
        } break;
        case (0x2A):    // LD A, [HL+]
        {
            af.hi = bus.read_byte(hl.reg);
            hl.reg++;
        } break;
        case (0x3A):    // LD A, [HL-]
        {
            af.hi = bus.read_byte(hl.reg);
            hl.reg--;
        } break;



        // 0x03 - 0x33 (iterating by MSB): INC r16
        // Increment the 16-bit register
        // 1 byte, 8 cycles

        case (0x03):    // INC BC
        {
            INC_r16(&bc.reg);
        } break;
        case (0x13):    // INC DE
        {
            INC_r16(&de.reg);
        } break;
        case (0x23):    // INC HL
        {
            INC_r16(&hl.reg);
        } break;
        case (0x33):    // INC SP
        {
            INC_r16(&sp);
        } break;

        // 0x0B - 0x3B (iterating by MSB): DEC r16
        // Decrement the 16-bit register
        // 1 byte, 8 cycles

        case (0x0B):    // DEC BC
        {
            DEC_r16(&bc.reg);
        } break;
        case (0x1B):    // DEC DE
        {
            DEC_r16(&de.reg);
        } break;
        case (0x2B):    // DEC HL
        {
            DEC_r16(&hl.reg);
        } break;
        case (0x3B):    // DEC SP
        {
            DEC_r16(&sp);
        } break;


        // 0x09 - 0x39 (iterating by MSB): ADD hl, r16
        // Add r16 to the hl register
        // 1 byte, 8 cycles
        case (0x09):    // ADD HL, BC
        {
            ;
        } break;



        // 0x04 - 0x34 and 0x0C - 0x3C (iterating by MSB): INC r8
        // Increment the 8-bit register
        // 1 byte, 4 cycles

        case (0x04):    // INC B
        {
            INC_r8(&bc.hi);
        } break;
        case (0x0C):    // INC C
        {
            INC_r8(&bc.lo);
        } break;
        case (0x14):    // INC D
        {
            INC_r8(&de.hi);
        } break;
        case (0x1C):    // INC E
        {
            INC_r8(&de.lo);
        } break;
        case (0x24):    // INC H
        {
            INC_r8(&hl.hi);
        } break;
        case (0x2C):    // INC L
        {
            INC_r8(&hl.lo);
        } break;
        case (0x34):    // INC [HL]
        // This instruction takes 12 cycles
        {
            uint8_t val = bus.read_byte(hl.reg);
            bus.write_byte(hl.reg, val + 1);

            int8_t h = ((val & 0x0F) == 0x0F);
            int8_t z = ((val + 1) == 0x00);
            setFlags(z, 0, h, -1);
        } break;
        case (0x3C):    // INC A
        {
            INC_r8(&af.hi);
        } break;

        // 0x05 - 0x35 and 0x0D - 0x3D (iterating by MSB): DEC r8
        // Decrement the 8-bit register
        // 1 byte, 4 cycles

        case (0x05):    // DEC B
        {
            DEC_r8(&bc.hi);
        } break;
        case (0x0D):    // DEC C
        {
            DEC_r8(&bc.lo);
        } break;
        case (0x15):    // DEC D
        {
            DEC_r8(&de.hi);
        } break;
        case (0x1D):    // DEC E
        {
            DEC_r8(&de.lo);
        } break;
        case (0x25):    // DEC H
        {
            DEC_r8(&hl.hi);
        } break;
        case (0x2D):    // DEC L
        {
            DEC_r8(&hl.lo);
        } break;
        case (0x35):    // DEC [HL]
        // This instruction takes 12 cycles
        {
            uint8_t val = bus.read_byte(hl.reg);
            bus.write_byte(hl.reg, val - 1);
            
            int8_t h = ((val & 0x0F) == 0x0F);
            int8_t z = ((val + 1) == 0x00);
            setFlags(z, 0, h, -1);
        } break;
        case (0x3D):    // DEC A
        {
            DEC_r8(&af.hi);
        } break;



        // 0x06 - 0x36 and 0x0E - 0x3E (iterating by MSB): LD r8, n8
        // Load imm8 into r8
        // 2 bytes, 8 cycles

        // For all instructions, read a second byte for the imm8
        case (0x06):    // LD B, n8
        {
            bc.hi = bus.read_byte(pc);
            ++pc;
            ;
        } break;
        case (0x0E):    // LD C, n8
        {
            bc.lo = bus.read_byte(pc);
            ++pc;
        } break;
        case (0x16):    // LD D, n8
        {
            de.hi = bus.read_byte(pc);
            ++pc;
        } break;
        case (0x1E):    // LD E, n8
        {
            de.lo = bus.read_byte(pc);
            ++pc;
        } break;
        case (0x26):    // LD H, n8
        {
            hl.hi = bus.read_byte(pc);
            ++pc;
        } break;
        case (0x2E):    // LD L, n8
        {
            hl.lo = bus.read_byte(pc);
            ++pc;
        } break;
        case (0x36):    // LD [HL], n8
        {
            // 12 cycles
            bus.write_byte(hl.reg, bus.read_byte(pc));
            ++pc;
        } break;
        case (0x3E):    // LD A, n8
        {
            af.hi = bus.read_byte(pc);
            ++pc;
        } break;



        // 0x40 - 0x7F: LD r8, r8
        // Load register on right into register on left (8-bit)
        // 1 byte, 4 cycles (8 for [HL])

        case (0x40):    // LD B, B
        {
            LD_r8(&bc.hi, bc.hi);
        } break;
        case (0x41):    // LD B, C
        {
            LD_r8(&bc.hi, bc.lo);
        } break;
        case (0x42):    // LD B, D
        {
            LD_r8(&bc.hi, de.hi);
        } break;
        case (0x43):    // LD B, E
        {
            LD_r8(&bc.hi, de.lo);
        } break;
        case (0x44):    // LD B, H
        {
            LD_r8(&bc.hi, hl.hi);
        } break;
        case (0x45):    // LD B, L
        {
            LD_r8(&bc.hi, hl.lo);
        } break;
        case (0x46):    // LD B, [HL]
        {
            // Load value pointed by HL into B
            bc.hi = bus.read_byte(hl.reg);
        } break;
        case (0x47):    // LD B, A
        {
            LD_r8(&bc.hi, af.hi);
        } break;
    }
    return;
}



// **** CPU Instructions **** //

void Z80::LD_n8(uint8_t* reg_a, uint8_t imm8) {
    *reg_a = imm8;
    return;
}

void Z80::INC_r16(uint16_t* reg_a) {
    (*reg_a)++;
    return;
}

void Z80::DEC_r16(uint16_t* reg_a) {
    (*reg_a)--;
    return;
}


void Z80::INC_r8(uint8_t* reg_a) {
    int8_t h = (*reg_a == 0x0F);
    (*reg_a)++;
    int8_t z = (*reg_a == 0x00);
    setFlags(z, 0, h, -1);
    return;
}

void Z80::DEC_r8(uint8_t* reg_a) {
    (*reg_a)--;
    return;
}

// LD r8, r8
// Load register on right into register on left

// Typically 1 byte, 4 T-cycles
// 8 T-cycles if HL register involved
void Z80::LD_r8(uint8_t* reg_a, uint8_t reg_b) {
    *reg_a = reg_b;
    return;
}
