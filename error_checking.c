#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "error_checking.h"

int bit_stuffing_check(logic_signal_packet* input)
{
    uint32_t len = input->packet_len;
    uint32_t find_byte = len / 32;
    uint32_t find_bit = 32;
    uint32_t count = 0;

    for(int i = 0; i <= find_byte; i++) {
        if(i == find_byte)
            find_bit = len % 32;
        for(int j = 0; j < find_bit; j++) {
            if(((input->bits[4-i] >> (31-j)) & 1) == 1)
                count = 0;
            else
                count++;

            if(count > 5) return -1;
        }
    }
    return 0;
}

int frame_check(logic_signal_packet* input)
{
    uint32_t len = input->packet_len;
    uint32_t find_byte = len / 32;
    uint32_t find_bit = 32;
    uint32_t count = 0;
    uint32_t fill = 0;
    uint32_t position = 0;
    uint32_t check_bound = len - 10;
    bool fill_flag = 0;

    for(int i = 0; i <= find_byte; i++) {
        if(i == find_byte) find_bit = len % 32;
        for(int j = 0; j < find_bit; j++) {
            if(((input->bits[4-i] >> (31-j)) & 1) == 1) count = 0;
            else count++;

            position = i * 32 + j - fill;

            if(fill_flag == 1) {
                fill_flag = 0;
                continue;
            }

            if(position == 12 || position == 13 || position == 14) {
                if(((input->bits[4-i] >> (31-j)) & 1) == 1) return -1;
            }
            if((i * 32 + j) >= check_bound) {
                if(((input->bits[4-i] >> (31-j)) & 1) == 0) return -1;
            }

            if(count == 5) {
                fill++;
                fill_flag = 1;
            }
        }
    }
    return 0;
}

uint16_t can_crc_next(uint16_t crc, uint8_t data)
{
    uint8_t i;

    crc ^= (uint16_t)data << 7;

    for (i = 0; i < 8; i++) {
        crc <<= 1;
        if (crc & 0x8000) {
            crc ^= 0xc599;
        }
    }

    return crc & 0x7fff;
}

int crc_check(frame_struct* input)
{
    int data_num = input->DLC;
    uint8_t crc_input[11];
    for(int i = 0; i < data_num; i++) {
        crc_input[i + 3] = input->data[data_num - 1 - i];
    }
    crc_input[2] = (uint8_t)((input->ID_A & 1) << 7) + data_num;
    crc_input[1] = (uint8_t)(input->ID_A >> 1);
    crc_input[0] = (uint8_t)(input->ID_A >> 9);

    uint16_t crc;
    crc = 0;

    for (int i = 0; i < data_num + 3; i++) {
        crc = can_crc_next(crc, crc_input[i]);
    }

    if(crc == input->CRC)
        return 0;
    else
        return -1;
}
