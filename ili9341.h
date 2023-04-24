#ifndef PICO_GPU_V1_ILI9341_H
#define PICO_GPU_V1_ILI9341_H

#include "pico/stdlib.h"
#include "hardware/spi.h"

void ILI9341_init(spi_inst_t *spi, uint16_t dc, uint16_t cs, uint16_t reset, uint16_t sck, uint16_t tx);
void ILI9341_setRotation(uint8_t rotation);
void ILI9341_write(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint16_t *bitmap);

#endif //PICO_GPU_V1_ILI9341_H
