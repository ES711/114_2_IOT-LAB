#ifndef __MAX7219_H
#define __MAX7219_H

#include "stm32f4xx_hal.h"

void Init_MAX7219_8x8(void);
void Write_Matrix(uint8_t address, uint8_t* dat);
void Write_Max7219(uint8_t address, uint8_t dat);

#endif