#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "typedefine.h"
#include "parser.c"
#include "error_checking.c"


int receive();
int signal_convert(int h, int l);
void save_data(frame_struct* , int, int);

int receive()
{
    int i;
    int count=0;
	FILE *H = fopen("test_case/High.txt","r");
    FILE *L = fopen("test_case/Low.txt","r");
    char chH,chL;
    int result;
    int temp = 160;
    logic_signal_packet *packet = malloc(sizeof(logic_signal_packet));
    frame_struct *result_packet = malloc(sizeof(frame_struct));
    packet->packet_len=0;
    for(i=0;i<5;i++)
        packet->bits[i]=0;
    if( NULL == H ){ 
        printf( "High.txt open failure\n" );
        return 0;
    }
    else if( NULL == L ) {
        printf( "Low.txt open failure\n" );
        return 0;
    }
    else {
        i=159;
        int temp1;
        int temp2;
        while((chH = fgetc(H)) != EOF) { 
            chL = fgetc(L);
            if(chH != ' '){
                if(chH=='1')temp1=1;
                else temp1=0;
                if(chL=='-'){
                    temp2=-1;
                    fgetc(L);
                }
                else temp2=0;
                if(chH != '\n'){
                    result = signal_convert(temp1,temp2);
                    packet->bits[i/32] = packet->bits[i/32] + (result << (i%32));
                    temp = i;
                    i--;
                }
                else {
                    packet->packet_len = 160 - temp;
                    parse(packet,result_packet);
                    if(bit_stuffing_check(packet)==-1)
                        printf("bit_stuffing_check Error\n");
                    else if(frame_check(packet)==-1)
                        printf("frame_check Error\n");
                    else if(crc_check(result_packet)==-1)
                        printf("crc_check Error\n");
                    else 
                        save_data(result_packet,++count, 160 - temp);
                    for(i=0;i<5;i++)
                        packet->bits[i]=0;
                    packet->packet_len=0;
                    i=159;
                }
            }
        }
        packet->packet_len = 160 - temp;
        parse(packet,result_packet);
        if(bit_stuffing_check(packet)==-1)
            printf("bit_stuffing_check Error\n");
        else if(frame_check(packet)==-1)
            printf("frame_check Error\n");
        else if(crc_check(result_packet)==-1)
            printf("crc_check Error\n");
        else 
            save_data(result_packet,++count, 160 - temp);
    }
}

int signal_convert(int h, int l)
{
	short result = 0;
	result=	(h==1 && l==-1) ? 0 : 
			(h==0 && l== 0) ? 1 : -1;
	return result;
}

void save_data(frame_struct* frame , int count, int packet_length)
{
	printf("ID:%2d, data:%s, packet count:%d, packet length:%d\n", frame->ID_A, frame->data, count, packet_length);
}
