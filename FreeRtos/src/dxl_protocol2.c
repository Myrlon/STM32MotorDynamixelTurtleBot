#include "dxl_protocol2.h"
#include "string.h"

HAL_StatusTypeDef DXL_SendPacket(UART_HandleTypeDef *huart, uint8_t *packet, uint16_t length)
{
	HAL_HalfDuplex_EnableTransmitter(huart);
    HAL_StatusTypeDef st = HAL_UART_Transmit(huart, packet, length, HAL_MAX_DELAY);
    HAL_HalfDuplex_EnableReceiver(huart);
  //  HAL_Delay(2);
    return st;
}


static const uint16_t crc_table[256] =
		{ 0x0000,
		    0x8005, 0x800F, 0x000A, 0x801B, 0x001E, 0x0014, 0x8011,
		    0x8033, 0x0036, 0x003C, 0x8039, 0x0028, 0x802D, 0x8027,
		    0x0022, 0x8063, 0x0066, 0x006C, 0x8069, 0x0078, 0x807D,
		    0x8077, 0x0072, 0x0050, 0x8055, 0x805F, 0x005A, 0x804B,
		    0x004E, 0x0044, 0x8041, 0x80C3, 0x00C6, 0x00CC, 0x80C9,
		    0x00D8, 0x80DD, 0x80D7, 0x00D2, 0x00F0, 0x80F5, 0x80FF,
		    0x00FA, 0x80EB, 0x00EE, 0x00E4, 0x80E1, 0x00A0, 0x80A5,
		    0x80AF, 0x00AA, 0x80BB, 0x00BE, 0x00B4, 0x80B1, 0x8093,
		    0x0096, 0x009C, 0x8099, 0x0088, 0x808D, 0x8087, 0x0082,
		    0x8183, 0x0186, 0x018C, 0x8189, 0x0198, 0x819D, 0x8197,
		    0x0192, 0x01B0, 0x81B5, 0x81BF, 0x01BA, 0x81AB, 0x01AE,
		    0x01A4, 0x81A1, 0x01E0, 0x81E5, 0x81EF, 0x01EA, 0x81FB,
		    0x01FE, 0x01F4, 0x81F1, 0x81D3, 0x01D6, 0x01DC, 0x81D9,
		    0x01C8, 0x81CD, 0x81C7, 0x01C2, 0x0140, 0x8145, 0x814F,
		    0x014A, 0x815B, 0x015E, 0x0154, 0x8151, 0x8173, 0x0176,
		    0x017C, 0x8179, 0x0168, 0x816D, 0x8167, 0x0162, 0x8123,
		    0x0126, 0x012C, 0x8129, 0x0138, 0x813D, 0x8137, 0x0132,
		    0x0110, 0x8115, 0x811F, 0x011A, 0x810B, 0x010E, 0x0104,
		    0x8101, 0x8303, 0x0306, 0x030C, 0x8309, 0x0318, 0x831D,
		    0x8317, 0x0312, 0x0330, 0x8335, 0x833F, 0x033A, 0x832B,
		    0x032E, 0x0324, 0x8321, 0x0360, 0x8365, 0x836F, 0x036A,
		    0x837B, 0x037E, 0x0374, 0x8371, 0x8353, 0x0356, 0x035C,
		    0x8359, 0x0348, 0x834D, 0x8347, 0x0342, 0x03C0, 0x83C5,
		    0x83CF, 0x03CA, 0x83DB, 0x03DE, 0x03D4, 0x83D1, 0x83F3,
		    0x03F6, 0x03FC, 0x83F9, 0x03E8, 0x83ED, 0x83E7, 0x03E2,
		    0x83A3, 0x03A6, 0x03AC, 0x83A9, 0x03B8, 0x83BD, 0x83B7,
		    0x03B2, 0x0390, 0x8395, 0x839F, 0x039A, 0x838B, 0x038E,
		    0x0384, 0x8381, 0x0280, 0x8285, 0x828F, 0x028A, 0x829B,
		    0x029E, 0x0294, 0x8291, 0x82B3, 0x02B6, 0x02BC, 0x82B9,
		    0x02A8, 0x82AD, 0x82A7, 0x02A2, 0x82E3, 0x02E6, 0x02EC,
		    0x82E9, 0x02F8, 0x82FD, 0x82F7, 0x02F2, 0x02D0, 0x82D5,
		    0x82DF, 0x02DA, 0x82CB, 0x02CE, 0x02C4, 0x82C1, 0x8243,
		    0x0246, 0x024C, 0x8249, 0x0258, 0x825D, 0x8257, 0x0252,
		    0x0270, 0x8275, 0x827F, 0x027A, 0x826B, 0x026E, 0x0264,
		    0x8261, 0x0220, 0x8225, 0x822F, 0x022A, 0x823B, 0x023E,
		    0x0234, 0x8231, 0x8213, 0x0216, 0x021C, 0x8219, 0x0208,
		    0x820D, 0x8207, 0x0202
};
uint16_t DXL_UpdateCRC(uint16_t crc_accum, uint8_t *data_blk_ptr, uint16_t data_blk_size)
{
  uint16_t i, j;
  for (j = 0; j < data_blk_size; j++)
  {
    i = ((uint16_t)(crc_accum >> 8) ^ data_blk_ptr[j]) & 0xFF;
    crc_accum = (crc_accum << 8) ^ crc_table[i];
  }
  return crc_accum;
}


