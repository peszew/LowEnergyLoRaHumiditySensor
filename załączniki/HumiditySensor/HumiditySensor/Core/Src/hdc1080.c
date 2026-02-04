#include "hdc1080.h"

static I2C_HandleTypeDef *hdcI2c;

void HDC1080_Init(I2C_HandleTypeDef *hi2c) {
    hdcI2c = hi2c;
    uint8_t config[3] = {0x02, 0x10, 0x00};
    HAL_I2C_Master_Transmit(hdcI2c, HDC1080_ADDRESS, config, 3, HAL_MAX_DELAY);
    HAL_Delay(15);
}

float HDC1080_ReadTemperature(void) {
    uint8_t command = 0x00;
    uint8_t data[2];

    HAL_I2C_Master_Transmit(hdcI2c, HDC1080_ADDRESS, &command, 1, HAL_MAX_DELAY);
    HAL_Delay(15);
    HAL_I2C_Master_Receive(hdcI2c, HDC1080_ADDRESS, data, 2, HAL_MAX_DELAY);

    uint16_t rawTemperature = (data[0] << 8) | data[1];
    return ((float)rawTemperature / 65536.0) * 165.0 - 40.0;
}

float HDC1080_ReadHumidity(void) {
    uint8_t command = 0x01;
    uint8_t data[2];

    HAL_I2C_Master_Transmit(hdcI2c, HDC1080_ADDRESS, &command, 1, HAL_MAX_DELAY);
    HAL_Delay(15);
    HAL_I2C_Master_Receive(hdcI2c, HDC1080_ADDRESS, data, 2, HAL_MAX_DELAY);

    uint16_t rawHumidity = (data[0] << 8) | data[1];
    return ((float)rawHumidity / 65536.0) * 100.0;
}

float HDC1080_AverageHumidity(void) {
    float measurements[10];
    for (int i = 0; i < 10; i++) {
        measurements[i] = HDC1080_ReadHumidity();
        HAL_Delay(50);
    }

    float sum = 0;
    for (int i = 5; i < 10; i++) {
        sum += measurements[i];
    }
    return sum / 5.0;
}
