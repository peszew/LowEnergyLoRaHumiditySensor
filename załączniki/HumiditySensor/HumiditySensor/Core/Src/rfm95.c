#include "rfm95.h"
#include <string.h>
#include <stdbool.h>

static SPI_HandleTypeDef *rfmSpi;
static GPIO_TypeDef *rfmNssPort;
static uint16_t rfmNssPin;
static GPIO_TypeDef *rfmDio0Port;
static uint16_t rfmDio0Pin;

static void RFM95_WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t buffer[2] = {reg | 0x80, value};
    HAL_GPIO_WritePin(rfmNssPort, rfmNssPin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(rfmSpi, buffer, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(rfmNssPort, rfmNssPin, GPIO_PIN_SET);
}

void RFM95_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *nssPort, uint16_t nssPin, GPIO_TypeDef *dio0Port, uint16_t dio0Pin) {
    rfmSpi = hspi;
    rfmNssPort = nssPort;
    rfmNssPin = nssPin;
    rfmDio0Port = dio0Port;
    rfmDio0Pin = dio0Pin;

    HAL_GPIO_WritePin(rfmNssPort, rfmNssPin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(rfmDio0Port, rfmDio0Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(rfmDio0Port, rfmDio0Pin, GPIO_PIN_SET);
    HAL_Delay(10);

    RFM95_EnterSleepMode();
    HAL_Delay(10);
    RFM95_EnterStandbyMode();
    RFM95_WriteRegister(0x0E, 0x80); // FIFO TX
    RFM95_WriteRegister(0x0F, 0x00); // FIFO RX

    // frequency
    uint32_t frf = (uint32_t)((double)DEFAULT_FREQUENCY / 61.03515625);
    RFM95_WriteRegister(REG_FRF_MSB, (frf >> 16) & 0xFF);
    RFM95_WriteRegister(REG_FRF_MID, (frf >> 8) & 0xFF);
    RFM95_WriteRegister(REG_FRF_LSB, frf & 0xFF);

    //Bandwidth, Coding Rate, Spreading Factor, CRC
    RFM95_WriteRegister(REG_MODEM_CONFIG1, DEFAULT_BANDWIDTH | DEFAULT_CODING_RATE);
    RFM95_WriteRegister(REG_MODEM_CONFIG2, DEFAULT_SPREADING_FACTOR | DEFAULT_CRC);
}


void RFM95_EnterSleepMode(void) {
    RFM95_WriteRegister(REG_OP_MODE, MODE_SLEEP);
    HAL_Delay(10);
}

void RFM95_EnterStandbyMode(void) {
    RFM95_WriteRegister(REG_OP_MODE, MODE_STDBY);
    HAL_Delay(10);
}

void RFM95_SetPayloadLength(uint8_t length) {
    RFM95_EnterStandbyMode();
    RFM95_WriteRegister(REG_PAYLOAD_LENGTH, length);
}

void RFM95_SendMessage(const char *message) {
    uint8_t length = strlen(message);
    RFM95_EnterStandbyMode();
    RFM95_WriteRegister(0x0D, 0x80); // fifo * -> tx

    for (uint8_t i = 0; i < length; i++) {
        RFM95_WriteRegister(REG_FIFO, message[i]);
    }

    RFM95_SetPayloadLength(length);
    RFM95_WriteRegister(REG_OP_MODE, MODE_TX);

    // tx / error
    uint32_t startTick = HAL_GetTick();
    while (HAL_GPIO_ReadPin(rfmDio0Port, rfmDio0Pin) == GPIO_PIN_RESET) {
        if (HAL_GetTick() - startTick > TX_TIMEOUT) {
            // Timeout
            RFM95_HandleError();
            return;
        }
    }

    RFM95_WriteRegister(REG_IRQ_FLAGS, 0x08); //clr txdone flag
}

void RFM95_HandleError(void) {
    // proba resetu modulu
    HAL_GPIO_WritePin(rfmDio0Port, rfmDio0Pin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(rfmDio0Port, rfmDio0Pin, GPIO_PIN_SET);
    HAL_Delay(10);

    RFM95_EnterSleepMode();
    HAL_Delay(10);

    uint32_t frf = (uint32_t)((double)DEFAULT_FREQUENCY / 61.03515625);
    RFM95_WriteRegister(REG_FRF_MSB, (frf >> 16) & 0xFF);
    RFM95_WriteRegister(REG_FRF_MID, (frf >> 8) & 0xFF);
    RFM95_WriteRegister(REG_FRF_LSB, frf & 0xFF);

    RFM95_WriteRegister(REG_MODEM_CONFIG1, DEFAULT_BANDWIDTH | DEFAULT_CODING_RATE);
    RFM95_WriteRegister(REG_MODEM_CONFIG2, DEFAULT_SPREADING_FACTOR | DEFAULT_CRC);
	}

