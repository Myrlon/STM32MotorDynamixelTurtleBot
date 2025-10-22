#include "dynamixel.h"
#include "dxl_protocol2.h"

static UART_HandleTypeDef *dxl_huart = NULL;

void Dxl_Init(UART_HandleTypeDef *huart)
{
    dxl_huart = huart;
}

HAL_StatusTypeDef Dxl_Ping(uint8_t id)
{
    return DXL_Ping(dxl_huart, id);
}

HAL_StatusTypeDef Dxl_TorqueEnable(uint8_t id, uint8_t enable)
{
    return DXL_Write1Byte(dxl_huart, id, ADDR_TORQUE_ENABLE, enable ? 1 : 0);
}

HAL_StatusTypeDef Dxl_Move(uint8_t id, uint32_t position)
{
    return DXL_Write4Byte(dxl_huart, id, ADDR_GOAL_POSITION, position);
}

HAL_StatusTypeDef Dxl_ReadPresentPosition(uint8_t id, uint32_t *position)
{
    uint8_t data[4];
    uint16_t got=0;
    HAL_StatusTypeDef st = DXL_Read(dxl_huart, id, ADDR_PRESENT_POSITION, 4, data, &got, 50);
    if (st != HAL_OK) return st;
    if (got < 4) return HAL_ERROR;
    *position = ((uint32_t)data[0]) | ((uint32_t)data[1] << 8) | ((uint32_t)data[2] << 16) | ((uint32_t)data[3] << 24);
    return HAL_OK;
}


HAL_StatusTypeDef Dxl_SetOperatingMode(uint8_t id, uint8_t mode)
{
    HAL_StatusTypeDef status;
    uint8_t read_mode = 0;
    uint16_t got = 0;

    // 1️Désactiver le couple
    status = Dxl_TorqueEnable(id, 0);
    if (status != HAL_OK) return status;
    HAL_Delay(20);

    // 2️Écrire le nouveau mode
    status = DXL_Write1Byte(dxl_huart, id, 11, mode);
    if (status != HAL_OK) return status;
    HAL_Delay(20);

    // 3️Vérifier la valeur écrite
    status = DXL_Read(dxl_huart, id, 11, 1, &read_mode, &got, 50);
    if (status != HAL_OK) return status;

    if (read_mode != mode)
    {
        printf("⚠️  Mode non modifié (valeur lue = %u)\r\n", read_mode);
        return HAL_ERROR;
    }

    printf("✅ Mode changé pour ID %u -> %u\r\n", id, mode);

    // 4️⃣ Réactiver le couple
    status = Dxl_TorqueEnable(id, 1);
    HAL_Delay(20);

    return status;
}


HAL_StatusTypeDef Dxl_SetProfileVelocity(uint8_t id, uint32_t velocity)
{
    HAL_StatusTypeDef status;
    uint8_t read_buf[4];
    uint16_t got = 0;

    // 1️ Écrire la nouvelle vitesse (adresse 112)
    status = DXL_Write4Byte(dxl_huart, id, 112, velocity);
    if (status != HAL_OK)
    {
        printf(" Erreur écriture profile velocity\r\n");
        return status;
    }
    HAL_Delay(10);

    // 2️ Lire pour confirmer
    status = DXL_Read(dxl_huart, id, 112, 4, read_buf, &got, 50);
    if (status != HAL_OK)
    {
        printf("Erreur lecture profile velocity\r\n");
        return status;
    }

    uint32_t val = read_buf[0] | (read_buf[1]<<8) | (read_buf[2]<<16) | (read_buf[3]<<24);

    if (val == velocity)
    {
        printf("profile velocity (ID %u) = %lu\r\n", id, (unsigned long)val);
        return HAL_OK;
    }
    else
    {
        printf("Valeur non confirmée (lue = %lu)\r\n", (unsigned long)val);
        return HAL_ERROR;
    }
}
