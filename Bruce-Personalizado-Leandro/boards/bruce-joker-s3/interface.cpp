#include "CYD28_TouchscreenR.h"
#include "core/bus_HAL.h"
#include "core/display.h"
#include "core/powerSave.h"
#include "core/utils.h"
#include <Wire.h>
#include <globals.h>
#include <interface.h>

CYD28_TouchR touch(320, 240);

namespace {
const uint8_t buttons[] = {UP_BTN, DW_BTN, L_BTN, R_BTN, SEL_BTN, ESC_BTN, JY050_SET_BTN};
bool touchReady = false;
}

void _setup_gpio() {
#if TFT_BL >= 0
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);
#endif
    for (uint8_t pin : buttons) pinMode(pin, INPUT_PULLUP);
    // Deselect every device before any SPI controller starts.
    const uint8_t selects[] = {TFT_CS, SDCARD_CS, CC1101_SS_PIN, NRF24_SS_PIN,
                               XPT2046_SPI_CONFIG_CS_GPIO_NUM};
    for (uint8_t pin : selects) {
        pinMode(pin, OUTPUT);
        digitalWrite(pin, HIGH);
    }
    pinMode(NRF24_CE_PIN, OUTPUT);
    digitalWrite(NRF24_CE_PIN, LOW);

    // The SD bus owns FSPI even when no card is fitted. setup_gpio() also
    // passes this instance to the CC1101 driver before storage is mounted.
#ifndef BRUCE_JOKER_MARAUDER
    sdcardSPI.begin(SPI_SCK_PIN, SPI_MISO_PIN, SPI_MOSI_PIN);
#endif
    // With the Marauder profile, tft.init() owns this bus. SD, touch and radios
    // obtain that same SPIClass through acquireSPIBus(); never start a second owner.
    setSysI2CBus(&Wire);
    Wire.begin(SYS_I2C_SDA, SYS_I2C_SCL);
    bruceConfigPins.rfModule = CC1101_SPI_MODULE;
    bruceConfigPins.rfidModule = PN532_I2C_MODULE;
    bruceConfig.colorInverted = BRUCE_PANEL_INVERTED;
}

void _post_setup_gpio() {
    // Restore the tested panel polarity after loading an older saved configuration.
    bruceConfig.colorInverted = BRUCE_PANEL_INVERTED;
    tft.invertDisplay(BRUCE_PANEL_INVERTED);
    SPIClass *bus = acquireSPIBus(
        (gpio_num_t)SPI_SCK_PIN, (gpio_num_t)SPI_MISO_PIN, (gpio_num_t)SPI_MOSI_PIN
    );
    touchReady = bus != nullptr && touch.begin(bus);
    if (!touchReady) Serial.println("Touch indisponivel; use o JY050.");
}

// The external voltmeter is independent; do not display a fictitious percentage.
int getBattery() { return 0; }
bool isCharging() { return false; }

// Same on/off backlight behavior as Marauder; no PWM brightness ramp.
void _setBrightness(uint8_t brightval) {
#if TFT_BL >= 0
    digitalWrite(TFT_BL, brightval > 5 ? HIGH : LOW);
#endif
}

void InputHandler() {
    static uint32_t lastEvent = 0;
    uint32_t now = millis();
    if (now - lastEvent < 200 && !LongPress) return;

    bool pressed[7];
    bool any = false;
    for (size_t i = 0; i < 7; ++i) {
        pressed[i] = digitalRead(buttons[i]) == BTN_ACT;
        any |= pressed[i];
    }
    if (any) {
        lastEvent = now;
        if (wakeUpScreen()) return;
        AnyKeyPress = true;
        UpPress = pressed[0];
        DownPress = pressed[1];
        PrevPagePress = pressed[0];
        NextPagePress = pressed[1];
        PrevPress = pressed[2];
        NextPress = pressed[3];
        SelPress = pressed[4] || pressed[6]; // MID and SET both confirm.
        EscPress = pressed[5];              // RST is a Back button, not ESP reset.
        return;
    }

    if (!touchReady) return;
    touch.setRotation(bruceConfigPins.rotation);
    if (!touch.touched()) return;
    auto point = touch.getPointScaled();
    lastEvent = now;
    if (wakeUpScreen()) return;
    AnyKeyPress = true;
    touchPoint.x = constrain(point.x, 0, tftWidth - 1);
    touchPoint.y = constrain(point.y, 0, tftHeight + 19);
    touchPoint.pressed = true;
    touchHeatMap(touchPoint);
}

// The toggle switch physically removes power; software cannot disconnect it.
void powerOff() {
    displayRedStripe("Desligue pela chave");
    delay(1000);
}
void checkReboot() {}
