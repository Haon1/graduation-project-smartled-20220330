#include <stm32f4xx.h>
#include "sys.h"
#include "rgb_led.h"
#include "delay.h"
#include "includes.h"

/*
接线
		c1  PD5
		c2  PD0
		c3  PE7
		c4  PE9
		
		r1  PD15
		r2  PD1
		r3  PE8
		r4  PE10
*/


//矩阵键盘初始化
void key_board_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    
    //使能端口D的硬件时钟，就是对端口D供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    //使能端口E的硬件时钟，就是对端口E供电
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
    //PD14 PD0 PE7 PE9      //Pd15 Pd1 PE8 PE9
	//使用GPIO_Init来配置引脚
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;        //输出模式
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;       //推挽输出模式,默认的
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_0; //指定第0 5根引脚
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;     //高速，但是功耗是最高
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  //无需上下拉（亦可使能下拉电阻）
	GPIO_Init(GPIOD,&GPIO_InitStructure);              //D口
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_9; //指定第7 9 根引脚
    GPIO_Init(GPIOE,&GPIO_InitStructure);              //E口

	
    
	/* 矩阵键盘输入模式必须使能内部上拉电阻，引脚悬空的时候为固定的高电平 */
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	   				//上拉
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;         				//输入模式
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15 | GPIO_Pin_1;            //指定第 1 15根引脚
    GPIO_Init(GPIOD,&GPIO_InitStructure);              //D口
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_10;            //指定第 8  10根引脚
    GPIO_Init(GPIOE,&GPIO_InitStructure);              //E口
    
}

//按键扫描
char get_key_board(void)
{
    //PD5 PD0 PE7 PE9     
    PDout(5) = 0;
    PDout(0) = 1;
    PEout(7) = 1;
    PEout(9) = 1;
    delay_ms(2);
	
    //PD15 PD1 PE8  PE10
    if( PDin(15) == 0 ) return '1';
    else if( PDin(1) == 0 ) return '2';
    else if( PEin(8) == 0 ) return '3';
    else if( PEin(10) == 0 ) return 'A';
    
	PDout(5) = 1;
    PDout(0) = 0;
    PEout(7) = 1;
    PEout(9) = 1;
    delay_ms(2);

    //PD15 PD1 PE8  PE10
    if( PDin(15) == 0 ) return '4';
    else if( PDin(1) == 0 ) return '5';
    else if( PEin(8) == 0 ) return '6';
    else if( PEin(10) == 0 ) return 'B';
    
    PDout(5) = 1;
    PDout(0) = 1;
    PEout(7) = 0;
    PEout(9) = 1;
    delay_ms(2);
	
    //PD15 PD1 PE8  PE10
    if( PDin(15) == 0 ) return '7';
    else if( PDin(1) == 0 ) return '8';
    else if( PEin(8) == 0 ) return '9';
    else if( PEin(10) == 0 ) return 'C';
    
    PDout(5) = 1;
    PDout(0) = 1;
    PEout(7) = 1;
    PEout(9) = 0;
    delay_ms(2);
	
    //PD15 PD1 PE8  PE10
    if( PDin(15) == 0 ) return '*';
    else if( PDin(1) == 0 ) return '0';
    else if( PEin(8) == 0 ) return '#';
    else if( PEin(10) == 0 ) return 'D';
    
    return 'N';
}

//处理按键事件  返回1需要上报数据，返回0不需要
int deal_key_code(char keycode)
{
	OS_ERR err;
	int rt=1;
	if(keycode=='N')
		return ;
	if(keycode == '1')		
	{
		led1_color_left();
	}
	else if(keycode=='2')
	{
		led2_color_left();
	}
	else if(keycode=='3')
	{
		OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_ADC_CTRL,OS_OPT_POST_FLAG_SET,&err);
		rt = 0;
	}
	else if(keycode=='A')
	{
		
	}
	else if(keycode=='4')
	{
		led1_color_right();
	}
	else if(keycode=='5')
	{
		led2_color_right();
	}
	else if(keycode=='6')
	{
		OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_ADC_CTRL,OS_OPT_POST_FLAG_CLR,&err);
		rt = 0;
	}
	else if(keycode=='B')
	{
	}
	else if(keycode=='7')
	{
		led1_level_down();
	}
	else if(keycode=='8')
	{
		led2_level_down();
	}
	else if(keycode=='9')
	{
		OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_PAJ7620,OS_OPT_POST_FLAG_SET,&err);
		rt = 0;
	}
	else if(keycode=='C')
	{
	}
	else if(keycode=='*')
	{
		led1_level_up();
	}
	else if(keycode=='0')
	{
		led2_level_up();
	}
	else if(keycode=='#')
	{
	}
	else if(keycode=='D')
	{
		OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_PAJ7620,OS_OPT_POST_FLAG_CLR,&err);
		rt = 0;
	}
	
	return rt;
}
