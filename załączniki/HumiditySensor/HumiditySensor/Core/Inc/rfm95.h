#ifndef RFM95_H
#define RFM95_H

#include "stm32l0xx_hal.h"

// RFM95 Registers
#define REG_OP_MODE 0x01
#define MODE_SLEEP 0x00
#define MODE_STDBY 0x01
#define MODE_TX 0x03
#define REG_FRF_MSB 0x06
#define REG_FRF_MID 0x07
#define REG_FRF_LSB 0x08
#define REG_FIFO 0x00
#define REG_PAYLOAD_LENGTH 0x22
#define REG_IRQ_FLAGS 0x12
#define REG_MODEM_CONFIG1 0x1D
#define REG_MODEM_CONFIG2 0x1E


#define DEFAULT_FREQUENCY 868000000 //
#define DEFAULT_BANDWIDTH 0x72 // 125 kHz
#define DEFAULT_CODING_RATE 0x04 // 4/5
#define DEFAULT_SPREADING_FACTOR 0x74 // SF7
#define DEFAULT_CRC 0x04 // CRC
#define TX_TIMEOUT 3000 // Timeout

void RFM95_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *nssPort, uint16_t nssPin, GPIO_TypeDef *dio0Port, uint16_t dio0Pin);
void RFM95_EnterSleepMode(void);
void RFM95_EnterStandbyMode(void);
void RFM95_SetPayloadLength(uint8_t length);
void RFM95_SendMessage(const char *message);
void RFM95_HandleError(void);


#endif
