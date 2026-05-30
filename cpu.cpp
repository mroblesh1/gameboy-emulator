#include "cpu.h"
#include "bus.h"

#include <cstdint>
#include <stack>
#include <iostream>
#include <iomanip>

#define HEX_FORMAT std::uppercase << std::setfill('0') << std::setw(4) << std::hex

#define START_ADDRESS 0x0100
#define SP_ADDRESS 0xFFFF       // Technically should start at 0xFFFE but offset to account for how data is written
#define AF_INIT 0
#define BC_INIT 0
#define DE_INIT 0
#define HL_INIT 0


Z80::Z80(Bus* data_bus) {
    pc = START_ADDRESS;
    sp = SP_ADDRESS;

    // Clear registers
    af.reg = AF_INIT;
    bc.reg = BC_INIT;
    de.reg = DE_INIT;
    hl.reg = HL_INIT;

    bus = data_bus;
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
    uint8_t opcode = bus->read_byte(pc);

    // CPU state output
    //std::cout << "PC: 0x" << HEX_FORMAT << (int) pc << ". Opcode: 0x" << HEX_FORMAT << (int) opcode << ". SP: 0x" << HEX_FORMAT << (int) sp << std::endl;

    // Increment PC after fetching
    ++pc;


    // Execute
    switch (opcode) {
        // 0x00: NOP
        // No Operation
        // 1 byte, 4 cycles
        case (0x00):    // NOP
        {
            ;
        } break;

        // 0x10: STOP
        // Stop
        // 2 bytes, 4 cycles
        // case (0x10):    // STOP
        // {
        //     // TODO: write STOP instruction implementation
        //     // Place the CPU in a low-power mode
        //     ;   
        // } break;


        // 0x01 - 0x31 (iterating by MSB): LD r16, n16
        // Load imm16 into r16
        // 3 bytes, 12 cycles
        case (0x01):    // LD BC, n16
        {
            bc.reg = bus->read_word(pc);
            ++pc;
            ++pc;
        } break;
        case (0x11):    // LD DE, n16
        {
            de.reg = bus->read_word(pc);
            ++pc;
            ++pc;
        } break;
        case (0x21):    // LD HL, n16
        {
            hl.reg = bus->read_word(pc);
            ++pc;
            ++pc;
        } break;
        case (0x31):    // LD SP, n16
        {
            sp = bus->read_word(pc);
            ++pc;
            ++pc;
        } break;

        
        // 0x08: LD [r16], sp
        // Copy word in sp register into [r16] and [r16 + 1]
        // 3 bytes, 20 cycles
        case (0x08):    // LD [r16], sp
        {
            bus->write_word(bus->read_word(pc), sp);
            ++pc;
            ++pc;
        } break;
        

        // 0x02 - 0x32 (iterating by MSB): LD [r16mem], a
        // Load reg A into [r16mem]
        // 1 byte, 8 cycles
        case (0x02):    // LD [BC], A
        {
            bus->write_byte(bc.reg, af.hi);
        } break;
        case (0x12):    // LD [DE], A
        {
            bus->write_byte(de.reg, af.hi);
        } break;
        case (0x22):    // LD [HL+], A
        {
            bus->write_byte(hl.reg, af.hi);
            hl.reg++;
        } break;
        case (0x32):    // LD [HL-], A
        {
            bus->write_byte(hl.reg, af.hi);
            hl.reg--;
        } break;

        // 0x0A - 0x3A (iterating by MSB): LD a, [r16mem]
        // Load [r16mem] into reg A
        // 1 byte, 8 cycles
        case (0x0A):    // LD A, [BC]
        {
            af.hi = bus->read_byte(bc.reg);
        } break;
        case (0x1A):    // LD A, [DE]
        {
            af.hi = bus->read_byte(de.reg);
        } break;
        case (0x2A):    // LD A, [HL+]
        {
            af.hi = bus->read_byte(hl.reg);
            hl.reg++;
        } break;
        case (0x3A):    // LD A, [HL-]
        {
            af.hi = bus->read_byte(hl.reg);
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
            ADD_r16(&hl.reg, bc.reg);
        } break;
        case (0x19):    // ADD HL, DE
        {
            ADD_r16(&hl.reg, de.reg);
        } break;
        case (0x29):    // ADD HL, HL
        {
            ADD_r16(&hl.reg, hl.reg);
        } break;
        case (0x39):    // ADD HL, SP
        {
            ADD_r16(&hl.reg, sp);
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
            // Flags
            // Z    Set if result is 0
            // N    0
            // H    Set if overflow from bit 3.
            // C    Unchanged
            uint8_t val = bus->read_byte(hl.reg);
            bus->write_byte(hl.reg, val + 1);

            int8_t h = ((val & 0x0F) == 0x0F);
            int8_t z = (((val + 1) & 0xFF) == 0x00);
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
            // Flags
            // Z    Set if result is 0
            // N    1
            // H    Set if borrow from bit 4.
            // C    Unchanged
            uint8_t val = bus->read_byte(hl.reg);
            bus->write_byte(hl.reg, val - 1);
            
            int8_t h = ((val & 0x0F) == 0x00);
            int8_t z = ((val - 1) == 0x00);
            setFlags(z, 1, h, -1);
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
            bc.hi = bus->read_byte(pc);
            ++pc;
            ;
        } break;
        case (0x0E):    // LD C, n8
        {
            bc.lo = bus->read_byte(pc);
            ++pc;
        } break;
        case (0x16):    // LD D, n8
        {
            de.hi = bus->read_byte(pc);
            ++pc;
        } break;
        case (0x1E):    // LD E, n8
        {
            de.lo = bus->read_byte(pc);
            ++pc;
        } break;
        case (0x26):    // LD H, n8
        {
            hl.hi = bus->read_byte(pc);
            ++pc;
        } break;
        case (0x2E):    // LD L, n8
        {
            hl.lo = bus->read_byte(pc);
            ++pc;
        } break;
        case (0x36):    // LD [HL], n8
        {
            // 12 cycles
            bus->write_byte(hl.reg, bus->read_byte(pc));
            ++pc;
        } break;
        case (0x3E):    // LD A, n8
        {
            af.hi = bus->read_byte(pc);
            ++pc;
        } break;



        // 0x07 and 0x0F: RLCA & RRCA
        // Rotate the A register (bit shifted out replaces empty bit)
        // 1 byte, 4 cycles

        case (0x07):    // RLCA
        {
            // Flags
            // Z    0
            // N    0
            // H    0
            // C    Set if bit 7 is 1

            int8_t c = af.hi >> 7;
            af.hi = (af.hi << 1) + c;

            setFlags(0, 0, 0, c);
        } break;
        case (0x0F):    // RRCA
        {
            // Flags
            // Z    0
            // N    0
            // H    0
            // C    Set if bit 0 is 1

            int8_t c = af.hi & 0x01;
            af.hi = (af.hi >> 1) + ((uint8_t) c << 7);

            setFlags(0, 0, 0, c);
        } break;


        // 0x17 and 0x1F: RLA & RRA
        // Rotate the A register, through the carry flag (treat as a 9-bit register)
        // 1 byte, 4 cycles

        case (0x17):    // RLA
        {
            // Flags
            // Z    0
            // N    0
            // H    0
            // C    Set if bit 7 is 1

            // TODO: Review if current_c is necessary.
            // I use it to typecast af.c to uint8_t, but it might be possible to skip entirely

            int8_t c = af.hi >> 7;
            uint8_t current_c = af.c;
            af.hi = (af.hi << 1) + current_c;

            setFlags(0, 0, 0, c);
        } break;
        case (0x1F):    // RRA
        {
            // Flags
            // Z    0
            // N    0
            // H    0
            // C    Set if bit 0 is 1

            int8_t c = af.hi & 0x01;
            uint8_t current_c = af.c;
            af.hi = (af.hi >> 1) + (current_c << 7);

            setFlags(0, 0, 0, c);
        } break;

        // 0x27: DAA
        // Decimal Adjust Accumulator: Convert from raw binary to Binary Coded Decimal
        // 1 byte, 4 cycles
        
        case (0x27):    // DAA
        {
            int8_t c = -1;

            if (af.n) {
                uint8_t adjustment = 0;
                if (af.h) {
                    adjustment += 0x06;
                }
                if (af.c) {
                    adjustment += 0x60;
                }
                af.a_reg -= adjustment;
            } else {
                uint8_t adjustment = 0;
                if (af.h | ((af.a_reg & 0x0F) > 0x09)) {
                    adjustment += 0x06;
                }
                if (af.c | (af.a_reg > 0x99)) {
                    adjustment += 0x60;
                    c = 1;
                }
                af.a_reg += adjustment;
            }
            
            int8_t z = (af.a_reg == 0x00);
            
            setFlags(z, -1, 0, c);
        } break;

        // 0x2F: CPL
        // Complement (invert) the accumulator value
        // 1 byte, 4 cycles
        case (0x2F):    // CPL
        {
            // Flags
            // Z    Unchanged
            // N    Set
            // H    Set
            // C    Unchanged
            af.hi = ~af.hi;
            setFlags(-1, 1, 1, -1);
        } break;

        // 0x37: SCF
        // Set carry flag
        // 1 byte, 4 cycles
        case (0x37):    // SCF
        {
            // Flags
            // Z    Unchanged
            // N    0
            // H    0
            // C    Set
            setFlags(-1, 0, 0, 1);
        } break;

        // 0x3F: CCF
        // Complement (invert) the carry flag
        // 1 byte, 4 cycles
        case (0x3F):    // CCF
        {
            // Flags
            // Z    Unchanged
            // N    0
            // H    0
            // C    Invert the existing c flag
            setFlags(-1, 0, 0, !af.c);
        } break;


        // 0x18: JR e8
        // Relative jump to imm16(?)
        // Or more directly, jump to the PC + an 8-bit offset
        // 2 bytes, 12 cycles
        case (0x18):    // JR e8
        {
            // byte 1: instruction
            // increment PC
            // (this has been done so far)

            // byte 2: 8-bit signed offset
            // increment PC
            int8_t offset = bus->read_byte(pc);
            ++pc;
            // Offset from current PC
            pc += offset;
        } break;


        // 0x20 - 0x30 & 0x29 - 0x39: JR cc, n16
        // Relative jump to imm16, based on some condition
        // 2 bytes, 12 cycles (8 cycles if untaken)

        case (0x20):    // JR NZ, e8
        {
            // byte 1: instruction
            // increment PC
            // (this has been done so far)

            // byte 2: 8-bit signed offset
            // increment PC
            int8_t offset = bus->read_byte(pc);
            ++pc;
            // Offset from current PC
            if (!af.z) {
                pc += offset;
            }
        } break;

        case (0x28):    // JR Z, e8
        {
            // byte 1: instruction
            // increment PC
            // (this has been done so far)

            // byte 2: 8-bit signed offset
            // increment PC
            int8_t offset = bus->read_byte(pc);
            ++pc;
            // Offset from current PC
            if (af.z) {
                pc += offset;
            }
        } break;

        case (0x30):    // JR NC, e8
        {
            // byte 1: instruction
            // increment PC
            // (this has been done so far)

            // byte 2: 8-bit signed offset
            // increment PC
            int8_t offset = bus->read_byte(pc);
            ++pc;
            // Offset from current PC
            if (!af.c) {
                pc += offset;
            }
        } break;

        case (0x38):    // JR C, e8
        {
            // byte 1: instruction
            // increment PC
            // (this has been done so far)

            // byte 2: 8-bit signed offset
            // increment PC
            int8_t offset = bus->read_byte(pc);
            ++pc;
            // Offset from current PC
            if (af.c) {
                pc += offset;
            }
        } break;



        // 0x40 - 0x7F: LD r8, r8
        // Load register on right into register on left (8-bit)
        // 1 byte, 4 cycles (8 for [HL])

        // B Register //

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
            bc.hi = bus->read_byte(hl.reg);
        } break;
        case (0x47):    // LD B, A
        {
            LD_r8(&bc.hi, af.hi);
        } break;


        // C Register //

        case (0x48):    // LD C, B
        {
            LD_r8(&bc.lo, bc.hi);
        } break;
        case (0x49):    // LD C, C
        {
            LD_r8(&bc.lo, bc.lo);
        } break;
        case (0x4A):    // LD C, D
        {
            LD_r8(&bc.lo, de.hi);
        } break;
        case (0x4B):    // LD C, E
        {
            LD_r8(&bc.lo, de.lo);
        } break;
        case (0x4C):    // LD C, H
        {
            LD_r8(&bc.lo, hl.hi);
        } break;
        case (0x4D):    // LD C, L
        {
            LD_r8(&bc.lo, hl.lo);
        } break;
        case (0x4E):    // LD C, [HL]
        {
            // Load value pointed by HL into C
            bc.lo = bus->read_byte(hl.reg);
        } break;
        case (0x4F):    // LD C, A
        {
            LD_r8(&bc.lo, af.hi);
        } break;


        // D Register //

        case (0x50):    // LD D, B
        {
            LD_r8(&de.hi, bc.hi);
        } break;
        case (0x51):    // LD D, C
        {
            LD_r8(&de.hi, bc.lo);
        } break;
        case (0x52):    // LD D, D
        {
            LD_r8(&de.hi, de.hi);
        } break;
        case (0x53):    // LD D, E
        {
            LD_r8(&de.hi, de.lo);
        } break;
        case (0x54):    // LD D, H
        {
            LD_r8(&de.hi, hl.hi);
        } break;
        case (0x55):    // LD D, L
        {
            LD_r8(&de.hi, hl.lo);
        } break;
        case (0x56):    // LD D, [HL]
        {
            // Load value pointed by HL into D
            de.hi = bus->read_byte(hl.reg);
        } break;
        case (0x57):    // LD D, A
        {
            LD_r8(&de.hi, af.hi);
        } break;


        // E Register //

        case (0x58):    // LD E, B
        {
            LD_r8(&de.lo, bc.hi);
        } break;
        case (0x59):    // LD E, C
        {
            LD_r8(&de.lo, bc.lo);
        } break;
        case (0x5A):    // LD E, D
        {
            LD_r8(&de.lo, de.hi);
        } break;
        case (0x5B):    // LD E, E
        {
            LD_r8(&de.lo, de.lo);
        } break;
        case (0x5C):    // LD E, H
        {
            LD_r8(&de.lo, hl.hi);
        } break;
        case (0x5D):    // LD E, L
        {
            LD_r8(&de.lo, hl.lo);
        } break;
        case (0x5E):    // LD E, [HL]
        {
            // Load value pointed by HL into E
            de.lo = bus->read_byte(hl.reg);
        } break;
        case (0x5F):    // LD E, A
        {
            LD_r8(&de.lo, af.hi);
        } break;


        // H Register //

        case (0x60):    // LD H, B
        {
            LD_r8(&hl.hi, bc.hi);
        } break;
        case (0x61):    // LD H, C
        {
            LD_r8(&hl.hi, bc.lo);
        } break;
        case (0x62):    // LD H, D
        {
            LD_r8(&hl.hi, de.hi);
        } break;
        case (0x63):    // LD H, E
        {
            LD_r8(&hl.hi, de.lo);
        } break;
        case (0x64):    // LD H, H
        {
            LD_r8(&hl.hi, hl.hi);
        } break;
        case (0x65):    // LD H, L
        {
            LD_r8(&hl.hi, hl.lo);
        } break;
        case (0x66):    // LD H, [HL]
        {
            // Load value pointed by HL into B
            hl.hi = bus->read_byte(hl.reg);
        } break;
        case (0x67):    // LD H, A
        {
            LD_r8(&hl.hi, af.hi);
        } break;

        
        // L Register //

        case (0x68):    // LD L, B
        {
            LD_r8(&hl.lo, bc.hi);
        } break;
        case (0x69):    // LD L, C
        {
            LD_r8(&hl.lo, bc.lo);
        } break;
        case (0x6A):    // LD L, D
        {
            LD_r8(&hl.lo, de.hi);
        } break;
        case (0x6B):    // LD L, E
        {
            LD_r8(&hl.lo, de.lo);
        } break;
        case (0x6C):    // LD L, H
        {
            LD_r8(&hl.lo, hl.hi);
        } break;
        case (0x6D):    // LD L, L
        {
            LD_r8(&hl.lo, hl.lo);
        } break;
        case (0x6E):    // LD L, [HL]
        {
            // Load value pointed by HL into C
            hl.lo = bus->read_byte(hl.reg);
        } break;
        case (0x6F):    // LD L, A
        {
            LD_r8(&hl.lo, af.hi);
        } break;


        // [HL] Register //
        // For all of these, except for HALT, 8 cycles

        case (0x70):    // LD [HL], B
        {
            bus->write_byte(hl.reg, bc.hi);
        } break;
        case (0x71):    // LD [HL], C
        {
            bus->write_byte(hl.reg, bc.lo);
        } break;
        case (0x72):    // LD [HL], D
        {
            bus->write_byte(hl.reg, de.hi);
        } break;
        case (0x73):    // LD [HL], E
        {
            bus->write_byte(hl.reg, de.lo);
        } break;
        case (0x74):    // LD [HL], H
        {
            bus->write_byte(hl.reg, hl.hi);
        } break;
        case (0x75):    // LD [HL], L
        {
            bus->write_byte(hl.reg, hl.lo);
        } break;
        // case (0x76):    // HALT
        // {
        //     // TODO: Write HALT instruction implementation
        //     ;
        // } break;
        case (0x77):    // LD [HL], A
        {
            bus->write_byte(hl.reg, af.hi);
        } break;

        
        // A Register //

        case (0x78):    // LD A, B
        {
            LD_r8(&af.hi, bc.hi);
        } break;
        case (0x79):    // LD A, C
        {
            LD_r8(&af.hi, bc.lo);
        } break;
        case (0x7A):    // LD A, D
        {
            LD_r8(&af.hi, de.hi);
        } break;
        case (0x7B):    // LD A, E
        {
            LD_r8(&af.hi, de.lo);
        } break;
        case (0x7C):    // LD A, H
        {
            LD_r8(&af.hi, hl.hi);
        } break;
        case (0x7D):    // LD A, L
        {
            LD_r8(&af.hi, hl.lo);
        } break;
        case (0x7E):    // LD A, [HL]
        {
            // Load value pointed by HL into C
            af.hi = bus->read_byte(hl.reg);
        } break;
        case (0x7F):    // LD A, A
        {
            LD_r8(&af.hi, af.hi);
        } break;




        // 0x80 - 0x87: ADD A, r8
        // Adds r8 to A register
        // 1 byte, 4 cycles (8 for [HL])
        case (0x80):    // ADD A, B
        {
            ADD_r8(bc.hi);
        } break;
        case (0x81):    // ADD A, C
        {
            ADD_r8(bc.lo);
        } break;
        case (0x82):    // ADD A, D
        {
            ADD_r8(de.hi);
        } break;
        case (0x83):    // ADD A, E
        {
            ADD_r8(de.lo);
        } break;
        case (0x84):    // ADD A, H
        {
            ADD_r8(hl.hi);
        } break;
        case (0x85):    // ADD A, L
        {
            ADD_r8(hl.lo);
        } break;
        case (0x86):    // ADD A, [HL]
        {
            ADD_r8(bus->read_byte(hl.reg));
        } break;
        case (0x87):    // ADD A, A
        {
            ADD_r8(af.hi);
        } break;


        // 0x88 - 0x8F: ADC A, r8
        // Adds r8 and carry to A register
        // 1 byte, 4 cycles (8 for [HL])
        case (0x88):    // ADC A, B
        {
            ADC_r8(bc.hi);
        } break;
        case (0x89):    // ADC A, C
        {
            ADC_r8(bc.lo);
        } break;
        case (0x8A):    // ADC A, D
        {
            ADC_r8(de.hi);
        } break;
        case (0x8B):    // ADC A, E
        {
            ADC_r8(de.lo);
        } break;
        case (0x8C):    // ADC A, H
        {
            ADC_r8(hl.hi);
        } break;
        case (0x8D):    // ADC A, L
        {
            ADC_r8(hl.lo);
        } break;
        case (0x8E):    // ADC A, [HL]
        {
            ADC_r8(bus->read_byte(hl.reg));
        } break;
        case (0x8F):    // ADC A, A
        {
            ADC_r8(af.hi);
        } break;


        // 0x90 - 0x97: SUB A, r8
        // Subtract r8 from A register
        // 1 byte, 4 cycles (8 for [HL])
        case (0x90):    // SUB A, B
        {
            SUB_r8(bc.hi);
        } break;
        case (0x91):    // SUB A, C
        {
            SUB_r8(bc.lo);
        } break;
        case (0x92):    // SUB A, D
        {
            SUB_r8(de.hi);
        } break;
        case (0x93):    // SUB A, E
        {
            SUB_r8(de.lo);
        } break;
        case (0x94):    // SUB A, H
        {
            SUB_r8(hl.hi);
        } break;
        case (0x95):    // SUB A, L
        {
            SUB_r8(hl.lo);
        } break;
        case (0x96):    // SUB A, [HL]
        {
            SUB_r8(bus->read_byte(hl.reg));
        } break;
        case (0x97):    // SUB A, A
        {
            SUB_r8(af.hi);
        } break;


        // 0x98 - 0x9F: SBC A, r8
        // Subtract r8 and carry from A register
        // 1 byte, 4 cycles (8 for [HL])
        case (0x98):    // SBC A, B
        {
            SBC_r8(bc.hi);
        } break;
        case (0x99):    // SBC A, C
        {
            SBC_r8(bc.lo);
        } break;
        case (0x9A):    // SBC A, D
        {
            SBC_r8(de.hi);
        } break;
        case (0x9B):    // SBC A, E
        {
            SBC_r8(de.lo);
        } break;
        case (0x9C):    // SBC A, H
        {
            SBC_r8(hl.hi);
        } break;
        case (0x9D):    // SBC A, L
        {
            SBC_r8(hl.lo);
        } break;
        case (0x9E):    // SBC A, [HL]
        {
            SBC_r8(bus->read_byte(hl.reg));
        } break;
        case (0x9F):    // SBC A, A
        {
            SBC_r8(af.hi);
        } break;


        // 0xA0 - 0xA7: AND A, r8
        // AND A and r8 and store result in register A
        // 1 byte, 4 cycles (8 for [HL])
        case (0xA0):    // AND A, B
        {
            AND_r8(bc.hi);
        } break;
        case (0xA1):    // AND A, C
        {
            AND_r8(bc.lo);
        } break;
        case (0xA2):    // AND A, D
        {
            AND_r8(de.hi);
        } break;
        case (0xA3):    // AND A, E
        {
            AND_r8(de.lo);
        } break;
        case (0xA4):    // AND A, H
        {
            AND_r8(hl.hi);
        } break;
        case (0xA5):    // AND A, L
        {
            AND_r8(hl.lo);
        } break;
        case (0xA6):    // AND A, [HL]
        {
            AND_r8(bus->read_byte(hl.reg));
        } break;
        case (0xA7):    // AND A, A
        {
            AND_r8(af.hi);
        } break;


        // 0xA8 - 0xAF: XOR A, r8
        // XOR A and r8 and store result in register A
        // 1 byte, 4 cycles (8 for [HL])
        case (0xA8):    // XOR A, B
        {
            XOR_r8(bc.hi);
        } break;
        case (0xA9):    // XOR A, C
        {
            XOR_r8(bc.lo);
        } break;
        case (0xAA):    // XOR A, D
        {
            XOR_r8(de.hi);
        } break;
        case (0xAB):    // XOR A, E
        {
            XOR_r8(de.lo);
        } break;
        case (0xAC):    // XOR A, H
        {
            XOR_r8(hl.hi);
        } break;
        case (0xAD):    // XOR A, L
        {
            XOR_r8(hl.lo);
        } break;
        case (0xAE):    // XOR A, [HL]
        {
            XOR_r8(bus->read_byte(hl.reg));
        } break;
        case (0xAF):    // XOR A, A
        {
            XOR_r8(af.hi);
        } break;


        // 0xB0 - 0xB7: OR A, r8
        // OR A and r8 and store result in register A
        // 1 byte, 4 cycles (8 for [HL])
        case (0xB0):    // OR A, B
        {
            OR_r8(bc.hi);
        } break;
        case (0xB1):    // OR A, C
        {
            OR_r8(bc.lo);
        } break;
        case (0xB2):    // OR A, D
        {
            OR_r8(de.hi);
        } break;
        case (0xB3):    // OR A, E
        {
            OR_r8(de.lo);
        } break;
        case (0xB4):    // OR A, H
        {
            OR_r8(hl.hi);
        } break;
        case (0xB5):    // OR A, L
        {
            OR_r8(hl.lo);
        } break;
        case (0xB6):    // OR A, [HL]
        {
            OR_r8(bus->read_byte(hl.reg));
        } break;
        case (0xB7):    // OR A, A
        {
            OR_r8(af.hi);
        } break;


        // 0xB8 - 0xBF: CP A, r8
        // Compare r8 with value in A register
        // Effectively, this is subtraction, but no result is saved. Output is in the flags
        // 1 byte, 4 cycles (8 for [HL])
        case (0xB8):    // CP A, B
        {
            CP_r8(bc.hi);
        } break;
        case (0xB9):    // CP A, C
        {
            CP_r8(bc.lo);
        } break;
        case (0xBA):    // CP A, D
        {
            CP_r8(de.hi);
        } break;
        case (0xBB):    // CP A, E
        {
            CP_r8(de.lo);
        } break;
        case (0xBC):    // CP A, H
        {
            CP_r8(hl.hi);
        } break;
        case (0xBD):    // CP A, L
        {
            CP_r8(hl.lo);
        } break;
        case (0xBE):    // CP A, [HL]
        {
            CP_r8(bus->read_byte(hl.reg));
        } break;
        case (0xBF):    // CP A, A
        {
            CP_r8(af.hi);
        } break;



        // Block 3

        // 0xC9: RET
        // Return from subroutine (Pop address at SP and jump to address)
        // 1 byte, 16 cycles
        case (0xC9):    // RET
        {
            pc = bus->read_word(sp);    // Set pc to popped value
            // Update sp
            sp++;
            sp++;
        } break;

        // 0xC0 - 0xD0, 0xC8 - 0xD8: RET cc
        // Return from subroutine depending on condition
        // 1 byte, 20 cycles (8 untaken)

        case (0xC0):    // RET NZ
        {
            if (!af.z) {
                pc = bus->read_word(sp);    // Set pc to popped value
                // Update sp
                sp++;
                sp++;
            }
        } break;

        case (0xD0):    // RET NC
        {
            if (!af.c) {
                pc = bus->read_word(sp);    // Set pc to popped value
                // Update sp
                sp++;
                sp++;
            }
        } break;


        case (0xC8):    // RET Z
        {
            if (af.z) {
                pc = bus->read_word(sp);    // Set pc to popped value
                // Update sp
                sp++;
                sp++;
            }
        } break;


        case (0xD8):    // RET C
        {
            if (af.c) {
                pc = bus->read_word(sp);    // Set pc to popped value
                // Update sp
                sp++;
                sp++;
            }
        } break;




        // 0xC3: JP n16
        // Jump to address n16
        // 3 bytes, 16 cycles
        case (0xC3):    // JP n16
        {
            // First, read two bytes to get n16
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Finally, jump to address n16
            pc = n16;
        } break;


        // 0xC2 - 0xD2, 0xCA - 0xDA: JP cc, n16
        // Jump to address n16 depending on some condition
        // 3 bytes, 16/12 cycles (if cond met / not met)

        case (0xC2):    // JP NZ, n16
        {
            // First, read two bytes to get n16
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Finally, jump to address n16 based on condition
            if (!af.z) {
                pc = n16;
            }
        } break;

        case (0xD2):    // JP NC, n16
        {
            // First, read two bytes to get n16
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Finally, jump to address n16 based on condition
            if (!af.c) {
                pc = n16;
            }
        } break;

        case (0xCA):    // JP Z, n16
        {
            // First, read two bytes to get n16
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Finally, jump to address n16 based on condition
            if (af.z) {
                pc = n16;
            }
        } break;

        case (0xDA):    // JP C, n16
        {
            // First, read two bytes to get n16
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Finally, jump to address n16 based on condition
            if (af.c) {
                pc = n16;
            }
        } break;

        // 0xE9: JP HL
        // Jump to 16-bit address in HL register
        // 1 byte, 4 cycles
        case (0xE9):
        {
            pc = hl.reg;
        } break;


        // 0xDC: CALL n16
        // Call address n16
        // Pushes address of next instruction to stack, then execute JP n16
        // 3 bytes, 24 cycles
        case (0xCD):    // CALL n16
        {
            // First, read two bytes to get n16
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Then, push the next instruction to the stack
            sp--;
            sp--;
            bus->write_word(sp, pc);

            // Finally, jump to address n16
            pc = n16;
        } break;
        

        // 0xC4 - 0xD4, 0xCC - 0xDC: CALL cc, n16
        // Call address n16 depending on some condition
        // 3 bytes, 24/12 cycles (if cond met / not met)
        
        case (0xC4):    // CALL NZ, n16
        {
            // First, read two bytes to get n16
            // Must always do this in order to finish reading the complete instruction
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Then, push/jump if condition met
            if (!af.z) {
                sp--;
                sp--;
                bus->write_word(sp, pc);

                // Finally, jump to address n16
                pc = n16;
            }
        } break;
        
        case (0xD4):    // CALL NC, n16
        {
            // First, read two bytes to get n16
            // Must always do this in order to finish reading the complete instruction
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Then, push/jump if condition met
            if (!af.c) {
                sp--;
                sp--;
                bus->write_word(sp, pc);

                // Finally, jump to address n16
                pc = n16;
            }
        } break;
        
        case (0xCC):    // CALL Z, n16
        {
            // First, read two bytes to get n16
            // Must always do this in order to finish reading the complete instruction
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Then, push/jump if condition met
            if (af.z) {
                sp--;
                sp--;
                bus->write_word(sp, pc);

                // Finally, jump to address n16
                pc = n16;
            }
        } break;
        
        case (0xDC):    // CALL C, n16
        {
            // First, read two bytes to get n16
            // Must always do this in order to finish reading the complete instruction
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            // Then, push/jump if condition met
            if (af.c) {
                sp--;
                sp--;
                bus->write_word(sp, pc);

                // Finally, jump to address n16
                pc = n16;
            }
        } break;



        // 0xC1 - 0xF1: POP r16
        // Pop the stack and place the value in r16
        // 1 byte, 12 cycles

        case (0xC1):    // POP BC
        {
            bc.reg = bus->read_word(sp);
            sp++;
            sp++;
        } break;

        case (0xD1):    // POP DE
        {
            de.reg = bus->read_word(sp);
            sp++;
            sp++;
        } break;

        case (0xE1):    // POP HL
        {
            hl.reg = bus->read_word(sp);
            sp++;
            sp++;
        } break;

        case (0xF1):    // POP AF
        {
            af.reg = bus->read_word(sp) & 0xFFF0;   // ALways hard code last 4 bits to 0
            sp++;
            sp++;
        } break;


        // 0xC5 - 0xF5: PUSH r16
        // Push the value in r16 onto the stack
        // 1 byte, 16 cycles

        case (0xC5):    // PUSH BC
        {
            sp--;
            sp--;
            bus->write_word(sp, bc.reg);
        } break;

        case (0xD5):    // PUSH DE
        {
            sp--;
            sp--;
            bus->write_word(sp, de.reg);
        } break;

        case (0xE5):    // PUSH HL
        {
            sp--;
            sp--;
            bus->write_word(sp, hl.reg);
        } break;

        case (0xF5):    // PUSH AF
        {
            sp--;
            sp--;
            bus->write_word(sp, af.reg);
        } break;


        // 0xF8: LD HL, SP + e8
        // Load signed value e8 + SP to HL register
        // 2 bytes, 12 cycles
        case (0xF8):    // LD HL, SP + e8
        {
            // Flags
            // Z    0
            // N    0
            // H    Set if overflow from bit 3.
            // C    Set if overflow from bit 7.

            uint8_t e8 = bus->read_byte(pc);
            pc++;

            hl.reg = sp + (int8_t) e8;

            // Check for the half carry of only the lowest 8 bits
            int8_t h = (((sp & 0xFF) ^ e8 ^ (hl.reg & 0xFF)) & 0x10) >> 4;
            // To check for the carry, check if the output overflows (when e8 is positive)
            int8_t c = ((sp & 0xFF) + e8) > 0xFF;

            setFlags(0, 0, h, c);
        } break;


        // 0xF9: LD SP, HL
        // Load HL register into SP
        // 1 byte, 8 cycles
        case (0xF9):    // LD HL, SP + e8
        {
            sp = hl.reg;
        } break;



        // 0xE0: LDH [a8], A
        // Copy value in A reg into [0xFF00 + n8]
        // 2 bytes, 12 cycles
        case (0xE0):    // LDH [a8], A
        {
            uint8_t a8 = bus->read_byte(pc);
            pc++;

            bus->write_byte(0xFF00 + a8, af.hi);
        } break;

        // 0xF0: LDH A, [a8]
        // Copy value in [0xFF00 + n8] into A reg
        // 2 bytes, 12 cycles
        case (0xF0):    // LDH A, [a8]
        {
            uint8_t a8 = bus->read_byte(pc);
            pc++;

            af.hi = bus->read_byte(0xFF00 + a8);
        } break;

        // 0xE3: LDH [C], A
        // Copy value in A reg into [$FF00 + C]
        // 1 byte, 8 cycles
        case (0xE3):    // LDH [C], A
        {
            bus->write_byte(0xFF00 + bc.lo, af.hi);
        } break;

        // 0xF3: LDH A, [C]
        // Copy byte at [$FF00 + C] into reg A
        // 1 byte, 8 cycles
        case (0xF3):    // LDH A, [C]
        {
            af.hi = bus->read_byte(0xFF00 + bc.lo);
        } break;



        // 0xEA: LD [n16], A
        // Load value in A reg into [n16]
        // 3 bytes, 16 cycles
        case (0xEA):    // LD [n16], A
        {
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            bus->write_byte(n16, af.hi);
        } break;

        // 0xFA: LD A, [n16]
        // Load value at [n16] into A reg
        // 3 bytes, 16 cycles
        case (0xFA):    // LD A, [n16]
        {
            uint16_t n16 = bus->read_word(pc);
            pc++;
            pc++;

            af.hi = bus->read_byte(n16);
        } break;



        // 0xC6 - 0xF6, oxCE - 0xFE: ALU A, n8
        // Same ALU operatiosn in Block 2, but with n8 instead of r8
        // 2 bytes, 8 cycles

        case (0xC6):    // ADD A, n8
        {
            uint8_t n8 = bus->read_byte(pc);
            pc++;

            ADD_r8(n8);
        } break;

        case (0xCE):    // ADC A, n8
        {
            uint8_t n8 = bus->read_byte(pc);
            pc++;

            ADC_r8(n8);
        } break;

        case (0xD6):    // SUB A, n8
        {
            uint8_t n8 = bus->read_byte(pc);
            pc++;

            SUB_r8(n8);
        } break;

        case (0xDE):    // SBC A, n8
        {
            uint8_t n8 = bus->read_byte(pc);
            pc++;

            SBC_r8(n8);
        } break;

        case (0xE6):    // AND A, n8
        {
            uint8_t n8 = bus->read_byte(pc);
            pc++;

            AND_r8(n8);
        } break;

        case (0xEE):    // XOR A, n8
        {
            uint8_t n8 = bus->read_byte(pc);
            pc++;

            XOR_r8(n8);
        } break;

        case (0xF6):    // OR A, n8
        {
            uint8_t n8 = bus->read_byte(pc);
            pc++;

            OR_r8(n8);
        } break;

        case (0xFE):    // CP A, n8
        {
            uint8_t n8 = bus->read_byte(pc);
            pc++;

            CP_r8(n8);
        } break;



        // 0xE8: ADD SP, e8
        // Add signed value e8 to SP
        // 2 bytes, 16 cycles
        case (0xE8):    // ADD SP, e8
        {
            // Flags
            // Z    0
            // N    0
            // H    Set if overflow from bit 3.
            // C    Set if overflow from bit 7.

            uint8_t e8 = bus->read_byte(pc);
            pc++;

            uint16_t result = sp + (int8_t) e8;

            // Check for the half carry of only the lowest 8 bits
            int8_t h = (((sp & 0xFF) ^ e8 ^ (result & 0xFF)) & 0x10) >> 4;
            // To check for the carry, check if the output overflows (when e8 is positive)
            int8_t c = ((sp & 0xFF) + e8) > 0xFF;

            sp = result;

            setFlags(0, 0, h, c);
        } break;



        // 0xCB: PREFIX
        // This specifies that the instruction will be a prefix command. Next byte determines which instruction
        // 1 byte, 4 cycles
        case (0xCB):    // PREFIX
        {
            // Fetch prefix instruction
            opcode = bus->read_byte(pc);
            pc++;

            // Execute
            switch (opcode) {

                // 0x00 - 0x07: RLC r8
                // Rotate r8 left
                // 2 bytes, 8 cycles
                case (0x00):    // RLC B
                {
                    RLC_r8(&bc.hi);
                } break;
                case (0x01):    // RLC C
                {
                    RLC_r8(&bc.lo);
                } break;
                case (0x02):    // RLC D
                {
                    RLC_r8(&de.hi);
                } break;
                case (0x03):    // RLC E
                {
                    RLC_r8(&de.lo);
                } break;
                case (0x04):    // RLC H
                {
                    RLC_r8(&hl.hi);
                } break;
                case (0x05):    // RLC L
                {
                    RLC_r8(&hl.lo);
                } break;
                case (0x06):    // RLC [HL]
                {
                    // 2 bytes, 16 cycles
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = (val == 0);
                    int8_t c = val >> 7;

                    val = (val << 1) | (c & 0x01);

                    bus->write_byte(hl.reg, val);

                    setFlags(z, 0, 0, c);
                } break;
                case (0x07):    // RLC A
                {
                    RLC_r8(&af.hi);
                } break;

                // 0x08 - 0x0F: RRC r8
                // Rotate r8 right
                // 2 bytes, 8 cycles
                case (0x08):    // RRC B
                {
                    RRC_r8(&bc.hi);
                } break;
                case (0x09):    // RRC C
                {
                    RRC_r8(&bc.lo);
                } break;
                case (0x0A):    // RRC D
                {
                    RRC_r8(&de.hi);
                } break;
                case (0x0B):    // RRC E
                {
                    RRC_r8(&de.lo);
                } break;
                case (0x0C):    // RRC H
                {
                    RRC_r8(&hl.hi);
                } break;
                case (0x0D):    // RRC L
                {
                    RRC_r8(&hl.lo);
                } break;
                case (0x0E):    // RRC [HL]
                {
                    // 2 bytes, 16 cycles
                    // TODO: Find out why this doesn't pass test 11
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = (val == 0);
                    int8_t c = val & 0x01;

                    val = (c << 7) | (val >> 1);

                    bus->write_byte(hl.reg, val);

                    setFlags(z, 0, 0, c);
                }
                case (0x0F):    // RRC A
                {
                    RRC_r8(&af.hi);
                } break;
                

                // 0x10 - 0x17: RL r8
                case (0x10):    // RL B
                {
                    RL_r8(&bc.hi);
                } break;
                case (0x11):    // RL C
                {
                    RL_r8(&bc.lo);
                } break;
                case (0x12):    // RL D
                {
                    RL_r8(&de.hi);
                } break;
                case (0x13):    // RL E
                {
                    RL_r8(&de.lo);
                } break;
                case (0x14):    // RL H
                {
                    RL_r8(&hl.hi);
                } break;
                case (0x15):    // RL L
                {
                    RL_r8(&hl.lo);
                } break;
                case (0x16):    // RL [HL]
                {
                    // 2 bytes, 16 cycles
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t c = val >> 7;
                    uint8_t current_c = af.c;
                    val = (val << 1) | (current_c & 0x01);
                    int8_t z = (val == 0);

                    bus->write_byte(hl.reg, val);

                    setFlags(z, 0, 0, c);
                } break;
                case (0x17):    // RL A
                {
                    RL_r8(&af.hi);
                } break;

                // 0x18 - 0x1F: RR r8
                // Rotate right r8 through the carry bit. Think of it as a 9-bit register
                // 2 bytes, 8 cycles
                case (0x18):    // RR B
                {
                    RR_r8(&bc.hi);
                } break;
                case (0x19):    // RR C
                {
                    RR_r8(&bc.lo);
                } break;
                case (0x1A):    // RR D
                {
                    RR_r8(&de.hi);
                } break;
                case (0x1B):    // RR E
                {
                    RR_r8(&de.lo);
                } break;
                case (0x1C):    // RR H
                {
                    RR_r8(&hl.hi);
                } break;
                case (0x1D):    // RR L
                {
                    RR_r8(&hl.lo);
                } break;
                case (0x1E):    // RR [HL]
                {
                    // 2 bytes, 16 cycles
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t c = val & 0x01;
                    uint8_t current_c = af.c;
                    uint8_t result = (val >> 1) + (current_c << 7);

                    bus->write_byte(hl.reg, result);
                    int8_t z = (result == 0);

                    setFlags(z, 0, 0, c);
                } break;
                case (0x1F):    // RR A
                {
                    RR_r8(&af.hi);
                } break;


                // 0x20 - 0x27: SLA r8
                // Shift r8 left arithmetically
                // 2 bytes, 8 cycles
                case (0x20):    // SLA B
                {
                    SLA_r8(&bc.hi);
                } break;
                case (0x21):    // SLA C
                {
                    SLA_r8(&bc.lo);
                } break;
                case (0x22):    // SLA D
                {
                    SLA_r8(&de.hi);
                } break;
                case (0x23):    // SLA E
                {
                    SLA_r8(&de.lo);
                } break;
                case (0x24):    // SLA H
                {
                    SLA_r8(&hl.hi);
                } break;
                case (0x25):    // SLA L
                {
                    SLA_r8(&hl.lo);
                } break;
                case (0x26):    // SLA [HL]
                {
                    // 2 bytes, 16 cycles
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t c = val >> 7;
                    
                    val <<= 1;

                    uint8_t z = (val == 0);
                    bus->write_byte(hl.reg, val);
                    setFlags(z, 0, 0, c);
                } break;
                case (0x27):    // SLA A
                {
                    SLA_r8(&af.hi);
                } break;

                
                // 0x28 - 0x2F: SRA r8
                // Shift r8 right arithmetically
                // 2 bytes, 8 cycles
                case (0x28):    // SRA B
                {
                    SRA_r8(&bc.hi);
                } break;
                case (0x29):    // SRA C
                {
                    SRA_r8(&bc.lo);
                } break;
                case (0x2A):    // SRA D
                {
                    SRA_r8(&de.hi);
                } break;
                case (0x2B):    // SRA E
                {
                    SRA_r8(&de.lo);
                } break;
                case (0x2C):    // SRA H
                {
                    SRA_r8(&hl.hi);
                } break;
                case (0x2D):    // SRA L
                {
                    SRA_r8(&hl.lo);
                } break;
                case (0x2E):    // SRA [HL]
                {
                    // 2 bytes, 16 cycles
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t c = val & 0x01;
                    
                    val = (val & 0x80) | (val >> 1);

                    uint8_t z = (val == 0);
                    bus->write_byte(hl.reg, val);
                    setFlags(z, 0, 0, c);
                } break;
                case (0x2F):    // SRA A
                {
                    SRA_r8(&af.hi);
                } break;


                // 0x30 - 0x37: SWAP r8
                // Swap upper 4 bits with lower 4 bits
                // 2 bytes, 8 cycles
                case (0x30):    // SWAP B
                {
                    SWAP_r8(&bc.hi);
                } break;
                case (0x31):    // SWAP C
                {
                    SWAP_r8(&bc.lo);
                } break;
                case (0x32):    // SWAP D
                {
                    SWAP_r8(&de.hi);
                } break;
                case (0x33):    // SWAP E
                {
                    SWAP_r8(&de.lo);
                } break;
                case (0x34):    // SWAP H
                {
                    SWAP_r8(&hl.hi);
                } break;
                case (0x35):    // SWAP L
                {
                    SWAP_r8(&hl.lo);
                } break;
                case (0x36):    // SWAP [HL]
                {
                    // 2 bytes, 16 cycles

                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = (val == 0);
                    bus->write_byte(hl.reg, (val >> 4) | (val << 4));

                    setFlags(z, 0, 0, 0);
                } break;
                case (0x37):    // SWAP A
                {
                    SWAP_r8(&af.hi);
                } break;

                // 0x38 - 0x3F: SRL r8
                // Shift right logically r8 (no wrap)
                // 2 bytes, 8 cycles
                case (0x38):    // SRL B
                {
                    SRL_r8(&bc.hi);
                } break;
                case (0x39):    // SRL C
                {
                    SRL_r8(&bc.lo);
                } break;
                case (0x3A):    // SRL D
                {
                    SRL_r8(&de.hi);
                } break;
                case (0x3B):    // SRL E
                {
                    SRL_r8(&de.lo);
                } break;
                case (0x3C):    // SRL H
                {
                    SRL_r8(&hl.hi);
                } break;
                case (0x3D):    // SRL L
                {
                    SRL_r8(&hl.lo);
                } break;
                case (0x3E):    // SRL [HL]
                {
                    // 2 bytes, 16 cycles
                    uint8_t val = bus->read_byte(hl.reg);
                    int8_t c = val & 0x01;
                    val >>= 1;
                    int8_t z = (val == 0);
                    
                    bus->write_byte(hl.reg, val);

                    setFlags(z, 0, 0, c);
                } break;
                case (0x3F):    // SRL A
                {
                    SRL_r8(&af.hi);
                } break;


                // 0x40 - 0x47: BIT 0, r8
                case (0x40):    // BIT 0, B
                {
                    BIT(0, &bc.hi);
                } break;
                case (0x41):    // BIT 0, C
                {
                    BIT(0, &bc.lo);
                } break;
                case (0x42):    // BIT 0, D
                {
                    BIT(0, &de.hi);
                } break;
                case (0x43):    // BIT 0, E
                {
                    BIT(0, &de.lo);
                } break;
                case (0x44):    // BIT 0, H
                {
                    BIT(0, &hl.hi);
                } break;
                case (0x45):    // BIT 0, L
                {
                    BIT(0, &hl.lo);
                } break;
                case (0x46):    // BIT 0, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = !(val & 0x01);
                    setFlags(z, 0, 1, -1);
                } break;
                case (0x47):    // BIT 0, A
                {
                    BIT(0, &af.hi);
                } break;

                // 0x48 - 0x4F: BIT 1, r8
                case (0x48):    // BIT 1, B
                {
                    BIT(1, &bc.hi);
                } break;
                case (0x49):    // BIT 1, C
                {
                    BIT(1, &bc.lo);
                } break;
                case (0x4A):    // BIT 1, D
                {
                    BIT(1, &de.hi);
                } break;
                case (0x4B):    // BIT 1, E
                {
                    BIT(1, &de.lo);
                } break;
                case (0x4C):    // BIT 1, H
                {
                    BIT(1, &hl.hi);
                } break;
                case (0x4D):    // BIT 1, L
                {
                    BIT(1, &hl.lo);
                } break;
                case (0x4E):    // BIT 1, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = !(val & 0x02);
                    setFlags(z, 0, 1, -1);
                } break;
                case (0x4F):    // BIT 1, A
                {
                    BIT(1, &af.hi);
                } break;

                // 0x50 - 0x57: BIT 2, r8
                case (0x50):    // BIT 2, B
                {
                    BIT(2, &bc.hi);
                } break;
                case (0x51):    // BIT 2, C
                {
                    BIT(2, &bc.lo);
                } break;
                case (0x52):    // BIT 2, D
                {
                    BIT(2, &de.hi);
                } break;
                case (0x53):    // BIT 2, E
                {
                    BIT(2, &de.lo);
                } break;
                case (0x54):    // BIT 2, H
                {
                    BIT(2, &hl.hi);
                } break;
                case (0x55):    // BIT 2, L
                {
                    BIT(2, &hl.lo);
                } break;
                case (0x56):    // BIT 2, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = !(val & 0x04);
                    setFlags(z, 0, 1, -1);
                } break;
                case (0x57):    // BIT 2, A
                {
                    BIT(2, &af.hi);
                } break;

                // 0x58 - 0x5F: BIT 3, r8
                case (0x58):    // BIT 3, B
                {
                    BIT(3, &bc.hi);
                } break;
                case (0x59):    // BIT 3, C
                {
                    BIT(3, &bc.lo);
                } break;
                case (0x5A):    // BIT 3, D
                {
                    BIT(3, &de.hi);
                } break;
                case (0x5B):    // BIT 3, E
                {
                    BIT(3, &de.lo);
                } break;
                case (0x5C):    // BIT 3, H
                {
                    BIT(3, &hl.hi);
                } break;
                case (0x5D):    // BIT 3, L
                {
                    BIT(3, &hl.lo);
                } break;
                case (0x5E):    // BIT 3, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = !(val & 0x08);
                    setFlags(z, 0, 1, -1);
                } break;
                case (0x5F):    // BIT 3, A
                {
                    BIT(3, &af.hi);
                } break;

                // 0x60 - 0x67: BIT 4, r8
                case (0x60):    // BIT 4, B
                {
                    BIT(4, &bc.hi);
                } break;
                case (0x61):    // BIT 4, C
                {
                    BIT(4, &bc.lo);
                } break;
                case (0x62):    // BIT 4, D
                {
                    BIT(4, &de.hi);
                } break;
                case (0x63):    // BIT 4, E
                {
                    BIT(4, &de.lo);
                } break;
                case (0x64):    // BIT 4, H
                {
                    BIT(4, &hl.hi);
                } break;
                case (0x65):    // BIT 4, L
                {
                    BIT(4, &hl.lo);
                } break;
                case (0x66):    // BIT 4, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = !(val & 0x10);
                    setFlags(z, 0, 1, -1);
                } break;
                case (0x67):    // BIT 4, A
                {
                    BIT(4, &af.hi);
                } break;

                // 0x68 - 0x6F: BIT 5, r8
                case (0x68):    // BIT 5, B
                {
                    BIT(5, &bc.hi);
                } break;
                case (0x69):    // BIT 5, C
                {
                    BIT(5, &bc.lo);
                } break;
                case (0x6A):    // BIT 5, D
                {
                    BIT(5, &de.hi);
                } break;
                case (0x6B):    // BIT 5, E
                {
                    BIT(5, &de.lo);
                } break;
                case (0x6C):    // BIT 5, H
                {
                    BIT(5, &hl.hi);
                } break;
                case (0x6D):    // BIT 5, L
                {
                    BIT(5, &hl.lo);
                } break;
                case (0x6E):    // BIT 5, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = !(val & 0x20);
                    setFlags(z, 0, 1, -1);
                } break;
                case (0x6F):    // BIT 5, A
                {
                    BIT(5, &af.hi);
                } break;

                // 0x70 - 0x77: BIT 6, r8
                case (0x70):    // BIT 6, B
                {
                    BIT(6, &bc.hi);
                } break;
                case (0x71):    // BIT 6, C
                {
                    BIT(6, &bc.lo);
                } break;
                case (0x72):    // BIT 6, D
                {
                    BIT(6, &de.hi);
                } break;
                case (0x73):    // BIT 6, E
                {
                    BIT(6, &de.lo);
                } break;
                case (0x74):    // BIT 6, H
                {
                    BIT(6, &hl.hi);
                } break;
                case (0x75):    // BIT 6, L
                {
                    BIT(6, &hl.lo);
                } break;
                case (0x76):    // BIT 6, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = !(val & 0x40);
                    setFlags(z, 0, 1, -1);
                } break;
                case (0x77):    // BIT 6, A
                {
                    BIT(6, &af.hi);
                } break;

                // 0x78 - 0x7F: BIT 7, r8
                case (0x78):    // BIT 7, B
                {
                    BIT(7, &bc.hi);
                } break;
                case (0x79):    // BIT 7, C
                {
                    BIT(7, &bc.lo);
                } break;
                case (0x7A):    // BIT 7, D
                {
                    BIT(7, &de.hi);
                } break;
                case (0x7B):    // BIT 7, E
                {
                    BIT(7, &de.lo);
                } break;
                case (0x7C):    // BIT 7, H
                {
                    BIT(7, &hl.hi);
                } break;
                case (0x7D):    // BIT 7, L
                {
                    BIT(7, &hl.lo);
                } break;
                case (0x7E):    // BIT 7, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);

                    int8_t z = !(val & 0x80);
                    setFlags(z, 0, 1, -1);
                } break;
                case (0x7F):    // BIT 7, A
                {
                    BIT(7, &af.hi);
                } break;


                // 0x80 - 0x87: RES 0, r8
                case (0x80):    // RES 0, B
                {
                    RES(0, &bc.hi);
                } break;
                case (0x81):    // RES 0, C
                {
                    RES(0, &bc.lo);
                } break;
                case (0x82):    // RES 0, D
                {
                    RES(0, &de.hi);
                } break;
                case (0x83):    // RES 0, E
                {
                    RES(0, &de.lo);
                } break;
                case (0x84):    // RES 0, H
                {
                    RES(0, &hl.hi);
                } break;
                case (0x85):    // RES 0, L
                {
                    RES(0, &hl.lo);
                } break;
                case (0x86):    // RES 0, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val &= 0xFE;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0x87):    // RES 0, A
                {
                    RES(0, &af.hi);
                } break;

                // 0x88 - 0x8F: RES 1, r8
                case (0x88):    // RES 1, B
                {
                    RES(1, &bc.hi);
                } break;
                case (0x89):    // RES 1, C
                {
                    RES(1, &bc.lo);
                } break;
                case (0x8A):    // RES 1, D
                {
                    RES(1, &de.hi);
                } break;
                case (0x8B):    // RES 1, E
                {
                    RES(1, &de.lo);
                } break;
                case (0x8C):    // RES 1, H
                {
                    RES(1, &hl.hi);
                } break;
                case (0x8D):    // RES 1, L
                {
                    RES(1, &hl.lo);
                } break;
                case (0x8E):    // RES 1, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val &= 0xFD;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0x8F):    // RES 1, A
                {
                    RES(1, &af.hi);
                } break;

                // 0x90 - 0x97: RES 2, r8
                case (0x90):    // RES 2, B
                {
                    RES(2, &bc.hi);
                } break;
                case (0x91):    // RES 2, C
                {
                    RES(2, &bc.lo);
                } break;
                case (0x92):    // RES 2, D
                {
                    RES(2, &de.hi);
                } break;
                case (0x93):    // RES 2, E
                {
                    RES(2, &de.lo);
                } break;
                case (0x94):    // RES 2, H
                {
                    RES(2, &hl.hi);
                } break;
                case (0x95):    // RES 2, L
                {
                    RES(2, &hl.lo);
                } break;
                case (0x96):    // RES 2, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val &= 0xFB;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0x97):    // RES 2, A
                {
                    RES(2, &af.hi);
                } break;

                // 0x98 - 0x9F: RES 3, r8
                case (0x98):    // RES 3, B
                {
                    RES(3, &bc.hi);
                } break;
                case (0x99):    // RES 3, C
                {
                    RES(3, &bc.lo);
                } break;
                case (0x9A):    // RES 3, D
                {
                    RES(3, &de.hi);
                } break;
                case (0x9B):    // RES 3, E
                {
                    RES(3, &de.lo);
                } break;
                case (0x9C):    // RES 3, H
                {
                    RES(3, &hl.hi);
                } break;
                case (0x9D):    // RES 3, L
                {
                    RES(3, &hl.lo);
                } break;
                case (0x9E):    // RES 3, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val &= 0xF7;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0x9F):    // RES 3, A
                {
                    RES(3, &af.hi);
                } break;

                // 0xA0 - 0xA7: RES 4, r8
                case (0xA0):    // RES 4, B
                {
                    RES(4, &bc.hi);
                } break;
                case (0xA1):    // RES 4, C
                {
                    RES(4, &bc.lo);
                } break;
                case (0xA2):    // RES 4, D
                {
                    RES(4, &de.hi);
                } break;
                case (0xA3):    // RES 4, E
                {
                    RES(4, &de.lo);
                } break;
                case (0xA4):    // RES 4, H
                {
                    RES(4, &hl.hi);
                } break;
                case (0xA5):    // RES 4, L
                {
                    RES(4, &hl.lo);
                } break;
                case (0xA6):    // RES 4, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val &= 0xEF;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xA7):    // RES 4, A
                {
                    RES(4, &af.hi);
                } break;

                // 0xA8 - 0xAF: RES 5, r8
                case (0xA8):    // RES 5, B
                {
                    RES(5, &bc.hi);
                } break;
                case (0xA9):    // RES 5, C
                {
                    RES(5, &bc.lo);
                } break;
                case (0xAA):    // RES 5, D
                {
                    RES(5, &de.hi);
                } break;
                case (0xAB):    // RES 5, E
                {
                    RES(5, &de.lo);
                } break;
                case (0xAC):    // RES 5, H
                {
                    RES(5, &hl.hi);
                } break;
                case (0xAD):    // RES 5, L
                {
                    RES(5, &hl.lo);
                } break;
                case (0xAE):    // RES 5, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val &= 0xDF;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xAF):    // RES 5, A
                {
                    RES(5, &af.hi);
                } break;

                // 0xB0 - 0xB7: RES 6, r8
                case (0xB0):    // RES 6, B
                {
                    RES(6, &bc.hi);
                } break;
                case (0xB1):    // RES 6, C
                {
                    RES(6, &bc.lo);
                } break;
                case (0xB2):    // RES 6, D
                {
                    RES(6, &de.hi);
                } break;
                case (0xB3):    // RES 6, E
                {
                    RES(6, &de.lo);
                } break;
                case (0xB4):    // RES 6, H
                {
                    RES(6, &hl.hi);
                } break;
                case (0xB5):    // RES 6, L
                {
                    RES(6, &hl.lo);
                } break;
                case (0xB6):    // RES 6, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val &= 0xBF;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xB7):    // RES 6, A
                {
                    RES(6, &af.hi);
                } break;

                // 0xB8 - 0xBF: RES 7, r8
                case (0xB8):    // RES 7, B
                {
                    RES(7, &bc.hi);
                } break;
                case (0xB9):    // RES 7, C
                {
                    RES(7, &bc.lo);
                } break;
                case (0xBA):    // RES 7, D
                {
                    RES(7, &de.hi);
                } break;
                case (0xBB):    // RES 7, E
                {
                    RES(7, &de.lo);
                } break;
                case (0xBC):    // RES 7, H
                {
                    RES(7, &hl.hi);
                } break;
                case (0xBD):    // RES 7, L
                {
                    RES(7, &hl.lo);
                } break;
                case (0xBE):    // RES 7, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val &= 0x7F;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xBF):    // RES 7, A
                {
                    RES(7, &af.hi);
                } break;


                // 0xC0 - 0xC7: SET 0, r8
                case (0xC0):    // SET 0, B
                {
                    SET(0, &bc.hi);
                } break;
                case (0xC1):    // SET 0, C
                {
                    SET(0, &bc.lo);
                } break;
                case (0xC2):    // SET 0, D
                {
                    SET(0, &de.hi);
                } break;
                case (0xC3):    // SET 0, E
                {
                    SET(0, &de.lo);
                } break;
                case (0xC4):    // SET 0, H
                {
                    SET(0, &hl.hi);
                } break;
                case (0xC5):    // SET 0, L
                {
                    SET(0, &hl.lo);
                } break;
                case (0xC6):    // SET 0, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val |= 0x01;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xC7):    // SET 0, A
                {
                    SET(0, &af.hi);
                } break;

                // 0xC8 - 0xCF: SET 1, r8
                case (0xC8):    // SET 1, B
                {
                    SET(1, &bc.hi);
                } break;
                case (0xC9):    // SET 1, C
                {
                    SET(1, &bc.lo);
                } break;
                case (0xCA):    // SET 1, D
                {
                    SET(1, &de.hi);
                } break;
                case (0xCB):    // SET 1, E
                {
                    SET(1, &de.lo);
                } break;
                case (0xCC):    // SET 1, H
                {
                    SET(1, &hl.hi);
                } break;
                case (0xCD):    // SET 1, L
                {
                    SET(1, &hl.lo);
                } break;
                case (0xCE):    // SET 1, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val |= 0x02;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xCF):    // SET 1, A
                {
                    SET(1, &af.hi);
                } break;

                // 0xD0 - 0xD7: SET 2, r8
                case (0xD0):    // SET 2, B
                {
                    SET(2, &bc.hi);
                } break;
                case (0xD1):    // SET 2, C
                {
                    SET(2, &bc.lo);
                } break;
                case (0xD2):    // SET 2, D
                {
                    SET(2, &de.hi);
                } break;
                case (0xD3):    // SET 2, E
                {
                    SET(2, &de.lo);
                } break;
                case (0xD4):    // SET 2, H
                {
                    SET(2, &hl.hi);
                } break;
                case (0xD5):    // SET 2, L
                {
                    SET(2, &hl.lo);
                } break;
                case (0xD6):    // SET 2, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val |= 0x04;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xD7):    // SET 2, A
                {
                    SET(2, &af.hi);
                } break;

                // 0xD8 - 0xDF: SET 3, r8
                case (0xD8):    // SET 3, B
                {
                    SET(3, &bc.hi);
                } break;
                case (0xD9):    // SET 3, C
                {
                    SET(3, &bc.lo);
                } break;
                case (0xDA):    // SET 3, D
                {
                    SET(3, &de.hi);
                } break;
                case (0xDB):    // SET 3, E
                {
                    SET(3, &de.lo);
                } break;
                case (0xDC):    // SET 3, H
                {
                    SET(3, &hl.hi);
                } break;
                case (0xDD):    // SET 3, L
                {
                    SET(3, &hl.lo);
                } break;
                case (0xDE):    // SET 3, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val |= 0x08;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xDF):    // SET 3, A
                {
                    SET(3, &af.hi);
                } break;

                // 0xE0 - 0xE7: SET 4, r8
                case (0xE0):    // SET 4, B
                {
                    SET(4, &bc.hi);
                } break;
                case (0xE1):    // SET 4, C
                {
                    SET(4, &bc.lo);
                } break;
                case (0xE2):    // SET 4, D
                {
                    SET(4, &de.hi);
                } break;
                case (0xE3):    // SET 4, E
                {
                    SET(4, &de.lo);
                } break;
                case (0xE4):    // SET 4, H
                {
                    SET(4, &hl.hi);
                } break;
                case (0xE5):    // SET 4, L
                {
                    SET(4, &hl.lo);
                } break;
                case (0xE6):    // SET 4, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val |= 0x10;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xE7):    // SET 4, A
                {
                    SET(4, &af.hi);
                } break;

                // 0xE8 - 0xEF: SET 5, r8
                case (0xE8):    // SET 5, B
                {
                    SET(5, &bc.hi);
                } break;
                case (0xE9):    // SET 5, C
                {
                    SET(5, &bc.lo);
                } break;
                case (0xEA):    // SET 5, D
                {
                    SET(5, &de.hi);
                } break;
                case (0xEB):    // SET 5, E
                {
                    SET(5, &de.lo);
                } break;
                case (0xEC):    // SET 5, H
                {
                    SET(5, &hl.hi);
                } break;
                case (0xED):    // SET 5, L
                {
                    SET(5, &hl.lo);
                } break;
                case (0xEE):    // SET 5, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val |= 0x20;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xEF):    // SET 5, A
                {
                    SET(5, &af.hi);
                } break;

                // 0xF0 - 0xF7: SET 6, r8
                case (0xF0):    // SET 6, B
                {
                    SET(6, &bc.hi);
                } break;
                case (0xF1):    // SET 6, C
                {
                    SET(6, &bc.lo);
                } break;
                case (0xF2):    // SET 6, D
                {
                    SET(6, &de.hi);
                } break;
                case (0xF3):    // SET 6, E
                {
                    SET(6, &de.lo);
                } break;
                case (0xF4):    // SET 6, H
                {
                    SET(6, &hl.hi);
                } break;
                case (0xF5):    // SET 6, L
                {
                    SET(6, &hl.lo);
                } break;
                case (0xF6):    // SET 6, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val |= 0x40;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xF7):    // SET 6, A
                {
                    SET(6, &af.hi);
                } break;

                // 0xF8 - 0xFF: SET 7, r8
                case (0xF8):    // SET 7, B
                {
                    SET(7, &bc.hi);
                } break;
                case (0xF9):    // SET 7, C
                {
                    SET(7, &bc.lo);
                } break;
                case (0xFA):    // SET 7, D
                {
                    SET(7, &de.hi);
                } break;
                case (0xFB):    // SET 7, E
                {
                    SET(7, &de.lo);
                } break;
                case (0xFC):    // SET 7, H
                {
                    SET(7, &hl.hi);
                } break;
                case (0xFD):    // SET 7, L
                {
                    SET(7, &hl.lo);
                } break;
                case (0xFE):    // SET 7, [HL]
                {
                    uint8_t val = bus->read_byte(hl.reg);
                    val |= 0x80;
                    bus->write_byte(hl.reg, val);
                } break;
                case (0xFF):    // SET 7, A
                {
                    SET(7, &af.hi);
                } break;

                default:
                {
                    std::cerr << "Unimplemented Prefix Opcode: 0x" << std::hex << (int)opcode 
                            << " at PC: 0x" << (pc - 1) << std::endl;
                    std::exit(1); // Safely crash the emulator so you know exactly what to code next
                } break;
            }
        } break;
        



        default:
        {
            std::cerr << "Unimplemented Opcode: 0x" << std::hex << (int)opcode 
                    << " at PC: 0x" << (pc - 1) << std::endl;
            std::exit(1); // Safely crash the emulator so you know exactly what to code next
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

void Z80::ADD_r16(uint16_t* reg_a, uint16_t reg_b) {
    // Flags
    // Z    Unchanged
    // N    0
    // H    Set if overflow from bit 11.
    // C    Set if overflow from bit 15.

    uint16_t result = *reg_a + reg_b;

    // To get the half carry flag, we do (reg_a ^ reg_b ^ result)
    // The final result will have 1's in places where a bit overflowed into
    // Assume the following
    // reg_a  = 0x00001100
    // reg_b  = 0x00001001
    // result = 0x00010101
    // First XOR yields  00000101. 
    // Second XOR yields 00010000. Carry in 4th bit

    // To check for the half carry of a 16-bit number, check the value of the 12th bit
    int8_t h = ((*reg_a ^ reg_b ^ result) & 0x1000) >> 12;
    // To check for the carry, check if the output is less than reg_a
    int8_t c = result < reg_b;

    *reg_a += reg_b;
    setFlags(-1, 0, h, c);
}


void Z80::INC_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    Set if overflow from bit 3.
    // C    Unchanged

    int8_t h = ((*reg_a & 0x0F) == 0x0F);
    (*reg_a)++;
    int8_t z = (*reg_a == 0x00);
    setFlags(z, 0, h, -1);
    return;
}

