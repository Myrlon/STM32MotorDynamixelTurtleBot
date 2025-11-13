#ifndef DXL_PROTOCOL2_H_
#define DXL_PROTOCOL2_H_

#include "main.h"
#include <stdint.h>


/* Protocol 2.0 instructions */
#define INST_PING   0x01
#define INST_READ   0x02
#define INST_WRITE  0x03

/* XM430 control table  */
#define ADDR_TORQUE_ENABLE     64   // 1 byte
#define ADDR_PRESENT_TEMPERATURE     146   // 1 byte
#define ADDR_BAUDRATE          8    // 1 byte
#define ADDR_OPERATING_MODE  11  // 1 bytes
#define ADDR_GOAL_POSITION     116  // 4 bytes
#define ADDR_PRESENT_POSITION  132  // 4 bytes
#define ADDR_GOAL_VELOCITY     104  // 4 bytes
#define ADDR_PRESENT_VELOCITY  128  // 4 bytes
#define ADDR_PROFILE_VELOCITY  112  // 4 bytes


//Rx reading buffer
extern volatile uint8_t rx[64];

//CRC

uint16_t DXL_UpdateCRC(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size);

/* API */
HAL_StatusTypeDef DXL_SendPacket(UART_HandleTypeDef *huart, uint8_t *packet, uint16_t length);
HAL_StatusTypeDef DXL_ReceivePacket(UART_HandleTypeDef *huart, uint8_t *rx_buf, uint16_t rx_buf_size, uint16_t *out_len, uint32_t timeout_ms);

HAL_StatusTypeDef DXL_Ping(UART_HandleTypeDef *huart, uint8_t id);
HAL_StatusTypeDef DXL_Write1Byte(UART_HandleTypeDef *huart, uint8_t id, uint16_t address, uint8_t value);
HAL_StatusTypeDef DXL_Write4Byte(UART_HandleTypeDef *huart, uint8_t id, uint16_t address, uint32_t value);
HAL_StatusTypeDef DXL_Read(UART_HandleTypeDef *huart, uint8_t id, uint16_t address, uint16_t read_len, uint8_t *out_data, uint16_t *out_len, uint32_t timeout_ms);

#endif /* DXL_PROTOCOL2_H_ */
