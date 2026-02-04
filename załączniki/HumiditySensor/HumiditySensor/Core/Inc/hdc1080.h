#ifndef HDC1080_H
#define HDC1080_H

#include "main.h"

#define HDC1080_ADDRESS 0x40 << 1


void HDC1080_Init(I2C_HandleTypeDef *hi2c);
float HDC1080_ReadTemperature(void);
float HDC1080_ReadHumidity(void);
float HDC1080_AverageHumidity(void);

#endif
