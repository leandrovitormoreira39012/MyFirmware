#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define USB_VID 0x303a
#define USB_PID 0x1001
static const uint8_t TX = 43;
static const uint8_t RX = 44;
static const uint8_t SDA = 1;
static const uint8_t SCL = 2;
static const uint8_t SCK = 6;
static const uint8_t MISO = 5;
static const uint8_t MOSI = 7;
static const uint8_t SS = 8;

// Standalone screen diagnosis: disconnect JY050, radios, IR, GPS and I2C modules.
// These assignments intentionally differ from the full firmware reserves.
#define USER_SETUP_LOADED 1
#define USE_HSPI_PORT
#if defined(BRUCE_TEST_ILI9341)
#define ILI9341_DRIVER 1
#else
#define ST7789_DRIVER 1
#endif
#define TFT_WIDTH 240
#define TFT_HEIGHT 320
#define TFT_RGB_ORDER TFT_BGR
#define TFT_INVERSION_ON
#define TFT_MOSI 11
#define TFT_MISO 18
#define TFT_SCLK 12
#define TFT_CS 10
#define TFT_DC 9
#define TFT_RST 14
#define TFT_BL 13
#define TFT_BACKLIGHT_ON HIGH
#define SPI_FREQUENCY 10000000
#define SPI_READ_FREQUENCY 1000000
#define LOAD_GLCD
#define TOUCH_CS -1

// Touch and SD retain the original S3 peripheral bus, separate from LCD HSPI.
#define TOUCH_XPT2046_SPI
#define XPT2046_TOUCH_CONFIG_INT_GPIO_NUM -1
#define XPT2046_SPI_BUS_MOSI_IO_NUM 7
#define XPT2046_SPI_BUS_MISO_IO_NUM 5
#define XPT2046_SPI_BUS_SCLK_IO_NUM 6
#define XPT2046_SPI_CONFIG_CS_GPIO_NUM 4
#define SDCARD_CS 8

#endif
