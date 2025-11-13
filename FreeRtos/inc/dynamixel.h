#ifndef DYNAMIXEL_H_
#define DYNAMIXEL_H_

#include "main.h"
#include <stdint.h>

void Dxl_Init(UART_HandleTypeDef *huart);
HAL_StatusTypeDef Dxl_Ping(uint8_t id);
HAL_StatusTypeDef Dxl_TorqueEnable(uint8_t id, uint8_t enable);
HAL_StatusTypeDef Dxl_Move(uint8_t id, uint32_t position);
HAL_StatusTypeDef Dxl_ReadPresentPosition(uint8_t id, uint32_t *position);
HAL_StatusTypeDef Dxl_SetOperatingMode(uint8_t id, uint8_t mode);
HAL_StatusTypeDef Dxl_SetProfileVelocity(uint8_t id, uint32_t velocity);
//uint8_t GetRx(uint8_t rx);

#endif /* DYNAMIXEL_H_ */