void Z80::DEC_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    1
    // H    Set if borrow from bit 4.
    // C    Unchanged

    int8_t h = ((*reg_a & 0x0F) == 0x00);
    (*reg_a)--;
    int8_t z = (*reg_a == 0x00);
    setFlags(z, 1, h, -1);
    return;
}

void Z80::LD_r8(uint8_t* reg_a, uint8_t reg_b) {
    *reg_a = reg_b;
    return;
}

void Z80::ADD_r8(uint8_t reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    Set if overflow from bit 3.
    // C    Set if overflow from bit 7.
    uint8_t result = af.hi + reg_a;

    int8_t z = (result == 0);
    // To check for the half carry of a 8-bit number, check the value of the 4th bit
    int8_t h = ((reg_a ^ af.hi ^ result) & 0x10) >> 4;
    // To check for the carry, check if the output is less than reg_a
    // Carry is only present when there is an overflow
    int8_t c = result < reg_a;

    af.hi = result;
    setFlags(z, 0, h, c);
    return;
}

void Z80::ADC_r8(uint8_t reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    Set if overflow from bit 3.
    // C    Set if overflow from bit 7.
    int16_t result = af.hi + reg_a + af.c;

    // Check if half carry overflow
    int8_t h = ((reg_a & 0x0F) + (af.hi & 0x0F) + (af.c & 0x0F)) > 0x0F;
    // Check if overflow
    int8_t c = result > 0xFF;

    af.hi = (uint8_t) result;

    int8_t z = (af.hi == 0);
    setFlags(z, 0, h, c);
    return;
}

