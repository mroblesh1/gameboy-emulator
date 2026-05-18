#include "bus.h"

#include <iostream>
#include <fstream>
#include <vector>

#define MEM_SIZE 0xFFFF


Bus::Bus() {
    // Empty memory map
    for (int i = 0; i < MEM_SIZE; i++) {
        map[i] = 0;
    }
}

bool Bus::loadROM(const std::string& filename) {
    // Open the file in binary mode and move the file pointer to the end
    std::ifstream file(filename, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        std::cerr << "Error: Failed to open ROM file: " << filename << std::endl;
        return false;
    }

    // Get the file size and return the pointer back to the beginning
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    // Read the entire file into the buffer
    if (!file.read(buffer.data(), size)) {
        std::cerr << "Error: Failed to read ROM data." << std::endl;
        return false;
    }

    size_t romSize = static_cast<size_t>(size); // Used to clear warnings
    // Write the buffer into the emulator's address space starting at 0x0000
    for (size_t i = 0; i < romSize; ++i) {
        if (i >= 0x10000) break;
        map[i] = static_cast<uint8_t>(buffer[i]);
    }

    std::cout << "Successfully loaded " << size << " bytes into memory." << std::endl;
    return true;
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

        // For Blargg test ROMs, text is outputted via the Serial Data Register (0xFF01)
        // The Serial Transfer Control (0xFF02) transmits from SDR when write value is 0x81
        if (address == 0xFF02 && byte == 0x81) {
            // Grab character in SDR
            char character = static_cast<char>(map[0xFF01]);

            std::cout << character << std::flush;

            // Clear control register
            map[0xFF02] = 0x00;
        }
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