#include "hdc1080testing.h"

#define HDC1080_TEMPERATURE_REG 0x00
#define HDC1080_HUMIDITY_REG 0x01

HAL_StatusTypeDef HDC1080_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t config[3] = {0x02, 0x10, 0x00}; // default config
    return HAL_I2C_Master_Transmit(hi2c, HDC1080_ADDR, config, 3, HAL_MAX_DELAY);
}

HAL_StatusTypeDef HDC1080_Read(I2C_HandleTypeDef *hi2c, HDC1080_Data *data) {
    uint8_t reg = HDC1080_TEMPERATURE_REG;
    uint8_t buffer[4];

    // Trigger t&h measure
    if (HAL_I2C_Master_Transmit(hi2c, HDC1080_ADDR, &reg, 1, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    HAL_Delay(20); // Measure

    
    if (HAL_I2C_Master_Receive(hi2c, HDC1080_ADDR, buffer, 4, HAL_MAX_DELAY) != HAL_OK)
        return HAL_ERROR;

    // raw->real
    uint16_t raw_temp = (buffer[0] << 8) | buffer[1];
    uint16_t raw_humidity = (buffer[2] << 8) | buffer[3];

    data->temperature = (raw_temp / 65536.0) * 165.0 - 40.0; // Convert to °C
    data->humidity = (raw_humidity / 65536.0) * 100.0;       // Convert to %

    return HAL_OK;
}
