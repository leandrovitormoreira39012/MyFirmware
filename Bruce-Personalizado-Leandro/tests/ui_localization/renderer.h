#pragma once
#include <array>
#include "Fonts/glcdfont.c"
#define LOAD_GLCD
#define pgm_read_byte(address) (*(address))

// Capture pixels from the production GLCD drawChar implementation, without SPI.
class TFT_eSPI {
public:
    bool _vpOoB = false, _cp437 = false, _utf8 = true;
    bool inTransaction = false, lockTransaction = false;
    int _xDatum = 0, _yDatum = 0, _vpX = 0, _vpY = 0, _vpW = 32, _vpH = 32;
    uint8_t textsize = 1;
    std::array<uint32_t, 32 * 32> pixels{};
    void drawChar(int32_t, int32_t, uint16_t, uint32_t, uint32_t, uint8_t);
    void begin_tft_write() {}
    void end_tft_write() {}
    void setWindow(int x0, int y0, int x1, int) {
        windowX = cursorX = x0;
        cursorY = y0;
        windowEnd = x1;
    }
    void tft_Write_16(uint32_t color) {
        drawPixel(cursorX, cursorY, color);
        if (++cursorX > windowEnd) { cursorX = windowX; ++cursorY; }
    }
    void drawPixel(int x, int y, uint32_t color) { pixels.at(y * 32 + x) = color; }
    void fillRect(int x, int y, int w, int h, uint32_t color) {
        for (int yy = y; yy < y + h; ++yy)
            for (int xx = x; xx < x + w; ++xx) drawPixel(xx, yy, color);
    }
private:
    int windowX = 0, windowEnd = 0, cursorX = 0, cursorY = 0;
};
#include "draw_char.inc"

void testRenderer() {
    // Both accelerated window writes and scaled drawing must produce the same glyph.
    for (uint16_t code : {0x41, 0xE3, 0xE7, 0xF5, 0xC3}) {
        TFT_eSPI small, large;
        large.textsize = 2;
        small.drawChar(0, 0, code, 1, 0, 1);
        large.drawChar(0, 0, code, 1, 0, 2);
        for (int y = 0; y < 8; ++y)
            for (int x = 0; x < 6; ++x)
                assert(small.pixels[y * 32 + x] == large.pixels[y * 2 * 32 + x * 2]);
    }
#ifdef BRUCE_UI_PT_BR
    TFT_eSPI screen;
    screen.drawChar(0, 0, 0xE3, 1, 0, 1);
    // Independent golden rows for the tilde and lowercase a.
    const char *rows[] = {"01010", "10100", "00000", "01110", "00001", "01111", "10001", "01111"};
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 5; ++x) assert(screen.pixels[y * 32 + x] == uint32_t(rows[y][x] - '0'));
#endif
}
