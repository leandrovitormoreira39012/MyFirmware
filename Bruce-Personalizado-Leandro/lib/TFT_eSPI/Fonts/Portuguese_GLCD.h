#pragma once
#include <stdint.h>

// Unicode Latin accents used by the PT-BR UI. Same 5x8 cell as GLCD.
// Existing glyphs come from glcdfont.c (Adafruit_GFX); missing accents are drawn here.
static const uint8_t *portugueseGlyph(uint16_t unicode) {
    switch (unicode) {
    case 0x00E1: { // á
        static const uint8_t glyph[5] PROGMEM = {0x20, 0x54, 0x54, 0x79, 0x41};
        return glyph;
    }
    case 0x00E0: { // à
        static const uint8_t glyph[5] PROGMEM = {0x21, 0x55, 0x54, 0x78, 0x40};
        return glyph;
    }
    case 0x00E2: { // â
        static const uint8_t glyph[5] PROGMEM = {0x21, 0x55, 0x55, 0x79, 0x41};
        return glyph;
    }
    case 0x00E3: { // ã
        static const uint8_t glyph[5] PROGMEM = {0x42, 0xA9, 0xAA, 0xA9, 0xF0};
        return glyph;
    }
    case 0x00E9: { // é
        static const uint8_t glyph[5] PROGMEM = {0x38, 0x54, 0x54, 0x55, 0x59};
        return glyph;
    }
    case 0x00EA: { // ê
        static const uint8_t glyph[5] PROGMEM = {0x39, 0x55, 0x55, 0x55, 0x59};
        return glyph;
    }
    case 0x00ED: { // í
        static const uint8_t glyph[5] PROGMEM = {0x00, 0x00, 0x44, 0x7D, 0x41};
        return glyph;
    }
    case 0x00F3: { // ó
        static const uint8_t glyph[5] PROGMEM = {0x30, 0x48, 0x48, 0x4A, 0x32};
        return glyph;
    }
    case 0x00F4: { // ô
        static const uint8_t glyph[5] PROGMEM = {0x32, 0x49, 0x49, 0x49, 0x32};
        return glyph;
    }
    case 0x00F5: { // õ
        static const uint8_t glyph[5] PROGMEM = {0x72, 0x89, 0x8A, 0x89, 0x70};
        return glyph;
    }
    case 0x00FA: { // ú
        static const uint8_t glyph[5] PROGMEM = {0x38, 0x40, 0x40, 0x22, 0x7A};
        return glyph;
    }
    case 0x00FC: { // ü
        static const uint8_t glyph[5] PROGMEM = {0x3A, 0x40, 0x40, 0x20, 0x7A};
        return glyph;
    }
    case 0x00E7: { // ç
        static const uint8_t glyph[5] PROGMEM = {0x0C, 0x1E, 0x52, 0x72, 0x12};
        return glyph;
    }
    case 0x00C1: { // Á
        static const uint8_t glyph[5] PROGMEM = {0xF0, 0x28, 0x2A, 0x29, 0xF0};
        return glyph;
    }
    case 0x00C0: { // À
        static const uint8_t glyph[5] PROGMEM = {0xF0, 0x29, 0x2A, 0x28, 0xF0};
        return glyph;
    }
    case 0x00C2: { // Â
        static const uint8_t glyph[5] PROGMEM = {0xF0, 0x2A, 0x29, 0x2A, 0xF0};
        return glyph;
    }
    case 0x00C3: { // Ã
        static const uint8_t glyph[5] PROGMEM = {0xF2, 0x29, 0x2A, 0x29, 0xF0};
        return glyph;
    }
    case 0x00C9: { // É
        static const uint8_t glyph[5] PROGMEM = {0x7C, 0x54, 0x55, 0x45, 0x00};
        return glyph;
    }
    case 0x00CA: { // Ê
        static const uint8_t glyph[5] PROGMEM = {0xF8, 0xAA, 0xA9, 0xAA, 0x88};
        return glyph;
    }
    case 0x00CD: { // Í
        static const uint8_t glyph[5] PROGMEM = {0x00, 0x88, 0xFA, 0x89, 0x00};
        return glyph;
    }
    case 0x00D3: { // Ó
        static const uint8_t glyph[5] PROGMEM = {0x70, 0x88, 0x8A, 0x89, 0x70};
        return glyph;
    }
    case 0x00D4: { // Ô
        static const uint8_t glyph[5] PROGMEM = {0x70, 0x8A, 0x89, 0x8A, 0x70};
        return glyph;
    }
    case 0x00D5: { // Õ
        static const uint8_t glyph[5] PROGMEM = {0x72, 0x89, 0x8A, 0x89, 0x70};
        return glyph;
    }
    case 0x00DA: { // Ú
        static const uint8_t glyph[5] PROGMEM = {0x78, 0x80, 0x82, 0x81, 0x78};
        return glyph;
    }
    case 0x00DC: { // Ü
        static const uint8_t glyph[5] PROGMEM = {0x3D, 0x40, 0x40, 0x40, 0x3D};
        return glyph;
    }
    case 0x00C7: { // Ç
        static const uint8_t glyph[5] PROGMEM = {0x1E, 0xA1, 0xA1, 0x61, 0x12};
        return glyph;
    }
    default: return nullptr;
    }
}
