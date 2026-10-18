#include "ui_text.h"
#include "ConfigMenu.h"
#include "../mykeyboard.h"
#include "core/display.h"
#include "core/i2c_finder.h"
#include "core/main_menu.h"
#include "core/settings.h"
#include "core/utils.h"
#include "core/wifi/wifi_common.h"
#ifdef HAS_RGB_LED
#include "core/led_control.h"
#endif

/*********************************************************************
**  Function: optionsMenu
**  Main Config menu entry point
**********************************************************************/
void ConfigMenu::optionsMenu() {
    returnToMenu = false;
    while (true) {
        // Check if we need to exit to Main Menu (e.g., DevMode disabled)
        if (returnToMenu) {
            returnToMenu = false; // Reset flag
            return;
        }

        std::vector<Option> localOptions = {
            {uiText("Display & UI"),  [this]() { displayUIMenu(); }},
#ifdef HAS_RGB_LED
            {uiText("LED Config"),    [this]() { ledMenu(); }      },
#endif
#if !defined(LITE_VERSION) && (defined(BUZZ_PIN) || defined(HAS_NS4168_SPKR))
            {uiText("Audio Config"),  [this]() { audioMenu(); }    },
#endif
            {uiText("System Config"), [this]() { systemMenu(); }   },
            {uiText("Power"),         [this]() { powerMenu(); }    },
        };

#if !defined(LITE_VERSION)
        if (!appStoreInstalled()) {
            localOptions.push_back({uiText("Install App Store"), []() { installAppStoreJS(); }});
        }
#endif

        if (bruceConfig.devMode) {
            localOptions.push_back({uiText("Dev Mode"), [this]() { devMenu(); }});
        }

        localOptions.push_back({uiText("About"), showDeviceInfo});
        localOptions.push_back({uiText("Main Menu"), []() {}});

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("Config"));

        // Exit to Main Menu only if user pressed Back
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Otherwise rebuild Config menu after submenu returns
    }
}

/*********************************************************************
**  Function: displayUIMenu
**  Display & UI configuration submenu with auto-rebuild
**********************************************************************/
void ConfigMenu::displayUIMenu() {
    while (true) {
        std::vector<Option> localOptions = {
            {uiText("Brightness"),  [this]() { setBrightnessMenu(); }               },
            {uiText("Dim Time"),    [this]() { setDimmerTimeMenu(); }               },
            {uiText("Orientation"), [this]() { lambdaHelper(gsetRotation, true)(); }},
            {uiText("UI Color"),    [this]() { setUIColor(); }                      },
            {uiText("UI Theme"),    [this]() { setTheme(); }                        },
            {uiText("Back"),        []() {}                                         },
        };

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("Display & UI"));

        // Exit only if user pressed Back or ESC
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Otherwise loop continues and menu rebuilds
    }
}

/*********************************************************************
**  Function: ledMenu
**  LED configuration submenu with auto-rebuild for toggles
**********************************************************************/
#ifdef HAS_RGB_LED
void ConfigMenu::ledMenu() {
    while (true) {
        std::vector<Option> localOptions = {
            {uiText("LED Color"),
             [this]() {
                 beginLed();
                 setLedColorConfig();
             }                                                                            },
            {uiText("LED Effect"),
             [this]() {
                 beginLed();
                 setLedEffectConfig();
             }                                                                            },
            {uiText("LED Brightness"),
             [this]() {
                 beginLed();
                 setLedBrightnessConfig();
             }                                                                            },
            {String(uiText("LED Blink: ")) + (bruceConfig.ledBlinkEnabled ? uiText("ON") : uiText("OFF")),
             [this]() {
                 // Toggle LED blink setting
                 bruceConfig.ledBlinkEnabled = !bruceConfig.ledBlinkEnabled;
                 bruceConfig.saveFile();
             }                                                                            },
            {uiText("Back"),                                                               []() {}},
        };

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("LED Config"));

        // Exit only if user pressed Back or ESC
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Menu rebuilds to update toggle label
    }
}
#endif
/*********************************************************************
**  Function: audioMenu
**  Audio configuration submenu with auto-rebuild for toggles
**********************************************************************/
void ConfigMenu::audioMenu() {
    while (true) {
        std::vector<Option> localOptions = {
#if !defined(LITE_VERSION)
#if defined(BUZZ_PIN) || defined(HAS_NS4168_SPKR)

            {String(uiText("Sound: ")) + (bruceConfig.soundEnabled ? uiText("ON") : uiText("OFF")),
                                                             [this]() {
                 // Toggle sound setting
                 bruceConfig.soundEnabled = !bruceConfig.soundEnabled;
                 bruceConfig.saveFile();
             }                                                                                                                                            },
#if defined(HAS_NS4168_SPKR)
            {uiText("Sound Volume"),                                                [this]() { setSoundVolume(); }},
#endif  // BUZZ_PIN || HAS_NS4168_SPKR
#endif  //  HAS_NS4168_SPKR
#endif  //  LITE_VERSION
            {uiText("Back"),                                                        []() {}                       },
        };

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("Audio Config"));

        // Exit only if user pressed Back or ESC
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Menu rebuilds to update toggle label
    }
}

