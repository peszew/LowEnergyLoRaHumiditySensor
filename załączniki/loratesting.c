#include "loratesting.h"
#include <string.h>

static SPI_HandleTypeDef *loraSpi;
static GPIO_TypeDef *loraNssPort;
static uint16_t loraNssPin;
static GPIO_TypeDef *loraResetPort;
static uint16_t loraResetPin;


static void write_register(uint8_t reg, uint8_t value) {
    HAL_GPIO_WritePin(loraNssPort, loraNssPin, GPIO_PIN_RESET); 
    uint8_t data[2] = {reg | 0x80, value}; 
    HAL_SPI_Transmit(loraSpi, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(loraNssPort, loraNssPin, GPIO_PIN_SET);   
}

static uint8_t read_register(uint8_t reg) {
    HAL_GPIO_WritePin(loraNssPort, loraNssPin, GPIO_PIN_RESET); 
    uint8_t data[2] = {reg & 0x7F, 0x00}; 
    HAL_SPI_TransmitReceive(loraSpi, data, data, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(loraNssPort, loraNssPin, GPIO_PIN_SET);   
    return data[1];
}

// LoRa Initialization
void LoRa_Init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *nssPort, uint16_t nssPin, GPIO_TypeDef *resetPort, uint16_t resetPin) {
    loraSpi = hspi;
    loraNssPort = nssPort;
    loraNssPin = nssPin;
    loraResetPort = resetPort;
    loraResetPin = resetPin;

   
    HAL_GPIO_WritePin(loraResetPort, loraResetPin, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(loraResetPort, loraResetPin, GPIO_PIN_SET);
    HAL_Delay(10);

    
    LoRa_SetFrequency(FRF_MSB, FRF_MID, FRF_LSB);

   
    write_register(REG_MODEM_CONFIG1, 0x72); 
    write_register(REG_MODEM_CONFIG2, 0x74); 

     //Set to Standby Mode
    write_register(REG_OP_MODE, MODE_STDBY);
}

// Set LoRa Frequency
void LoRa_SetFrequency(uint8_t msb, uint8_t mid, uint8_t lsb) {
    write_register(REG_FRF_MSB, msb);
    write_register(REG_FRF_MID, mid);
    write_register(REG_FRF_LSB, lsb);
}

// Send a Message
void LoRa_SendMessage(const char *message) {
    uint8_t length = strlen(message);
    // Set to Standby Mode
    write_register(REG_OP_MODE, MODE_STDBY);
    // Set FIFO pointer
    write_register(REG_FIFO_ADDR_PTR, 0x00);
    // Load data into FIFO
    for (uint8_t i = 0; i < length; i++) {
        write_register(REG_FIFO, message[i]);
    }
    // Set payload length
    write_register(REG_PAYLOAD_LENGTH, length);
    // Switch to Transmit Mode
    write_register(REG_OP_MODE, MODE_TX);
    // Wait for TX Done with a timeout
    uint32_t timeout = HAL_GetTick() + 2000; // 2-second timeout
    while ((read_register(REG_IRQ_FLAGS) & 0x08) == 0) {
        if (HAL_GetTick() > timeout) {
            printf("Error: TX timeout\n");
            break; 
        }
    }
    // ClrIRQflags
    write_register(REG_IRQ_FLAGS, 0xFF);

    // Standyby
    write_register(REG_OP_MODE, MODE_STDBY);
}


