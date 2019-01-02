#ifndef ERROR_CHECKING_H
#define ERROR_CHECKING_H

#include <stdint.h>
#include "typedefine.h"

int bit_stuffing_check(logic_signal_packet* input);
int frame_check(logic_signal_packet* input);
uint16_t can_crc_next(uint16_t crc, uint8_t data);
int crc_check(frame_struct* input);

#endif
