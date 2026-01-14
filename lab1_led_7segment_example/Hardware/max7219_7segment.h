#ifndef __MAX7219_7SEGMENT_H
#define __MAX7219_7SEGMENT_H

#include "stm32f4xx_hal.h"

void Write_Max7219(uint8_t address, uint8_t data);
void Init_Max7219();

#endif