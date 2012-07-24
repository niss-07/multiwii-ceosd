#include <stdio.h>

#include "main.h"
#include "SPI_MAX7456.h"

#define VIDEO_VOLTAGE		12*30 + 22
#define MAIN_VOLTAGE 		10*30
#define MAIN_CURRENT		11*30
#define MAIN_CONSUMED		12*30
#define RSSI_VOLTAGE		1*30

static uint16_t calibrate_c = 0xFFFF;
static uint32_t consumed_current = 0;
int32_t	m_alt;
int16_t m_pitch, m_roll;
int16_t m_distance_home, m_direction_home;

void print_video_voltage()
{
		uint32_t v;
	
		v = (VideoAvgValue * 89)/ 100;
		_sprintf(&screen[VIDEO_VOLTAGE], "%2u.%u\x8e", v / 100, (v%100)/10);
}

void print_main_voltage()
{
		uint32_t v, b;
	
		v = (MainAvgValue * 89)/ 100;
		/* 9.6 - min, 12.6 - max */
		if(v >= 960)
			b = v;
		else
			b = 960;
		
		b -= (960 - 40);
		b /= 60;
		b += 0xb4;
	
		_sprintf(&screen[MAIN_VOLTAGE], "%c%2u.%u\x8e", b, v / 100, (v%100)/10);
}

void print_main_current()
{
		uint32_t c, t;
	
		c = CurrentAvgValue;
		if (c < calibrate_c)
			calibrate_c = c;
		c = (c - calibrate_c) / 2;
		t = (Ticks(0) - 1) * c;
		consumed_current += t;
		
		_sprintf(&screen[MAIN_CURRENT], "%2u.%u\x8f", c/10, c%10);
}

void print_main_consumed()
{
		_sprintf(&screen[MAIN_CONSUMED], "%05u", consumed_current/36000);
}

void print_rssi()
{
		_sprintf(&screen[RSSI_VOLTAGE], "\x12%3u%%", RSSIAvgValue * 25 / 1000);
}

void print_version(int version)
{
		_sprintf(&screen[RSSI_VOLTAGE + 12], "v: %u%", version);
}

void print_status()
{
	
}

void print_gps()
{
		int c;
		m_direction_home = m_direction_home % 360;
		c = m_direction_home * 10 + 112;
		c = 2 * (c / 225);
		c = c % 32;
		_sprintf(&screen[11*30 + 13], "%c%c", c + 0x90, c + 0x91 );
		_sprintf(&screen[11*30 + 16], "\x1f%5i", m_distance_home );		
}

void print_attitude_part(int X,int Y,int roll)
{
	int xx;

	X=X*(0.6)+28;

	if(X>56)
		X=56;
	if(X<0)
		X=0;

	xx=X/8;    
	switch (xx) 
	{
		case 0:
					screen[(roll*30)+100+Y] = 13 - (X);
		break;
		case 1:
					screen[(roll*30)+130+Y] = 13 - (X-8);
		break;
		case 2:
					screen[(roll*30)+160+Y] = 13 - (X-16);
		break;
		case 3:
					screen[(roll*30)+190+Y] = 13 - (X-24);
		break;
		case 4:
					screen[(roll*30)+220+Y] = 13 - (X-32);
		break;
		case 5:
					screen[(roll*30)+250+Y] = 13 - (X-40);
		break;
		case 6:
					screen[(roll*30)+280+Y] = 13 - (X-48);
		break;
	}
}  

void print_attitude()
{
	int16_t roll, pitch;

	roll = m_roll / 20;
  pitch = m_pitch /8;
  if(pitch>25) pitch=25;
  if(pitch<-25) pitch=-25;
  if(roll>40) roll=40;
  if(roll<-40) roll=-40;
  pitch = pitch /5;
    
	_sprintf(&screen[RSSI_VOLTAGE + 12], "\xb1%4i", pitch );
	
	_sprintf(&screen[7*30 + 22], "\xb2%4i", roll );
	
  print_attitude_part(roll,0,pitch );
  print_attitude_part(roll*0.75,1,pitch );
  print_attitude_part(roll*0.5,2,pitch );
  print_attitude_part(roll*0.25,3,pitch );
  print_attitude_part(0,4,pitch );
  print_attitude_part(-1*roll*0.25,5,pitch );
  print_attitude_part(-1*roll*0.5,6,pitch );
  print_attitude_part(-1*roll*0.75,7,pitch );
  print_attitude_part(-1*roll,8,pitch ); 
}

void print_altitude()
{
	_sprintf(&screen[12*30 + 13], "\x85%4i", m_alt/100);
}