void Z80::SUB_r8(uint8_t reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    1
    // H    Set if borrow from bit 4.
    // C    Set if borrow from bit 8.
    uint8_t result = af.hi - reg_a;

    int8_t z = (result == 0);
    // To check for the half carry of a 8-bit number, check the value of the 4th bit
    int8_t h = ((reg_a ^ af.hi ^ result) & 0x10) >> 4;
    // To check for the carry, check if the subtracting operand is less than reg_a
    int8_t c = reg_a > af.hi;

    af.hi = result;
    setFlags(z, 1, h, c);
    return;
}

void Z80::SBC_r8(uint8_t reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    1
    // H    Set if borrow from bit 4.
    // C    Set if borrow from bit 8.
    int16_t result = af.hi - reg_a - af.c;

    // To check for the half carry of a 8-bit number, check the value of the 4th bit
    int8_t h = ((af.hi & 0x0F) - (reg_a & 0x0F) - af.c) < 0;
    // To check for the carry, check if the subtracting operand is less than reg_a
    int8_t c = result < 0;

    af.hi = (uint8_t) result;

    int8_t z = (af.hi == 0);
    setFlags(z, 1, h, c);
    return;
}

void Z80::AND_r8(uint8_t reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    1
    // C    0

    af.hi &= reg_a;
    int8_t z = (af.hi == 0);
    setFlags(z, 0, 1, 0);
    return;
}

