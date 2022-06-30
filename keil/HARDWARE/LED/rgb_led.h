#ifndef __LED_H__
#define __LED_H__

typedef enum
{
	red		= 0x00,		//红
	yellow	= 0x01,		//黄
	blue	= 0x02,		//蓝
	cyan	= 0x03,		//浅蓝
	green	= 0x04,		//绿
	white	= 0x05,		//白
	purple	= 0x06,		//紫
	black	= 0x07		//黑（全灭）
}color_type;



void rgb_led_init(void);
void rgb_led1_crl(int color,int level);
void rgb_led2_crl(int color,int level);

void tim1_init(void);
void tim4_init(void);


void led1_color_right(void);
void led2_color_right(void);

void led1_color_left(void);
void led2_color_left(void);

void led1_level_up(void);
void led2_level_up(void);

void led1_level_down(void);
void led2_level_down(void);

void led1_level_min(void);
void led2_level_min(void);

void led1_level_max(void);
void led2_level_max(void);

void send_message_to_led(void);
#endif
