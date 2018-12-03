#include<stdio.h>
#include<stdint.h>
#include"typedefine.h"
void parse(logic_signal_packet *in,frame_struct *out)
{
	uint32_t stuff_0 = 0x0;
	uint32_t stuff_1 = 0x1;
	printf("parser : %X %X %X %X %X %d\n",in->bits[4],in->bits[3],in->bits[2],in->bits[1],in->bits[0],in->packet_len);
	int count_1 = 0;
	int count_0 = 0;
	int N_stuff = 0;
	uint32_t check=0;	
	//uint32_t mask = 0xffffffff;
	uint32_t result[5]={0,0,0,0,0};
	uint32_t shift = 0;
	//remove stuffing bit
	for(int k=4;k>=0;k--)
	{
		for(int i=31;i>=0;i--)
		{
			N_stuff++;
			if(N_stuff <= in->packet_len -10)
			{
				count_0 = ((in->bits[k]>>i & stuff_1) == 0) ? count_0++ : 0;
				count_1 = ((in->bits[k]>>i | stuff_0) == 1) ? count_1++ : 0;
				check = ((count_0 == 5) || (count_1 == 5)) ? 1 : 0;			
			}
			else 
				check == 0;
			
			if(check == 0)
			{
				if(count_0 == 5)
					count_0 = 0;
				if(count_1 == 5)
					count_1 = 0;
		
				result[k] = result[k] +(in->bits[k] >> i & 0x1);
				shift++;
				result[k] = result[k] << shift;				
			}
		}
	}
	//parser
	out->SOF = (result[5] >> 31) & 0x1;
	
	for(int i=30;i>=20;i--)
		out->ID_A = out->ID_A + (((result[5] >>i) & 0x1) << (i-20));
	out->SRR = (result[5] >> 19) & 0x1;
	out->IDE = (result[5] >> 18) & 0x1;
	for(int i=17;i>=0;i--)	
		out->ID_B = out->ID_B + (((result[5] >>i) & 0x1) << i);
	out->RTR = (result[4] >> 31) & 0x1;
	out->Res = ((result[4] >> 30) & 0x1) <<1 + (result[4] >> 29) & 0x1 ;
	out->DLC = ((result[4] >> 28) & 0x1) <<3 +((result[4] >> 27) & 0x1) <<2 +((result[4] >> 26) & 0x1) <<1 + (result[4] >> 25) & 0x1;
	switch(out->DLC)
	{
		case 0:
		{
			out->data[0]='\0';
			for(int i=24;i>=10;i--)
				out->CRC = out->CRC + (((result[4] >>i) & 0x1) << (i-10));
			out->CRC_del = (result[4] >> 9) & 0x1;
			out->ACK = (result[4] >> 8) & 0x1;
			out->ACK_del = (result[4] >> 7) & 0x1;
			for(int i=6;i>=0;i--)
				out->end_of_frame = out->end_of_frame + (((result[4] >>i) & 0x1) << i);
		}
		case 1:
		{
			for(int i=24;i>=17;i--)
				out->data[0] = out->data[0] + (((result[4] >>i) & 0x1) << (i-17));
			out->data[1] = '\0';
			for(int i=16;i>=2;i--)
				out->CRC = out->CRC + (((result[4] >>i) & 0x1) << (i-2));
			out->CRC_del = (result[4] >> 1) & 0x1;
			out->ACK = (result[4] >> 0) & 0x1;
			out->ACK_del = (result[3] >> 31) & 0x1;
			for(int i=30;i>=24;i--)
				out->end_of_frame = out->end_of_frame + (((result[3] >>i) & 0x1) << (i-24));
		
		}
		case 2:
		{
			//data
			for(int i=24;i>=17;i--)
				out->data[0] = out->data[0] + (((result[4] >>i) & 0x1) << (i-17));
			for(int i=16;i>=9;i--)
				out->data[1] = out->data[1] + (((result[4] >>i) & 0x1) << (i-9));
			//CRC
			for(int i=8;i>=0;i--)
				out->CRC = out->CRC + (((result[4] >>i) & 0x1) << (i+6));
			for(int i=31;i>=26;i--)
				out->CRC = out->CRC + (((result[3] >>i) & 0x1) << (i-26));
			//CRC_del
			out->CRC_del = (result[3] >> 25) & 0x1;
			//ACK
			out->ACK = (result[3] >> 24) & 0x1;
			//ACK_del
			out->ACK_del = (result[3] >> 23) & 0x1;
			//end_of_frame			
			for(int i=22;i>=16;i--)
				out->end_of_frame = out->end_of_frame + (((result[3] >>i) & 0x1) << (i-16));
		}
		case 3:
		{
			//data
			for(int i=24;i>=17;i--)
				out->data[0] = out->data[0] + (((result[4] >>i) & 0x1) << (i-17));
			for(int i=16;i>=9;i--)
				out->data[1] = out->data[1] + (((result[4] >>i) & 0x1) << (i-9));
			for(int i=8;i>=1;i--)
				out->data[2] = out->data[2] + (((result[4] >>i) & 0x1) << (i-1));
			//CRC
			out->CRC = out->CRC + ((result[4] >>0) & 0x1) << 14;
			for(int i=31;i>=18;i--)
				out->CRC = out->CRC + (((result[3] >>i) & 0x1) << (i-18));
			//CRC_del
			out->CRC_del = (result[3] >> 17) & 0x1;
			//ACK
			out->ACK = (result[3] >> 16) & 0x1;
			//ACK_del
			out->ACK_del = (result[3] >> 15) & 0x1;
			//end_of_frame		
			for(int i=14;i>=8;i--)
				out->end_of_frame = out->end_of_frame + (((result[3] >>i) & 0x1) << (i-8));
		}
		case 4:
		{
			//data
			for(int i=24;i>=17;i--)
				out->data[0] = out->data[0] + (((result[4] >>i) & 0x1) << (i-17)); //data[0] 
			for(int i=16;i>=9;i--)
				out->data[1] = out->data[1] + (((result[4] >>i) & 0x1) << (i-9)); //data[1]
			for(int i=8;i>=1;i--)
				out->data[2] = out->data[2] + (((result[4] >>i) & 0x1) << (i-1)); //data[2]
			out->data[3] = out->data[3] + (((result[4] >>0) & 0x1) << 7);		  //data[3][0]	
			for(int i=31;i>=25;i--)
				out->data[3] = out->data[3] + (((result[3] >>i) & 0x1) << (i-25)); //data[3][1:7]
			//CRC
			for(int i=24;i>=10;i--)
				out->CRC = out->CRC + (((result[3] >>i) & 0x1) << (i-10));
			//CRC_del
			out->CRC_del = (result[3] >> 9) & 0x1;
			//ACK
			out->ACK = (result[3] >> 8) & 0x1;
			//ACK_del
			out->ACK_del = (result[3] >> 7) & 0x1;
			//end_of_frame		
			for(int i=6;i>=0;i--)
				out->end_of_frame = out->end_of_frame + (((result[3] >>i) & 0x1) << i);
		}
		case 5:
		{
			//data
			for(int i=24;i>=17;i--)
				out->data[0] = out->data[0] + (((result[4] >>i) & 0x1) << (i-17)); //data[0] 
			for(int i=16;i>=9;i--)
				out->data[1] = out->data[1] + (((result[4] >>i) & 0x1) << (i-9)); //data[1]
			for(int i=8;i>=1;i--)
				out->data[2] = out->data[2] + (((result[4] >>i) & 0x1) << (i-1)); //data[2]
			out->data[3] = out->data[3] + (((result[4] >>0) & 0x1) << 7);		  //data[3][0]	
			for(int i=31;i>=25;i--)
				out->data[3] = out->data[3] + (((result[3] >>i) & 0x1) << (i-25)); //data[3][1:7]
			for(int i=31;i>=25;i--)
				out->data[4] = out->data[4] + (((result[3] >>i) & 0x1) << (i-25)); //data[3][1:7]
			//CRC
			for(int i=24;i>=10;i--)
				out->CRC = out->CRC + (((result[3] >>i) & 0x1) << (i-10));
			//CRC_del
			out->CRC_del = (result[3] >> 9) & 0x1;
			//ACK
			out->ACK = (result[3] >> 8) & 0x1;
			//ACK_del
			out->ACK_del = (result[3] >> 7) & 0x1;
			//end_of_frame		
			for(int i=6;i>=0;i--)
				out->end_of_frame = out->end_of_frame + (((result[3] >>i) & 0x1) << i);
		}
		case 6:
		{
			out->data[0]='\0';
			for(int i=24;i>=10;i--)
				out->CRC = out->CRC + (((result[4] >>i) & 0x1) << (i-10));
			out->CRC_del = (result[4] >> 9) & 0x1;
			out->ACK = (result[4] >> 8) & 0x1;
			out->ACK_del = (result[4] >> 7) & 0x1;
			for(int i=6;i>=0;i--)
				out->end_of_frame = out->end_of_frame + (((result[4] >>i) & 0x1) << i);
		}
		case 7:
		{
			out->data[0]='\0';
			for(int i=24;i>=10;i--)
				out->CRC = out->CRC + (((result[4] >>i) & 0x1) << (i-10));
			out->CRC_del = (result[4] >> 9) & 0x1;
			out->ACK = (result[4] >> 8) & 0x1;
			out->ACK_del = (result[4] >> 7) & 0x1;
			for(int i=6;i>=0;i--)
				out->end_of_frame = out->end_of_frame + (((result[4] >>i) & 0x1) << i);
		}
		default:
		{
			out->data[0]='\0';
			for(int i=24;i>=10;i--)
				out->CRC = out->CRC + (((result[4] >>i) & 0x1) << (i-10));
			out->CRC_del = (result[4] >> 9) & 0x1;
			out->ACK = (result[4] >> 8) & 0x1;
			out->ACK_del = (result[4] >> 7) & 0x1;
			for(int i=6;i>=0;i--)
				out->end_of_frame = out->end_of_frame + (((result[4] >>i) & 0x1) << i);
		}
	}
}












	/*for(int i=31;i>=0;i--)
	{
		
		
		count_0 = ((a>>i & stuff_1) == 0) ? count_0++ : 0;
		count_1 = ((a>>i | stuff_0) == 1) ? count_1++ : 0;				
		mask = mask >> 1;
		 
		if(count_0 == 5)
		{	
			shift++;
			mask1 = mask >> 1 ;
			b = b & ~mask1;
			b = ((a & mask1) << shift) + (a & ~mask); 
			count_0 = 0;
			
		}
		else if(count_1 == 5)
		{
			shift++;
			mask1 = mask >> 1 ;
			b = ((a & mask1) << shift) + (a & ~mask);
			count_1 = 0;
		}
			
			
	}*/
	