void Z80::XOR_r8(uint8_t reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    0

    af.hi ^= reg_a;
    int8_t z = (af.hi == 0);
    setFlags(z, 0, 0, 0);
    return;
}

void Z80::OR_r8(uint8_t reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    0

    af.hi |= reg_a;
    int8_t z = (af.hi == 0);
    setFlags(z, 0, 0, 0);
    return;
}

void Z80::CP_r8(uint8_t reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    1
    // H    Set if borrow from bit 4.
    // C    Set if borrow from bit 8.
    uint8_t result = af.hi - reg_a;

    int8_t z = (result == 0);
    // To check for the half carry of a 8-bit number, check the value of the 4th bit
    int8_t h = ((reg_a ^ af.hi ^ result) & 0x10) >> 4;
    // To check for the carry, check if the subtracting operand is less than reg_a
    int8_t c = reg_a > af.hi;

    setFlags(z, 1, h, c);
    return;
}

void Z80::RLC_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    Set based on bit rotated out
    int8_t z = (*reg_a == 0);
    int8_t c = *reg_a >> 7;

    *reg_a = (*reg_a << 1) | (c & 0x01);

    setFlags(z, 0, 0, c);
}

void Z80::RRC_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    Set according to bit rotated out
    int8_t z = (*reg_a == 0);
    int8_t c = *reg_a & 0x01;

    *reg_a = (c << 7) | (*reg_a >> 1);

    setFlags(z, 0, 0, c);
}