HAL_StatusTypeDef DXL_ReceivePacket(UART_HandleTypeDef *huart, uint8_t *rx_buf, uint16_t rx_buf_size, uint16_t *out_len, uint32_t timeout_ms)
{
    /*if (rx_buf_size < 16) return HAL_ERROR;

    HAL_StatusTypeDef st;
    uint16_t to_read = 7;
    uint32_t t0 = HAL_GetTick();
    uint16_t read = 0;
    while (read < 7 && (HAL_GetTick() - t0) < timeout_ms)
    {
        if (HAL_UART_Receive(huart, rx_buf + read, 1, 10) == HAL_OK)
            read++;
    }

    // validate header quickly
    if (!(rx_buf[0] == 0xFF && rx_buf[1] == 0xFF && rx_buf[2] == 0xFD && rx_buf[3] == 0x00))
    {
        return HAL_ERROR;
    }

    uint16_t len_field = rx_buf[5] | (rx_buf[6] << 8);
    uint16_t remain = (uint16_t)(len_field + 2);
    if ((7 + remain) > rx_buf_size) return HAL_ERROR;

    // read remaining bytes
    read = 0;
    while (read < remain)
    {
        st = HAL_UART_Receive(huart, rx_buf + 7 + read, remain - read, timeout_ms);
        if (st != HAL_OK) return st;
        read = remain;
    }
    //simple version
  //  if (read < 7) return HAL_TIMEOUT;

    *out_len = (uint16_t)(7 + remain);

    uint16_t crc_calc = DXL_UpdateCRC(0, rx_buf, *out_len - 2);
    uint16_t crc_recv = rx_buf[*out_len - 2] | (rx_buf[*out_len - 1] << 8);
    if (crc_calc != crc_recv) return HAL_ERROR;

    return HAL_OK;*/
	 if (rx_buf_size < 16) return HAL_ERROR;
    uint32_t start = HAL_GetTick();
        uint8_t hdr[7];
        int hdr_got = 0;
        HAL_StatusTypeDef st;
        uint8_t b;

        // 1) Resynchronize: read bytes until we find header 0xFF 0xFF 0xFD 0x00
        while ((HAL_GetTick() - start) < timeout_ms)
        {
            st = HAL_UART_Receive_IT(huart, &b, 1);
            if (st != HAL_OK) {
                // no byte this 10ms slice -> continue until global timeout
                continue;
            }

            // append to hdr buffer (sliding window)
            if (hdr_got < 7) {
                hdr[hdr_got++] = b;
            } else {
                // shift left 1 and append
                memmove(hdr, hdr + 1, 6);
                hdr[6] = b;
            }

            // check for header sequence at the end of hdr buffer
            if (hdr_got >= 4) {
                int idx = hdr_got - 4;
                if (hdr[idx] == 0xFF && hdr[idx+1] == 0xFF && hdr[idx+2] == 0xFD && hdr[idx+3] == 0x00) {
                    // Found header. Now ensure we have id,lenL,lenH in buffer (3 bytes after header)
                    int avail_after_header = hdr_got - (idx + 4); // how many bytes we already have beyond header
                    // Build rx_buf[0..6] = header(4) + id + lenL + lenH
                    // first copy header bytes
                    rx_buf[0] = 0xFF; rx_buf[1] = 0xFF; rx_buf[2] = 0xFD; rx_buf[3] = 0x00;

                    int pos_in_hdr = idx + 4;
                    int copied = 0;
                    // copy available bytes after header from hdr buffer
                    while (copied < avail_after_header && copied < 3) {
                        rx_buf[4 + copied] = hdr[pos_in_hdr + copied];
                        copied++;
                    }

                    // if not enough bytes for id/len, read the missing ones
                    while (copied < 3) {
                        uint32_t tsub = HAL_GetTick();
                        // read one byte at a time, with small per-byte timeout to remain responsive
                        if (HAL_UART_Receive(huart, &rx_buf[4 + copied], 1, 50) != HAL_OK) {
                            // timeout for this byte -> overall failure
                            return HAL_TIMEOUT;
                        }
                        copied++;
                        // check overall timeout
                        if ((HAL_GetTick() - start) >= timeout_ms) return HAL_TIMEOUT;
                    }

                    // now we have first 7 bytes in rx_buf[0..6]
                    goto READ_REMAINING;
                }
            }
        }

        // global timeout while waiting header
        return HAL_TIMEOUT;

    READ_REMAINING:
        {
            // rx_buf[5] = lenL, rx_buf[6] = lenH
            uint16_t len_field = (uint16_t)rx_buf[5] | ((uint16_t)rx_buf[6] << 8);
            // In protocol 2.0: len_field = (error(1) + params(n) + CRC(2))
            // remaining bytes to read after the first 7 bytes = len_field
            uint16_t remaining = len_field;

            // ensure buffer big enough: total = 7 + remaining
            if ((uint32_t)7 + (uint32_t)remaining > rx_buf_size) return HAL_ERROR;

            uint16_t idx = 7;
            uint32_t tsub_start = HAL_GetTick();
            while (remaining > 0) {
                // read in chunks if desired, here we read byte-by-byte with small timeout to be robust
                st = HAL_UART_Receive(huart, &rx_buf[idx], 1, 50);
                if (st != HAL_OK) {
                    // if per-byte timeout, check global timeout
                    if ((HAL_GetTick() - start) >= timeout_ms) return HAL_TIMEOUT;
                    // else try again
                    continue;
                }
                idx++;
                remaining--;
                // protect against infinite loop
                if ((HAL_GetTick() - start) >= timeout_ms) return HAL_TIMEOUT;
            }

            uint16_t total_len = idx; // equals 7 + len_field

            // CRC verification: last two bytes are CRC (low, high)
            if (total_len < 3) return HAL_ERROR;
            uint16_t recv_crc = (uint16_t)rx_buf[total_len - 2] | ((uint16_t)rx_buf[total_len - 1] << 8);
            uint16_t calc_crc = DXL_UpdateCRC(0, rx_buf, total_len - 2);
            if (recv_crc != calc_crc) {
                return HAL_ERROR;
            }

            *out_len = total_len;
            return HAL_OK;
        }
}


