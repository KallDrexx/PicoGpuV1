#include "ili9341.h"

// Modified from https://github.com/tvlad1234/pico-displayDrivs

#define MADCTL_MY 0x80  ///< Bottom to top
#define MADCTL_MX 0x40  ///< Right to left
#define MADCTL_MV 0x20  ///< Reverse Mode
#define MADCTL_ML 0x10  ///< LCD refresh Bottom to top
#define MADCTL_RGB 0x00 ///< Red-Green-Blue pixel order
#define MADCTL_BGR 0x08 ///< Blue-Green-Red pixel order
#define MADCTL_MH 0x04  ///< LCD refresh right to left

#define TFTWIDTH 240  ///< ILI9341 max TFT width
#define TFTHEIGHT 320 ///< ILI9341 max TFT height

#define NOP 0x00     ///< No-op register
#define SWRESET 0x01 ///< Software reset register
#define RDDID 0x04   ///< Read display identification information
#define RDDST 0x09   ///< Read Display Status

#define SLPIN 0x10  ///< Enter Sleep Mode
#define SLPOUT 0x11 ///< Sleep Out
#define PTLON 0x12  ///< Partial Mode ON
#define NORON 0x13  ///< Normal Display Mode ON

#define RDMODE 0x0A     ///< Read Display Power Mode
#define RDMADCTL 0x0B   ///< Read Display MADCTL
#define RDPIXFMT 0x0C   ///< Read Display Pixel Format
#define RDIMGFMT 0x0D   ///< Read Display Image Format
#define RDSELFDIAG 0x0F ///< Read Display Self-Diagnostic Result

#define INVOFF 0x20   ///< Display Inversion OFF
#define INVON 0x21    ///< Display Inversion ON
#define GAMMASET 0x26 ///< Gamma Set
#define DISPOFF 0x28  ///< Display OFF
#define DISPON 0x29   ///< Display ON

#define CASET 0x2A ///< Column Address Set
#define PASET 0x2B ///< Page Address Set
#define RAMWR 0x2C ///< Memory Write
#define RAMRD 0x2E ///< Memory Read

#define PTLAR 0x30    ///< Partial Area
#define VSCRDEF 0x33  ///< Vertical Scrolling Definition
#define MADCTL 0x36   ///< Memory Access Control
#define VSCRSADD 0x37 ///< Vertical Scrolling Start Address
#define PIXFMT 0x3A   ///< COLMOD: Pixel Format Set

#define FRMCTR1                                                        \
  0xB1 ///< Frame Rate Control (In Normal Mode/Full Colors)
#define FRMCTR2 0xB2 ///< Frame Rate Control (In Idle Mode/8 colors)
#define FRMCTR3                                                        \
  0xB3 ///< Frame Rate control (In Partial Mode/Full Colors)
#define INVCTR 0xB4  ///< Display Inversion Control
#define DFUNCTR 0xB6 ///< Display Function Control

#define PWCTR1 0xC0 ///< Power Control 1
#define PWCTR2 0xC1 ///< Power Control 2
#define PWCTR3 0xC2 ///< Power Control 3
#define PWCTR4 0xC3 ///< Power Control 4
#define PWCTR5 0xC4 ///< Power Control 5
#define VMCTR1 0xC5 ///< VCOM Control 1
#define VMCTR2 0xC7 ///< VCOM Control 2

#define RDID1 0xDA ///< Read ID 1
#define RDID2 0xDB ///< Read ID 2
#define RDID3 0xDC ///< Read ID 3
#define RDID4 0xDD ///< Read ID 4

#define GMCTRP1 0xE0 ///< Positive Gamma Correction
#define GMCTRN1 0xE1 ///< Negative Gamma Correction

// Some ready-made 16-bit ('565') color settings:
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define ORANGE 0xFC00

uint16_t _width;  ///< Display width as modified by current rotation
uint16_t _height; ///< Display height as modified by current rotation
uint8_t _rotation;
spi_inst_t  *_spi;

// pins
uint16_t _chipSelect;
uint16_t _dc;
uint16_t _reset;
uint16_t _sck;
uint16_t _tx;