void Z80::RL_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    Set according to bit rotated out

    int8_t c = *reg_a >> 7;
    uint8_t current_c = af.c;
    *reg_a = (*reg_a << 1) | (current_c & 0x01);
    int8_t z = (*reg_a == 0);

    setFlags(z, 0, 0, c);
}

void Z80::RR_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    Set if bit 0 is 1

    int8_t c = *reg_a & 0x01;
    uint8_t current_c = af.c;
    *reg_a = (*reg_a >> 1) | (current_c << 7);
    int8_t z = (*reg_a == 0);

    setFlags(z, 0, 0, c);
}

void Z80::SLA_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    Set based on bit shifted out
    int8_t c = *reg_a >> 7;
                    
    *reg_a <<= 1;

    uint8_t z = (*reg_a == 0);
    setFlags(z, 0, 0, c);
}

void Z80::SRA_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    Set based on bit shifted out
    int8_t c = *reg_a & 0x01;
                    
    *reg_a = (*reg_a & 0x80) | (*reg_a >> 1);

    uint8_t z = (*reg_a == 0);
    setFlags(z, 0, 0, c);
}

void Z80::SWAP_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    0

    int8_t z = (*reg_a == 0);
    *reg_a = (*reg_a >> 4) | (*reg_a << 4);

    setFlags(z, 0, 0, 0);
}

void Z80::SRL_r8(uint8_t* reg_a) {
    // Flags
    // Z    Set if result is 0
    // N    0
    // H    0
    // C    Set if bit 0 is 1

    int8_t c = *reg_a & 0x01;

    *reg_a >>= 1;

    int8_t z = (*reg_a == 0x00);
    setFlags(z, 0, 0, c);
    return;
}

void Z80::BIT(uint8_t bit, uint8_t* reg_a) {
    int8_t z = !(*reg_a & (0x01 << bit));
    setFlags(z, 0, 1, -1);
    return;
}

void Z80::RES(uint8_t bit, uint8_t* reg_a) {
    uint8_t mask = ~(0x01 << bit);
    *reg_a &= mask;
    return;
}

void Z80::SET(uint8_t bit, uint8_t* reg_a) {
    uint8_t mask = (0x01 << bit);
    *reg_a |= mask;
    return;
}