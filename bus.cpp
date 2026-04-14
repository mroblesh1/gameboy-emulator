#include "bus.h"

#include <cstdint>

#define MEM_SIZE 0xFFFF


Bus::Bus() {
    // Empty memory map
    for (int i = 0; i < MEM_SIZE; i++) {
        map[i] = 0;
    }
}


uint8_t Bus::read_byte(uint16_t address) {
    if (address < BANK_N_START) {               // Game ROM
        // address stays the same
        // This should read from cartridge.cpp
        return map[address];
    } else if (address < VRAM_START) {          // Bank N ROM
        // address % BANK_N_START
        // This should read from cartridge.cpp
        return map[address];
    } else if (address < EXTERNAL_RAM_START) {  // VRAM
        // address % VRAM_START
        return map[address];
    } else if (address < WRAM_START) {          // External RAM
        // address % EXTERNAL_RAM_START
        return map[address];
    } else if (address < ECHO_START) {          // WRAM
        // address % WRAM_START
        return map[address];
    } else if (address < OAM_START) {           // Echo RAM
        // address % ECHO_START
        return map[address];
    } else if (address < UNUSED_START) {        // OAM
        // address % OAM_START
        return map[address];
    } else if (address < IO_START) {            // Unused
        // NO NEED TO ADDRESS
        return 0;
    } else if (address < HRAM_START) {          // IO Ports
        // address % IO_START
        return map[address];
    } else if (address < IER) {                 // HRAM
        // address % HRAM_START
        return map[address];
    } else {
        // read from the IER
        return map[address];
    }
}

void Bus::write_byte(uint16_t address, uint8_t byte) {
    if (address < BANK_N_START) {               // Game ROM
        // address stays the same
        // This should read from cartridge.cpp
        map[address] = byte;
    } else if (address < VRAM_START) {          // Bank N ROM
        // address % BANK_N_START
        // This should read from cartridge.cpp
        map[address] = byte;
    } else if (address < EXTERNAL_RAM_START) {  // VRAM
        // address % VRAM_START
        map[address] = byte;
    } else if (address < WRAM_START) {          // External RAM
        // address % EXTERNAL_RAM_START
        map[address] = byte;
    } else if (address < ECHO_START) {          // WRAM
        // address % WRAM_START
        map[address] = byte;
    } else if (address < OAM_START) {           // Echo RAM
        // address % ECHO_START
        map[address] = byte;
    } else if (address < UNUSED_START) {        // OAM
        // address % OAM_START
        map[address] = byte;
    } else if (address < IO_START) {            // Unused
        // NO NEED TO ADDRESS
        ;
    } else if (address < HRAM_START) {          // IO Ports
        // address % IO_START
        map[address] = byte;
    } else if (address < IER) {                 // HRAM
        // address % HRAM_START
        map[address] = byte;
    } else {
        // read from the IER
        map[address] = byte;
    }
    return;
}


uint16_t Bus::read_word(uint16_t address) {
    if (address < BANK_N_START) {               // Game ROM
        // address stays the same
        // This should read from cartridge.cpp
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else if (address < VRAM_START) {          // Bank N ROM
        // address % BANK_N_START
        // This should read from cartridge.cpp
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else if (address < EXTERNAL_RAM_START) {  // VRAM
        // address % VRAM_START
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else if (address < WRAM_START) {          // External RAM
        // address % EXTERNAL_RAM_START
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else if (address < ECHO_START) {          // WRAM
        // address % WRAM_START
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else if (address < OAM_START) {           // Echo RAM
        // address % ECHO_START
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else if (address < UNUSED_START) {        // OAM
        // address % OAM_START
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else if (address < IO_START) {            // Unused
        // NO NEED TO ADDRESS
        return 0;
    } else if (address < HRAM_START) {          // IO Ports
        // address % IO_START
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else if (address < IER) {                 // HRAM
        // address % HRAM_START
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    } else {
        // read from the IER
        return (map[address]) + ((uint16_t) map[address+1] << 8);
    }
}

void Bus::write_word(uint16_t address, uint16_t word) {
    if (address < BANK_N_START) {               // Game ROM
        // address stays the same
        // This should read from cartridge.cpp
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else if (address < VRAM_START) {          // Bank N ROM
        // address % BANK_N_START
        // This should read from cartridge.cpp
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else if (address < EXTERNAL_RAM_START) {  // VRAM
        // address % VRAM_START
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else if (address < WRAM_START) {          // External RAM
        // address % EXTERNAL_RAM_START
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else if (address < ECHO_START) {          // WRAM
        // address % WRAM_START
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else if (address < OAM_START) {           // Echo RAM
        // address % ECHO_START
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else if (address < UNUSED_START) {        // OAM
        // address % OAM_START
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else if (address < IO_START) {            // Unused
        // NO NEED TO ADDRESS
        ;
    } else if (address < HRAM_START) {          // IO Ports
        // address % IO_START
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else if (address < IER) {                 // HRAM
        // address % HRAM_START
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    } else {
        // read from the IER
        map[address] = word & 0xFF;
        map[address + 1] = word >> 8;
    }
    return;
}