HAL_StatusTypeDef DXL_Ping(UART_HandleTypeDef *huart, uint8_t id)
{
    uint8_t pkt[10] = {0xFF,0xFF,0xFD,0x00, id, 0x03,0x00, INST_PING, 0x00, 0x00};
    uint16_t crc = DXL_UpdateCRC(0, pkt, 8);
    pkt[8] = crc & 0xFF;
    pkt[9] = (crc >> 8) & 0xFF;
    return DXL_SendPacket(huart, pkt, sizeof(pkt));
}


HAL_StatusTypeDef DXL_Write1Byte(UART_HandleTypeDef *huart, uint8_t id, uint16_t address, uint8_t value)
{
    uint8_t pkt[13];
    memset(pkt, 0, sizeof(pkt));
    pkt[0]=0xFF; pkt[1]=0xFF; pkt[2]=0xFD; pkt[3]=0x00; pkt[4]=id;
    pkt[5]=0x06; pkt[6]=0x00; // length = 6 (instruction + params: addr L, addr H, data(1))
    pkt[7]=INST_WRITE;
    pkt[8] = (uint8_t)(address & 0xFF);
    pkt[9] = (uint8_t)((address >> 8) & 0xFF);
    pkt[10] = value;
    uint16_t crc = DXL_UpdateCRC(0, pkt, 11);
    pkt[11] = crc & 0xFF;
    pkt[12] = (crc >> 8) & 0xFF;
    return DXL_SendPacket(huart, pkt, 13);
}

