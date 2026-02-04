#ifndef BQ27411_H
#define BQ27411_H

#include "stm32l0xx_hal.h"

#define BQ27411_ADDRESS 0x55

void bq27411_Init(I2C_HandleTypeDef *hi2c);
float bq27411_GetStableBatteryLevel(void);
void bq27411_ConfigureBattery(void);

#endif
