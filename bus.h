#ifndef BUS_H
#define BUS_H

#include <cstdint>


// Address for routing
#define BANK_0_START        0x0000  // 0x0000 - 0x00FF (Boot ROM) | 0x0000 - 0x3FFF (Game ROM)
#define BANK_N_START        0x4000  // 0x4000 - 0x7FFF
#define VRAM_START          0x8000  // 0x8000 - 0x97FF (Tile RAM) + 0x9800 - 0x9FFF (Background Map)
#define EXTERNAL_RAM_START  0xA000  // 0xA000 - 0xBFFF
#define WRAM_START          0xC000  // 0xC000 - 0xDFFF
#define ECHO_START          0xE000  // 0xE000 - 0xFDFF is marked for the echo RAM. Just ignore
#define OAM_START           0xFE00  // 0xFE00 - 0xFE9F: Object Attribute Memory
#define UNUSED_START        0xFEA0  // 0xFEA0 - 0xFEFF: Unused. Reading should return 0 and writing should do nothing
#define IO_START            0xFF00  // 0xFF00 - 0xFF7F
#define HRAM_START          0xFF80  // 0xFF80 - 0xFFFE
#define IER                 0xFFFF  // Interrupt Enabled Register

class Bus {
    private:
        // Memory map
        uint8_t map[0xFFFF];    // For now, we will use this to store all of our data


    public:
        Bus();

        uint8_t read_byte(uint16_t address);
        void write_byte(uint16_t address, uint8_t byte);

        uint16_t read_word(uint16_t address);
        void write_word(uint16_t address, uint16_t word);

};

#endif