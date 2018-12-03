#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <typedefine.h>

int bit_stuffing_check(logic_signal_packet* input)
{
	uint32_t len = input->packet_len;
	uint32_t find_byte = len / 32;
	uint32_t find_bit = 32;
	uint32_t count = 0;

	for(int i = 0; i <= find_byte; i++)
	{
		if(i == find_byte) find_bit = len % 32;
		for(int j = 0; j < find_bit; j++)
		{
			if((input->bits[i] >> j) & 1 == 1) count = 0;
			else count++;

			if(count > 5) return 0;
		}
	}
	return 1;
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

	for(int i = 0; i <= find_byte; i++)
	{
		if(i == find_byte) find_bit = len % 32;
		for(int j = 0; j < find_bit; j++)
		{
			if((input->bits[i] >> j) & 1 == 1) count = 0;
			else count++;

			position = i * 32 + j - fill;

			if(fill_flag == 1)
			{
				fill_flag = 0;
				continue;
			}

			if(position == 12 || position == 13 || position == 14)
			{
				if((input->bits[i] >> j) & 1 == 1) return -1;
			}
			if((i * 32 + j) >= check_bound)
			{
				if((input->bits[i] >> j) & 1 == 0) return -1;
			}

			if(count == 5)
			{
				fill++;
				fill_flag = 1;
			}
		}
	}
	return 0;
}

int crc_check(frame_struct* input)
{

}