const uint8_t initcmd[] = {
        24, //24 commands
        0xEF, 3, 0x03, 0x80, 0x02,
        0xCF, 3, 0x00, 0xC1, 0x30,
        0xED, 4, 0x64, 0x03, 0x12, 0x81,
        0xE8, 3, 0x85, 0x00, 0x78,
        0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
        0xF7, 1, 0x20,
        0xEA, 2, 0x00, 0x00,
        PWCTR1, 1, 0x23,	   // Power control VRH[5:0]
        PWCTR2, 1, 0x10,	   // Power control SAP[2:0];BT[3:0]
        VMCTR1, 2, 0x3e, 0x28, // VCM control
        VMCTR2, 1, 0x86,	   // VCM control2
        MADCTL, 1, 0x48,	   // Memory Access Control
        VSCRSADD, 1, 0x00,	   // Vertical scroll zero
        PIXFMT, 1, 0x55,
        FRMCTR1, 2, 0x00, 0x18,
        DFUNCTR, 3, 0x08, 0x82, 0x27,					 // Display Function Control
        0xF2, 1, 0x00,											 // 3Gamma Function Disable
        GAMMASET, 1, 0x01,								 // Gamma curve selected
        GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, // Set Gamma
        0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
        GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, // Set Gamma
        0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
        SLPOUT, 0x80, // Exit Sleep
        DISPON, 0x80, // Display on
        0x00				  // End of list
};

void initSpi() {
    spi_init(_spi, 1000 * 62500);
    spi_set_format(_spi, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    gpio_set_function(_sck, GPIO_FUNC_SPI);
    gpio_set_function(_tx, GPIO_FUNC_SPI);

    gpio_init(_chipSelect);
    gpio_set_dir(_chipSelect, GPIO_OUT);
    gpio_put(_chipSelect, 1);

    gpio_init(_dc);
    gpio_set_dir(_dc, GPIO_OUT);
    gpio_put(_dc, 1);

    gpio_init(_reset);
    gpio_set_dir(_reset, GPIO_OUT);
    gpio_put(_reset, 1);
}

void reset() {
    gpio_put(_reset, 0);
    sleep_ms(5);
    gpio_put(_reset, 1);
    sleep_ms(150);
}

void select() {
    gpio_put(_chipSelect, 0);
}

void deselect() {
    gpio_put(_chipSelect, 1);
}

void registerCommand() {
    gpio_put(_dc, 0);
}

void registerData() {
    gpio_put(_dc, 1);
}

void writeCommand(uint8_t command) {
    registerCommand();
    spi_set_format(_spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_write_blocking(_spi, &command, sizeof(command));
}

void writeData(uint8_t* buffer, size_t size) {
    registerData();
    spi_set_format(_spi, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_write_blocking(_spi, buffer, size);
}

void sendCommand(uint8_t commandByte, uint8_t* dataBytes, uint8_t numDataBytes) {
    select();
    writeCommand(commandByte);
    writeData(dataBytes, numDataBytes);
    deselect();
}

void setAddressWindow(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height) {
    uint32_t xa = ((uint32_t)startX << 16) | (startX + width - 1);
    uint32_t ya = ((uint32_t)startY << 16) | (startY + height - 1);

    xa = __builtin_bswap32(xa);
    ya = __builtin_bswap32(ya);

    writeCommand(CASET);
    writeData(&xa, sizeof(xa));

    // row address set
    writeCommand(PASET);
    writeData(&ya, sizeof(ya));

    // write to RAM
    writeCommand(RAMWR);
}

void ILI9341_init(spi_inst_t* spi, uint16_t dc, uint16_t cs, uint16_t resetPort, uint16_t sck, uint16_t tx) {
    _spi = spi;
    _chipSelect = cs;
    _dc = dc;
    _reset = resetPort;
    _sck = sck;
    _tx = tx;

    initSpi();
    select();
    reset();

    uint8_t* commands = initcmd;
    uint16_t ms;
    uint8_t commandCount = *(commands++);

    while(commandCount--)
    {
        uint8_t command = *(commands++);
        uint8_t x = *(commands++);
        uint8_t argCount = x & 0x7f;
        sendCommand(command, commands, argCount);
        command += argCount;
        if (x & 0x80) {
            sleep_ms(150);
        }
    }

    _width = TFTWIDTH;
    _height = TFTHEIGHT;
}

void ILI9341_setRotation(uint8_t rotation) {
    _rotation = rotation % 4; // can't be higher than 3
    switch (rotation)
    {
        case 0:
            rotation = (MADCTL_MX | MADCTL_BGR);
            _width = TFTWIDTH;
            _height =TFTHEIGHT;
            break;
        case 1:
            rotation = (MADCTL_MV | MADCTL_BGR);
            _width = TFTHEIGHT;
            _height = TFTWIDTH;
            break;
        case 2:
            rotation = (MADCTL_MY | MADCTL_BGR);
            _width = TFTWIDTH;
            _height = TFTHEIGHT;
            break;
        case 3:
            rotation = (MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
            _width = TFTHEIGHT;
            _height = TFTWIDTH;
            break;
    }

    sendCommand(MADCTL, &rotation, 1);
}

void ILI9341_write(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint16_t *bitmap) {
    select();
    setAddressWindow(startX, startY, width, height);
    registerData();

    spi_set_format(_spi, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    spi_write16_blocking(_spi, bitmap, width * height);

    deselect();
}
