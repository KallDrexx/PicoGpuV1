#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "ili9341.h"

#define DISPLAY_RESET_PIN 28
#define DISPLAY_DC_PIN 27
#define DISPLAY_LED_PIN 26
#define DISPLAY_SPI_TX_PIN 19
#define DISPLAY_SPI_SCK_PIN 18
#define DISPLAY_SPI_CS_PIN 17
#define SPI_PORT spi0

int main()
{
	stdio_init_all();

    gpio_init(DISPLAY_LED_PIN);
    gpio_set_dir(DISPLAY_LED_PIN, GPIO_OUT);

    ILI9341_init(SPI_PORT,
                 DISPLAY_DC_PIN,
                 DISPLAY_SPI_CS_PIN,
                 DISPLAY_RESET_PIN,
                 DISPLAY_SPI_SCK_PIN,
                 DISPLAY_SPI_TX_PIN);



    while(1)
    {

    }
}
