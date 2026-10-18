#include <Arduino.h>
#include <CYD28_TouchscreenR.h>
#include <SD.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <initializer_list>

// No Bruce setup, saved preferences or radio libraries are involved in this test.
TFT_eSPI screen;
SPIClass peripherals(FSPI);
CYD28_TouchR touch(320, 240);

#ifdef BRUCE_TEST_ILI9341
static const char *panelName = "ILI9341";
#else
static const char *panelName = "ST7789";
#endif

void setup() {
    // Light first: a later SPI or storage fault should not leave BL unpowered.
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
    for (uint8_t cs : {TFT_CS, SDCARD_CS, XPT2046_SPI_CONFIG_CS_GPIO_NUM}) {
        pinMode(cs, OUTPUT);
        digitalWrite(cs, HIGH);
    }
    Serial.begin(115200);
    delay(500);
    Serial.printf("\nJoker S3 display test: %s, LED=13, RESET=14\n", panelName);

    screen.init(); // Includes physical RESET pulse on GPIO14.
    screen.setRotation(1);
    screen.invertDisplay(true);
    digitalWrite(TFT_BL, HIGH);
    screen.fillScreen(TFT_BLACK);
    screen.setTextColor(TFT_WHITE, TFT_BLACK);
    screen.setTextSize(2);
    screen.setCursor(8, 8);
    screen.printf("Teste %s", panelName);
    screen.fillRect(0, 36, 106, 46, TFT_RED);
    screen.fillRect(106, 36, 106, 46, TFT_GREEN);
    screen.fillRect(212, 36, 108, 46, TFT_BLUE);
    screen.setCursor(8, 92);
    screen.println("LED e RESET por GPIO");
    screen.setCursor(8, 118);
    screen.println("Toque para testar");
    Serial.println("LCD init returned; expected red/green/blue bars.");

    peripherals.begin(SCK, MISO, MOSI);
    touch.begin(&peripherals);
    touch.setRotation(1);
    // Mount only; never format the card or write files.
    bool sdReady = SD.begin(SDCARD_CS, peripherals, 4000000, "/sd", 5, false);
    screen.setCursor(8, 146);
    screen.println(sdReady ? "SD: montado" : "SD: ausente ou falhou");
    Serial.printf("SD mount: %s\n", sdReady ? "OK" : "not detected / failed");
}

void loop() {
    static uint32_t lastBeat = 0;
    static uint32_t lastTouch = 0;
    uint32_t now = millis();
    if (now - lastBeat >= 1000) {
        lastBeat = now;
        screen.fillRect(0, 212, 320, 28, TFT_BLACK);
        screen.setCursor(8, 216);
        screen.printf("Ativo: %lus", static_cast<unsigned long>(now / 1000));
        Serial.printf("ALIVE %lu BL=%d\n", static_cast<unsigned long>(now / 1000), digitalRead(TFT_BL));
    }
    if (now - lastTouch >= 100 && touch.touched()) {
        lastTouch = now;
        auto raw = touch.getPointRaw();
        auto point = touch.getPointScaled();
        int x = constrain(point.x, 0, 319);
        int y = constrain(point.y, 0, 239);
        screen.fillRect(0, 174, 320, 28, TFT_BLACK);
        screen.setCursor(8, 178);
        screen.printf("Toque: %d, %d", x, y);
        screen.fillCircle(x, y, 3, TFT_YELLOW);
        Serial.printf("TOUCH raw=%d,%d z=%d xy=%d,%d\n", raw.x, raw.y, raw.z, x, y);
    }
    delay(10);
}
