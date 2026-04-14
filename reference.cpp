// Game Boy (DMG) specs

// ---- Clocks/Timing ---- //
// Master Clock	4.194304 MHz1   (up to 8.388608 MhZ for GBC)
// System Clock	1/4 the frequency of Master Clock

// Horizontal sync	9.198 KHz
// Vertical sync	59.73 Hz

// ---- CPU ---- //
// CPU	8-bit 8080-like Sharp CPU (speculated to be a SM83 core)

// ---- RAM ---- //
// Work RAM	    8 KiB	(32 KiB (4 + 7 × 4 KiB) for GBC)
// Video RAM	8 KiB	(16 KiB (2 × 8 KiB) for GBC)

// ---- Display ---- //
// Screen	LCD 4.7 × 4.3 cm	(TFT 4.4 × 4 cm for GBC)
// Resolution	160 × 144

// ---- Graphics ---- //
// OBJ ("sprites")	8 × 8 or 8 × 16 ; max 40 per screen, 10 per line
// Palettes	BG: 1 × 4, OBJ: 2 × 3	(BG: 8 × 4, OBJ: 8 × 33 for GBC)
// Colors	4 shades of green	[32768 colors (15-bit RGB) for GBC]

// ---- Audio ---- //
// Sound	4 channels with stereo output

// ---- Misc ---- // (don't expect to emulate)
// Power	DC 6V, 0.7 W	(3V, 0.6 W for GBC)
