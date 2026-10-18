#include <cassert>
#include <string>
#include <vector>
#include <cstring>
#include "ui_text.h"
#include "ui_utf8.h"
#define PROGMEM
#include "Fonts/Portuguese_GLCD.h"

// Only the Arduino String operations used by the portable layout helpers.
struct String : std::string {
    using std::string::string;
    String(const std::string &value) : std::string(value) {}
    bool isEmpty() const { return empty(); }
    String substring(size_t from, size_t to = std::string::npos) const {
        if (from >= size()) return "";
        return substr(from, to == std::string::npos ? to : to - from);
    }
};
// Runner extracts the production wrapText function for a native regression test.
#include "wrap_text.inc"
#include "renderer.h"
struct PaddedScreen {
    std::vector<String> lines;
    void setCursor(int, int) {}
    int getCursorY() const { return 0; }
    void println(const String &value) { lines.push_back(value); }
} tft;
constexpr int tftWidth = 88, BORDER_PAD_X = 5, FP = 1, LW = 6;
#include "padded_lines.inc"

void testPaddedLines() {
    String value = std::string(12, 'a') + "ãb";
    padprintln(value, 1);
    assert(tft.lines.size() == 2 && tft.lines[0] == std::string(12, 'a') + "ã" && tft.lines[1] == "b");
    tft.lines.clear();
    padprintln(value.c_str(), 1);
    assert(tft.lines.size() == 2 && tft.lines[0] + tft.lines[1] == value);
    tft.lines.clear();
    padprintln("", 1);
    assert(tft.lines.size() == 1 && tft.lines[0].empty());
}


int main() {
    testRenderer();
    testPaddedLines();
#ifdef BRUCE_UI_PT_BR
    static_assert(bruce_ui::equal(uiText("Config"), "Configurações"));
    assert(std::string(uiText("No")) == "Não");
    assert(std::string(uiText("Read tag")) == "Ler tag");
    assert(std::string(uiText("Network Password:")) == "Senha da rede:");
    for (const auto &entry : bruce_ui::entries) {
        assert(std::string(uiText(entry.english)) == entry.portuguese);
    }
    assert(std::string(uiText("Factory Reset")) == "Restaurar padrões");
#else
    static_assert(bruce_ui::equal(uiText("Config"), "Config"));
    assert(std::string(uiText("No")) == "No");
#endif
    assert(std::string(uiText("/BruceJS/Tools/App Store.js")) == "/BruceJS/Tools/App Store.js");
    assert(std::string(uiText("BruceNet")) == "BruceNet");
    String label = "Configurações";
    assert(label.length() == 15 && uiLength(label) == 13);
    assert(uiSlice(label, 9, 11) == "çõ");
    assert(uiSlice(label, 0, 10) == "Configuraç");
    assert(uiSlice(label, 13, 50).empty());
    assert(uiSlice(String("WiFi"), 0, 99) == "WiFi");
    assert(uiLength(String("áéíóú ãõ ç")) == 10);
    auto lines = wrapText(String("Configurações"), 10);
    assert(lines.size() == 2 && lines[0] == "Configuraç" && lines[1] == "ões");
    lines = wrapText(String("Não desligue"), 8);
    assert(lines.size() == 2 && lines[0] == "Não" && lines[1] == "desligue");
    lines = wrapText(String("abcdefghijkl"), 5);
    assert(lines.size() == 3 && lines[0] + lines[1] + lines[2] == "abcdefghijkl");
    assert(wrapText(String(""), 10).empty());
    assert(wrapText(String("teste"), 0).empty());
    // All PT-BR diacritics have glyphs; ASCII retains the original font.
    for (auto ch : U"áàâãéêíóôõúüçÁÀÂÃÉÊÍÓÔÕÚÜÇ") {
        if (!ch) continue;
        const auto glyph = portugueseGlyph(ch);
        assert(glyph);
        assert(glyph[0] | glyph[1] | glyph[2] | glyph[3] | glyph[4]);
    }
    assert(portugueseGlyph('A') == nullptr);
    assert(portugueseGlyph(0x03A9) == nullptr);
    assert(portugueseGlyph(0x00E3)[1] != portugueseGlyph(0x00E1)[1]);
}
