#ifndef TYPEDEFINE_H
#define TYPEDEFINE_H

#include <stdint.h>

typedef struct {
    uint32_t bits[5];
    uint32_t packet_len;
} logic_signal_packet; //the first bit is lsb

typedef  struct {
    uint32_t ID_A;
    char IDE:1;
    char RTR:1;
    char Res:1;
    char DLC:4;
    char data[9];    //the last element is for '\0'
    uint32_t CRC:15;
    char CRC_del:1;
    char ACK:1;
    char ACK_del:1;
    char end_of_frame:7;
} frame_struct;

#endif  //TYPEDEFINE_H
