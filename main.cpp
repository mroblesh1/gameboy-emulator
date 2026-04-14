// Game Boy (DMG) specs

// ---- Clocks/Timing ---- //
// Master Clock	4.194304 MHz1   (up to 8.388608 MhZ for GBC)
// 0.238 us for the period
// System Clock	1/4 the frequency of Master Clock
// this would be 0.953 us for the period

// Horizontal sync	9.198 KHz
// 0.1087 ms for the period
// Vertical sync	59.73 Hz
// 16.7 ms for the period
// Seems to refresh @ 60 fps

// ---- CPU ---- //
// CPU	8-bit 8080-like Sharp CPU (speculated to be a SM83 core)

// ---- Memory ---- //
// Work RAM	    8 KiB	(32 KiB (4 + 7 × 4 KiB) for GBC)
// Video RAM	8 KiB	(16 KiB (2 × 8 KiB) for GBC)


// ---- Misc ---- // (don't expect to emulate)
// Power	DC 6V, 0.7 W	(3V, 0.6 W for GBC)

// ---- I/O ---- //
// Display //
// Screen	LCD 4.7 × 4.3 cm	(TFT 4.4 × 4 cm for GBC)
// Resolution	160 × 144

// Graphics //
// OBJ ("sprites")	8 × 8 or 8 × 16 ; max 40 per screen, 10 per line
// Palettes	BG: 1 × 4, OBJ: 2 × 3	(BG: 8 × 4, OBJ: 8 × 33 for GBC)
// Colors	4 shades of green	[32768 colors (15-bit RGB) for GBC]

// Audio //
// Sound	4 channels with stereo output

// Gamepad //


#include "cpu.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    Bus bus;
    Z80 z80(&bus);
    
    std::cout << "Initial Dump" << std::endl;
    z80.dumpRegs();
    std::cout << "\n";

    z80.LD_n8(&z80.af.hi, 1);
    z80.LD_n8(&z80.bc.hi, 5);
    z80.LD_n8(&z80.de.hi, 15);
    z80.LD_n8(&z80.hl.hi, 37);

    z80.LD_n8(&z80.af.lo, 50);
    z80.LD_n8(&z80.bc.lo, 10);
    z80.LD_n8(&z80.de.lo, 200);
    z80.LD_n8(&z80.hl.lo, 3);

    std::cout << "Loaded Registers" << std::endl;
    z80.dumpRegs();
    std::cout << "\n";

    z80.cycle();
    std::cout << "One cycle (0x46)" << std::endl;
    z80.dumpRegs();
    std::cout << "\n";

    z80.setFlags(1, 1, 0, 0);
    std::cout << "Set flags, affecting F Register" << std::endl;
    z80.dumpRegs();


    return 0;
}