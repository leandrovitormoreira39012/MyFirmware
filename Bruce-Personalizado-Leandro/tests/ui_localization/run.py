"""Native UI regressions; no firmware hardware or third-party Python packages needed."""
from pathlib import Path
import subprocess
import tempfile

repo = Path(__file__).resolve().parents[2]
source = (repo / "src/core/display.cpp").read_text()
start = source.index("std::vector<String> wrapText(")
end = source.index("\n}\n", start) + 3
graphics = (repo / "lib/TFT_eSPI/TFT_eSPI.cpp").read_text()
glyph_start = graphics.index("void TFT_eSPI::drawChar(int32_t x,")
glyph_end = graphics.index("\n}\n", glyph_start) + 3
padded_start = source.index("void padprintln(const String &")
padded_end = source.index("void padprintln(char c,", padded_start)
with tempfile.TemporaryDirectory(prefix="bruce-ui-test-") as tmp:
    path = Path(tmp)
    (path / "wrap_text.inc").write_text(source[start:end])
    (path / "draw_char.inc").write_text(graphics[glyph_start:glyph_end])
    (path / "padded_lines.inc").write_text(source[padded_start:padded_end])
    for enabled in (False, True):
        command = ["g++", "-std=c++17", "-Wall", "-Wextra", "-Werror",
                   "-I" + str(repo / "include"), "-I" + str(repo / "lib/TFT_eSPI"),
                   "-I" + tmp, str(repo / "tests/ui_localization/test.cpp"),
                   "-o", str(path / "test")]
        if enabled:
            command.append("-DBRUCE_UI_PT_BR")
        subprocess.run(command, check=True)
        subprocess.run([str(path / "test")], check=True)
        print("PASS:", "PT-BR" if enabled else "English fallback", "catalog, UTF-8 layout, wrapping, actual GLCD pixel rendering")
