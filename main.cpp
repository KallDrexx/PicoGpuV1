#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "ili934x.h"

#define DISPLAY_RESET_PIN 28
#define DISPLAY_DC_PIN 27
#define DISPLAY_LED_PIN 26
#define DISPLAY_SPI_TX_PIN 19
#define DISPLAY_SPI_SCK_PIN 18
#define DISPLAY_SPI_CS_PIN 17
#define DISPLAY_SPI_RX_PIN 16
#define SPI_PORT spi0

int main()
{
	stdio_init_all();

    spi_init(SPI_PORT,  1 * 1000 * 1000);
    gpio_set_function(DISPLAY_SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_SPI_SCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(DISPLAY_SPI_RX_PIN, GPIO_FUNC_SPI);

    gpio_init(DISPLAY_SPI_CS_PIN);
    gpio_set_dir(DISPLAY_SPI_CS_PIN, GPIO_OUT);
    gpio_init(DISPLAY_DC_PIN);
    gpio_set_dir(DISPLAY_DC_PIN, GPIO_OUT);
    gpio_init(DISPLAY_RESET_PIN);
    gpio_set_dir(DISPLAY_RESET_PIN, GPIO_OUT);
    gpio_init(DISPLAY_LED_PIN);
    gpio_set_dir(DISPLAY_LED_PIN, GPIO_OUT);

    bi_decl(bi_3pins_with_func(DISPLAY_SPI_TX_PIN, DISPLAY_SPI_SCK_PIN, DISPLAY_SPI_CS_PIN, GPIO_FUNC_SPI));

    ILI934X display = ILI934X(
        SPI_PORT,
        DISPLAY_SPI_CS_PIN,
        DISPLAY_DC_PIN,
        DISPLAY_RESET_PIN);

    display.reset();
    display.init();
    display.clear(COLOUR_AQUA);
    gpio_put(DISPLAY_LED_PIN, true);

    uint64_t lastTime1hz = 0;
    while (1)
    {
        if (lastTime1hz + 1000000000 < time_us_64())
        {
            lastTime1hz = time_us_64();

//            display.clear(COLOUR_AQUA);
//            display.drawCircle(50, 50, 25, display.colour565(255,0,0));
        }
    }
}
