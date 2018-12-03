#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "typedefine.h"
//#include "error_checking.c"


int receive();
int signal_convert(int h, int l);
void save_data(frame_struct* , int, int);

int receive()
{
    int i;
    int count=0;
	FILE *H = fopen("High.txt","r");
    FILE *L = fopen("Low.txt","r");
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
        while((chH = fgetc(H)) != EOF) { 
            chL = fgetc(L);
            int temp1;
            int temp2;
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
                    printf("result: %d\n",result);
                    //while(1){
                        packet->bits[i/32] = packet->bits[i/32] + (result << (i%32));
                        temp = i;
                        i--;
                    //}
                }
                else {
                    count++;
                    packet->packet_len = 160 - temp;/*
                    if(bit_stuffing_check(packet)==-1)
                        printf("bit_stuffing_check Error\n");
                    if(frame_check(packet)==-1)
                        printf("frame_check Error\n");*/
                    parse(packet,result_packet);/*
                    if(crc_check(result_packet)==-1)
                    printf("crc_check Error\n");*/
                    save_data(result_packet,count, 160 - temp);
    printf("---------------------------------\n");
    printf("%X %X %X %X %X %d\n",packet->bits[4],packet->bits[3],packet->bits[2],packet->bits[1],packet->bits[0],packet->packet_len);
                    for(i=0;i<5;i++)
                        packet->bits[i]=0;
                    packet->packet_len=0;
                    i=159;
                }
            }
        }
    }
}

int signal_convert(int h, int l)
{
    printf("signal_convert : %d %d\n",h,l);
	short result = 0;
	result=	(h==1 && l==-1) ? 0 : 
			(h==0 && l== 0) ? 1 : -1;
	return result;
}

void save_data(frame_struct* frame , int count, int packet_length)
{
	printf("ID:%d, data:%s, packet count:%d, packet length:%d\n", frame->ID_A, frame->data, count, packet_length);
}