/*********************************************************************
**  Function: systemMenu
**  System configuration submenu with auto-rebuild for toggles
**********************************************************************/
void ConfigMenu::systemMenu() {
    while (true) {
        std::vector<Option> localOptions = {
            {String(uiText("InstaBoot: ")) + (bruceConfig.instantBoot ? uiText("ON") : uiText("OFF")),
             [this]() {
                 // Toggle InstaBoot setting
                 bruceConfig.instantBoot = !bruceConfig.instantBoot;
                 bruceConfig.saveFile();
             }                                                                                                           },
            {String(uiText("WiFi Startup: ")) + (bruceConfig.wifiAtStartup ? uiText("ON") : uiText("OFF")),
             [this]() {
                 // Toggle WiFi at startup setting
                 bruceConfig.wifiAtStartup = !bruceConfig.wifiAtStartup;
                 bruceConfig.saveFile();
             }                                                                                                           },
            {uiText("Startup App"),                                                         [this]() { setStartupApp(); }        },
            {uiText("Hide/Show Apps"),                                                      [this]() { mainMenu.hideAppsMenu(); }},
            {uiText("Clock"),                                                               [this]() { setClock(); }             },
            {String(uiText("Keyboard Language: ")) + bruceConfig.keyboardLang,              [this]() { setKeyboardLanguage(); }  },
            {uiText("Advanced"),                                                            [this]() { advancedMenu(); }         },
            {uiText("Back"),                                                                []() {}                              },
        };

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("System Config"));

        // Exit only if user pressed Back or ESC
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Menu rebuilds to update toggle labels
    }
}

/*********************************************************************
**  Function: advancedMenu
**  Advanced settings submenu (nested under System Config)
**********************************************************************/
void ConfigMenu::advancedMenu() {
    while (true) {
        std::vector<Option> localOptions = {
            {uiText("Set Device pins"), [this]() { pinsMenu(); }           },
#if !defined(LITE_VERSION)
            {uiText("Toggle BLE API"),  [this]() { enableBLEAPI(); }       },
            {"BadUSB/BLE",      [this]() { setBadUSBBLEMenu(); }   },
#endif
            {uiText("BLE name"),
             [this]() {
                 String name = keyboard(bruceConfigPins.bleName, 30, uiText("BLE device name"));
                 if (name.length() > 0 && name != "\x1B") bruceConfigPins.setBleName(name);
             }                                                     },
            {uiText("Network Creds"),   [this]() { setNetworkCredsMenu(); }},
            {uiText("Factory Reset"),
             []() {
                 // Confirmation dialog for destructive action
                 drawMainBorder(true);
                 int8_t choice = displayMessage(
                     uiText("Are you sure you want\nto Factory Reset?\nAll data will be lost!"),
                     uiText("No"),
                     nullptr,
                     uiText("Yes"),
                     TFT_RED
                 );

                 if (choice == 1) {
                     // User confirmed - perform factory reset
                     bruceConfigPins.factoryReset();
                     bruceConfig.factoryReset(); // Restarts ESP
                 }
                 // If cancelled, loop continues and menu rebuilds
             }                                                     },
            {uiText("Back"),            []() {}                            },
        };

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("Advanced"));

        // Exit to System Config menu
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Menu rebuilds after each action
    }
}
/*********************************************************************
**  Function: powerMenu
**  Power management submenu with auto-rebuild
**********************************************************************/
void ConfigMenu::powerMenu() {
    while (true) {
        std::vector<Option> localOptions = {
            {uiText("Deep Sleep"), goToDeepSleep          },
            {uiText("Sleep"),      setSleepMode           },
            {uiText("Restart"),    []() { ESP.restart(); }},
            {uiText("Power Off"),
             []() {
                 // Confirmation dialog for power off
                 drawMainBorder(true);
                 int8_t choice = displayMessage(uiText("Power Off Device?"), uiText("No"), nullptr, uiText("Yes"), TFT_RED);

                 if (choice == 1) { powerOff(); }
             }                                    },
            {uiText("Back"),       []() {}                },
        };

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("Power Menu"));

        // Exit to Config menu
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Menu rebuilds after each action
    }
}

/*********************************************************************
**  Function: devMenu
**  Developer mode menu for advanced hardware configuration
**********************************************************************/
void ConfigMenu::devMenu() {
    while (true) {
        std::vector<Option> localOptions = {
            {uiText("Serial use USB"),  [this]() { switchToUSBSerial(); }          },
            {uiText("Serial use UART"), [this]() { switchToUARTSerial(); }         },
            {uiText("Disable DevMode"), [this]() { bruceConfig.setDevMode(false); }},
            {uiText("Back"),            []() {}                                    },
        };

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("Dev Mode"));

        // Check if "Disable DevMode" was pressed (second-to-last option)
        if (selected == localOptions.size() - 2) {
            returnToMenu = true; // Signal to exit all Config menus
            return;
        }

        // Exit to Config menu on Back or ESC
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Menu rebuilds after each action
    }
}

