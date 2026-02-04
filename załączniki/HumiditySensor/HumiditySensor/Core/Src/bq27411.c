#include "bq27411.h"

static I2C_HandleTypeDef *bqI2c;

void bq27411_Init(I2C_HandleTypeDef *hi2c) {
    bqI2c = hi2c;
    bq27411_ConfigureBattery();
}

void bq27411_ConfigureBattery(void) {
    uint8_t designCapacity[3] = {0x3C, 0xD0, 0x07}; // 2000mAh
    HAL_I2C_Master_Transmit(bqI2c, BQ27411_ADDRESS << 1, designCapacity, 3, HAL_MAX_DELAY);
}

float bq27411_GetStableBatteryLevel(void)  {
    uint8_t measurements[10];
    for (int i = 0; i < 10; i++) {
        uint8_t buffer[2];
        HAL_I2C_Mem_Read(bqI2c, BQ27411_ADDRESS << 1, 0x2C, I2C_MEMADD_SIZE_8BIT, buffer, 2, HAL_MAX_DELAY);
        uint16_t level = (buffer[0] << 8) | buffer[1];
        measurements[i] = level / 256;
        HAL_Delay(50);
    }
    uint16_t sum = 0;
    for (int i = 5; i < 10; i++) {
        sum += measurements[i];
    }

    return sum / 5.0f;
}


