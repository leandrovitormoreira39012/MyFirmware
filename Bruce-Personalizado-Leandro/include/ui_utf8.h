#pragma once
#include <stddef.h>

// Character positions for the fixed-width UTF-8 UI font (not byte positions).
// These helpers accept Arduino String and keep each UTF-8 sequence intact.
template <typename Text> size_t uiLength(const Text &text) {
    size_t count = 0;
    for (size_t i = 0; i < text.length(); ++i) {
        if ((static_cast<unsigned char>(text[i]) & 0xC0) != 0x80) ++count;
    }
    return count;
}
template <typename Text> size_t uiByteOffset(const Text &text, size_t characters) {
    size_t i = 0;
    while (i < text.length() && characters > 0) {
        ++i;
        while (i < text.length() && (static_cast<unsigned char>(text[i]) & 0xC0) == 0x80) ++i;
        --characters;
    }
    return i;
}
template <typename Text> Text uiSlice(const Text &text, size_t start, size_t end) {
    return text.substring(uiByteOffset(text, start), uiByteOffset(text, end));
}
