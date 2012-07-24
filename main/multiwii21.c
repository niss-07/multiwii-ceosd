#include "multiwii21.h"
#include "main.h"
#include "usart.h"
#include <stdio.h>

#define MULTIWII_MAX_PAYLOAD_SIZE 32

extern int32_t	m_alt;
extern int16_t m_pitch, m_roll;
extern int16_t m_distance_home, m_direction_home;

uint8_t checksum;

void parse_cmd(int cmd, int psize, uint8_t* buffer);

int got_responce(void)
{
	int rx_size;
	
	rx_size = _rx_buffer(1);
	if(rx_size < 6)
		return 0;
	
	return 1;
}

int parse_responce()
{
	int rx_size, c, state = 0, psize = 0, cmd = 0, i;
	uint8_t chsum = 0, buffer[MULTIWII_MAX_PAYLOAD_SIZE];
	
	rx_size = _rx_buffer(1);
	if(rx_size < 6)
		return 0;
	
	while((c = GetKey()) != -1)
	{
		LED_TOGGLE;
		if(c == '$' && state == 0)
		{
			state = 1;
			continue;
		}
		if(c == 'M' && state == 1)
		{
			state = 2;
			continue;
		}
		if(c == '>' && state == 2)
		{
			state = 3;
			continue;
		}
		// payload size
		if(state == 3)
		{
			state = 4;
			psize = c;
			chsum ^= c;
			continue;
		}
		// cmd
		if(state == 4)
		{
			state = 5;
			cmd = c;
			chsum ^= c;

			for(i=0;i<psize;i++)
			{
				while((c = GetKey()) == -1){}
				buffer[i] = c;
				chsum ^= c;
			}
			while((c = GetKey()) == -1);
			//if(chsum == c)
				parse_cmd(cmd, psize, buffer);
			return 1;
		}
	}
	return 0;
}

void parse_cmd(int cmd, int psize, uint8_t* buffer)
{
		switch(cmd)
		{
			case MSP_IDENT:
				print_version(buffer[0]);
				break;
			case MSP_STATUS:

				break;
			case MSP_COMP_GPS:
				{
					int16_t *p = (int16_t*)buffer;
					m_distance_home = p[0];
					m_direction_home = p[1];
				}
				break;
			case MSP_ATTITUDE:
				{
					int16_t *p = (int16_t*)buffer;
					m_pitch = p[1];
					m_roll  = p[0];
				}
				break;
			case MSP_ALTITUDE:
				{
					int32_t *p = (int32_t*)buffer;
					m_alt = p[0];
				}
				break;
			default:
				return;
		}
}

void send_command(uint8_t size, int cmd, uint8_t *p)
{
	checksum = 0;
	_putstr("$M<");
	SendChar(size);
	checksum ^= size;
	SendChar(cmd);
	checksum ^= cmd;
	while(size--)
	{
			SendChar(*p);
			checksum ^= *p++;
	}
	SendChar(checksum);
}

int request(void)
{
//	send_command(0, MSP_IDENT, NULL);	
//	send_command(0, MSP_STATUS, NULL);	
	send_command(0, MSP_COMP_GPS, NULL);
	send_command(0, MSP_ATTITUDE, NULL);	
	send_command(0, MSP_ALTITUDE, NULL);
	
	return 3;
}