/*********************************************************************
**  Function: pinsMenu
**  Developer mode menu for advanced hardware configuration
**********************************************************************/
void ConfigMenu::pinsMenu() {
    while (true) {
        std::vector<Option> localOptions = {
            {uiText("I2C Finder"),     [this]() { find_i2c_addresses(); }                      },
            {uiText("CC1101 Pins"),    [this]() { setSPIPinsMenu(bruceConfigPins.CC1101_bus); }},
            {uiText("NRF24  Pins"),    [this]() { setSPIPinsMenu(bruceConfigPins.NRF24_bus); } },
#if !defined(LITE_VERSION)
            {uiText("LoRa Pins"),      [this]() { setSPIPinsMenu(bruceConfigPins.LoRa_bus); }  },
            {uiText("ST25R3916 Pins"), [this]() { setSPIPinsMenu(bruceConfigPins.ST25R_bus); } },
            {uiText("W5500 Pins"),     [this]() { setSPIPinsMenu(bruceConfigPins.W5500_bus); } },
#endif
            {uiText("SDCard Pins"),    [this]() { setSPIPinsMenu(bruceConfigPins.SDCARD_bus); }},
            {uiText("I2C Pins"),       [this]() { setI2CPinsMenu(bruceConfigPins.i2c_bus); }   },
            {uiText("UART Pins"),      [this]() { setUARTPinsMenu(bruceConfigPins.uart_bus); } },
            {uiText("GPS Pins"),       [this]() { setUARTPinsMenu(bruceConfigPins.gps_bus); }  },
            //{"Serial use USB",  [this]() { switchToUSBSerial(); }                       },
            //{"Serial use UART", [this]() { switchToUARTSerial(); }                      },
            {uiText("Back"),           []() {}                                                 },
        };

        int selected = loopOptions(localOptions, MENU_TYPE_SUBMENU, uiText("Pins Setup"));

        // Exit to Config menu on Back or ESC
        if (selected == -1 || selected == localOptions.size() - 1) { return; }
        // Menu rebuilds after each action
    }
}

/*********************************************************************
**  Function: switchToUSBSerial
**  Switch serial output to USB Serial
**********************************************************************/
void ConfigMenu::switchToUSBSerial() {
    USBserial.setSerialOutput(&Serial);
    Serial1.end();
}

/*********************************************************************
**  Function: switchToUARTSerial
**  Switch serial output to UART (handles pin conflicts)
**********************************************************************/
void ConfigMenu::switchToUARTSerial() {
    // Check and resolve SD card pin conflicts
    if (bruceConfigPins.SDCARD_bus.checkConflict(bruceConfigPins.uart_bus.rx) ||
        bruceConfigPins.SDCARD_bus.checkConflict(bruceConfigPins.uart_bus.tx)) {
        sdcardSPI.end();
    }

    // Check and resolve CC1101/NRF24 pin conflicts
    if (bruceConfigPins.CC1101_bus.checkConflict(bruceConfigPins.uart_bus.rx) ||
        bruceConfigPins.CC1101_bus.checkConflict(bruceConfigPins.uart_bus.tx) ||
        bruceConfigPins.NRF24_bus.checkConflict(bruceConfigPins.uart_bus.rx) ||
        bruceConfigPins.NRF24_bus.checkConflict(bruceConfigPins.uart_bus.tx)) {
        AUX_SPI.end();
    }

    // Configure UART pins and switch serial output
    pinMode(bruceConfigPins.uart_bus.rx, INPUT);
    pinMode(bruceConfigPins.uart_bus.tx, OUTPUT);
    Serial1.begin(115200, SERIAL_8N1, bruceConfigPins.uart_bus.rx, bruceConfigPins.uart_bus.tx);
    USBserial.setSerialOutput(&Serial1);
}
/*********************************************************************
**  Function: drawIcon
**  Draw config gear icon
**********************************************************************/
void ConfigMenu::drawIcon(float scale) {
    clearIconArea();
    int radius = scale * 9;

    // Draw 6 gear teeth segments
    for (int i = 0; i < 6; i++) {
        tft.drawArc(
            iconCenterX,
            iconCenterY,
            3.5 * radius,
            2 * radius,
            15 + 60 * i,
            45 + 60 * i,
            bruceConfig.priColor,
            bruceConfig.bgColor,
            true
        );
    }

    // Draw inner circle
    tft.drawArc(
        iconCenterX,
        iconCenterY,
        2.5 * radius,
        radius,
        0,
        360,
        bruceConfig.priColor,
        bruceConfig.bgColor,
        false
    );
}