/* Write 4 bytes (little-endian) */
HAL_StatusTypeDef DXL_Write4Byte(UART_HandleTypeDef *huart, uint8_t id, uint16_t address, uint32_t value)
{
    uint8_t pkt[16];
    memset(pkt,0,sizeof(pkt));
    pkt[0]=0xFF; pkt[1]=0xFF; pkt[2]=0xFD; pkt[3]=0x00; pkt[4]=id;
    pkt[5]=0x09; pkt[6]=0x00; // length = 9 (instruction + params addr(2)+data(4))
    pkt[7]=INST_WRITE;
    pkt[8] = (uint8_t)(address & 0xFF);
    pkt[9] = (uint8_t)((address >> 8) & 0xFF);
    pkt[10] = (uint8_t)(value & 0xFF);
    pkt[11] = (uint8_t)((value >> 8) & 0xFF);
    pkt[12] = (uint8_t)((value >> 16) & 0xFF);
    pkt[13] = (uint8_t)((value >> 24) & 0xFF);
    uint16_t crc = DXL_UpdateCRC(0, pkt, 14);
    pkt[14] = crc & 0xFF;
    pkt[15] = (crc >> 8) & 0xFF;
    return DXL_SendPacket(huart, pkt, 16);
}


HAL_StatusTypeDef DXL_Read(UART_HandleTypeDef *huart, uint8_t id, uint16_t address, uint16_t read_len, uint8_t *out_data, uint16_t *out_len, uint32_t timeout_ms)
{
    // Build read packet
    uint8_t pkt[12];
    memset(pkt,0,sizeof(pkt));
    pkt[0]=0xFF; pkt[1]=0xFF; pkt[2]=0xFD; pkt[3]=0x00; pkt[4]=id;
    pkt[5]=0x07; pkt[6]=0x00; // length = 7 (instruction + params addr(2) + data_length(2))
    pkt[7]=INST_READ;
    pkt[8] = (uint8_t)(address & 0xFF);
    pkt[9] = (uint8_t)((address >> 8) & 0xFF);
    pkt[10] = (uint8_t)(read_len & 0xFF);
    pkt[11] = (uint8_t)((read_len >> 8) & 0xFF);
    uint16_t crc = DXL_UpdateCRC(0, pkt, 12 - 2);
    uint8_t packet_full[14];
    memcpy(packet_full, pkt, 12);
    uint16_t crc_full = DXL_UpdateCRC(0, packet_full, 12);
    packet_full[12] = crc_full & 0xFF;
    packet_full[13] = (crc_full >> 8) & 0xFF;

    // send
    HAL_StatusTypeDef st = DXL_SendPacket(huart, packet_full, 14);
    if (st != HAL_OK) return st;

    // receive status packet
    uint8_t rxbuf[64];
    //volatile uint8_t rx[64];
    uint16_t rxlen=0;
    //new enable receiveing on half duplex pin !!
    HAL_HalfDuplex_EnableReceiver(huart);
    st = DXL_ReceivePacket(huart, rxbuf, sizeof(rxbuf), &rxlen, timeout_ms);
    HAL_HalfDuplex_EnableTransmitter(huart);
  //check directly rx buffer from IT Receiver
    //  if (st != HAL_OK) return st;
    if(rx[0]=='\0') return HAL_ERROR;

    //rx version
    uint16_t len_field = rx[5] | (rx[6] << 8);
       uint16_t param_len = (len_field > 2) ? (len_field - 2) : 0;
       if (param_len > read_len) param_len = read_len;
       if (param_len)
       {
           memcpy(out_data, &rx[9], param_len);
       }
       *out_len = param_len;
       return HAL_OK;
    /*
     * st version
    uint16_t len_field = rxbuf[5] | (rxbuf[6] << 8);
    uint16_t param_len = (len_field > 2) ? (len_field - 2) : 0;
    if (param_len > read_len) param_len = read_len;
    if (param_len)
    {
        memcpy(out_data, &rxbuf[9], param_len);
    }
    *out_len = param_len;
    return HAL_OK;*/
}


