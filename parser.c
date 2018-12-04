#include<stdio.h>
#include<stdint.h>
#include"typedefine.h"

#define target_buffer_len 127

void parse(logic_signal_packet *in,frame_struct *out)
{
    uint32_t count_1 = 0;
    uint32_t count_0 = 0;
    uint32_t check = 0;
    uint32_t result[4]= {0};
    //remove stuffing bit
    int i= target_buffer_len;
    int count = 159;
    int payload_len = -1;
    //printf("--------------parse-------------\n");
    while(i >= 0) {
        uint8_t cur_bit = (in->bits[count/32] & (0x01 << (count%32))) ? 0x01 : 0x00;
        //printf("%d ",cur_bit);

        if( cur_bit != 0) {
            count_1++;
            count_0 = 0;
        } else {
            count_0++;
            count_1 = 0;
        }
        /*printf("%d: %d( 0x%08X &  0x%08X = 0x%08X), counters = %d, %d\n",
                count, cur_bit, in->bits[count/32], (0x01 << (count%32)),
                (in->bits[count/32] & (0x01 << (count%32))),
                count_1,count_0);*/
        if(count_1 == 5 || count_0 == 5) {
            if(payload_len > 0) {
                if ( (target_buffer_len - i - 8*payload_len ) < 34 )
                    count --;
            } else
                count--;
            //printf("\t%d : %d , %d, %d\n", count, cur_bit, count_1, count_0);
            count_1 = 0;
            count_0 = 0;
        }


        {
            result[i/32] |= cur_bit << i%32;
            /*if(cur_bit)*/
            //    printf("result[%d] |= 0x%08x (i=%d)\n", i/32, cur_bit<<i%32, i);
            i--;
            if(target_buffer_len - i == 19) {
                payload_len = (result[3] >> 13) & 0xf;
                if(payload_len > 8)
                    payload_len = 8;

            }

        }
        if(target_buffer_len - i == 8*payload_len + 19 + 25)
            break;
        count--;

    }
    //printf("\n");
    //printf("result[3]:0x%08X\n",result[3]);
    //printf("result[2]:0x%08X\n",result[2]);
    //parser
    out->SOF = (result[3] >> 31) & 0x1;
    out->ID_A = 0;
    for(int i=30; i>=20; i--)
        out->ID_A = out->ID_A + (((result[3] >>i) & 0x1) << (i-20));
    out->RTR = (result[3] >> 19) & 0x1;
    out->IDE = (result[3] >> 18) & 0x1;
    out->r0 = (result[3] >> 17) & 0x1;
    out->DLC = (result[3] >> 13) & 0xf;
    //out->DLC = ((result[3] >> 16) & 0x1) <<3 | ((result[3] >> 15) & 0x1) <<2 | ((result[3] >> 14) & 0x1) <<1 + (result[3] >> 13) & 0x1;
    //printf("DLC:%d\n",out->DLC);
    for(int j = 1; j < out->DLC +1; j++ ) {
        uint32_t data_base = target_buffer_len - 19 - 8*j;
        uint32_t _tmp = 0;
        if(data_base % 32 > 24 ) {//92
            _tmp = result[data_base/32] >> (data_base%32 +1);
            _tmp |= result[data_base/32 +1] << (31 - data_base%32);
            _tmp &= 0xff;
        } else {//100
            _tmp = result[data_base/32] >> (data_base%32 +1);
            _tmp &= 0xff;

        }
        //printf("_tmp:0x%02X\n",_tmp);
    }
    switch(out->DLC) {
        case 0:
            out->data[0]='\0';
            for(int i=24; i>=10; i--)
                out->CRC = out->CRC + (((result[2] >>i) & 0x1) << (i-10));
            out->CRC_del = (result[2] >> 9) & 0x1;
            out->ACK = (result[3] >> 8) & 0x1;
            out->ACK_del = (result[2] >> 7) & 0x1;
            for(int i=6; i>=0; i--)
                out->end_of_frame = out->end_of_frame + (((result[2] >>i) & 0x1) << i);
            break;
        case 1: {
            out->data[0]='\0';
            for(int i=12; i>=5; i--) {
                //printf("(((result[2] >>i) & 0x1):%X:\n",((result[3] >>i) & 0x1));
                out->data[0] = out->data[0] | (((result[3] >>i) & 0x1) << (i-5));
            }
            out->data[1] = '\0';
            for(int i=4; i>=0; i--)
                out->CRC = out->CRC + ((result[3] >>i) & 0x1) << (i+10);
            for(int i=31; i>=22; i--)
                out->CRC = out->CRC + ((result[2] >>i) & 0x1) << (i-22);
            out->CRC_del = (result[2] >> 21) & 0x1;
            out->ACK = (result[2] >> 20) & 0x1;
            out->ACK_del = (result[2] >> 19) & 0x1;
            for(int i=18; i>=12; i--)
                out->end_of_frame = out->end_of_frame + (((result[2] >>i) & 0x1) << (i-12));

            //printf("data[0]:0x%08X\n",out->data[0]);
            break;

        }
    }
}
