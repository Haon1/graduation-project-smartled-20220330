#include <stm32f4xx.h>
#include "sys.h"
#include "rgb_led.h"
#include "includes.h"

/*
led1   tim1  r:PE11 g:PE13   b:PE14  通道234
led2   tim4  r:PD12 g:PD13   b:PD14  通道123
*/



void rgb_led_init(void)
{
	tim1_init();		//pwm
	tim4_init();		//pwm
}


void tim1_init(void)
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  		TIM_OCInitStructure;
	
	//E端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//时钟使能	TIM14 是APB1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
	
	
	//配置PE  11 13 PE14引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11|GPIO_Pin_13|GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOE, &GPIO_InitStructure); 
	
	//把PE  11  13 PE14复用为TIM1
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource11, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource13, GPIO_AF_TIM1);
	GPIO_PinAFConfig(GPIOE, GPIO_PinSource14, GPIO_AF_TIM1);
	
	//时钟源频率为1KHz,10000次计数，中断频率为1Hz(PWM输出频率)
	//1000次计数，中断频率为10Hz
	//100次计数，中断频率为100Hz
	TIM_TimeBaseStructure.TIM_Period = 99;		//计数值 99 是100HZ  10000/(99+1)
	//TIM_TimeBaseStructure.TIM_Period = 49;		//计数值 49次次是200HZ  10000/(49+1)
	TIM_TimeBaseStructure.TIM_Prescaler = 16799;		//分频值 168000000/(16799+1)=10000Hz,10000次计数是1S
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//再分频,F407不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	
	/* 配置TIM1 的输出PWM通道2  通道几要看原理图*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	//PWM模#式1		计数值<比较值 ，通道有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 100;					//比较值	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//通道有效时为高电平
	TIM_OC2Init(TIM1, &TIM_OCInitStructure);

	/* 配置TIM1 的输出PWM通道3  通道几要看原理图*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	//PWM模#式1		计数值<比较值 ，通道有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 100;					//比较值	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//通道有效时为高电平
	TIM_OC3Init(TIM1, &TIM_OCInitStructure);
	
	/* 配置TIM1 的输出PWM通道4  通道几要看原理图*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	//PWM模#式1		计数值<比较值 ，通道有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 100;					//比较值	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//通道有效时为高电平
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);

	
	TIM_Cmd(TIM1, ENABLE);		//使能TIM1工作	
	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);	//TIM1特殊之处，要加这一句才工作
}


void tim4_init(void)
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  		TIM_OCInitStructure;
	//E端口时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	
	//时钟使能	TIM4
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	
	
	//配置PD12 pd13 Pd14引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13|GPIO_Pin_14 ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	//把Pd12 pd13 Pd14复用为TIM4
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource13, GPIO_AF_TIM4);
	GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_TIM4);
	
	//时钟源频率为1KHz,10000次计数，中断频率为1Hz(PWM输出频率)
	//1000次计数，中断频率为10Hz
	//100次计数，中断频率为100Hz
	TIM_TimeBaseStructure.TIM_Period = 99;		//计数值 99 是100HZ  10000/(99+1)
	//TIM_TimeBaseStructure.TIM_Period = 49;		//计数值 49次次是200HZ  10000/(49+1)
	TIM_TimeBaseStructure.TIM_Prescaler = 8399;		//分频值 168000000/(9399+1)=10000Hz,10000次计数是1S
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;	//再分频,F407不支持
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//向上计数
	
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	

	/* 配置TIM4 的输出PWM通道1  通道几要看原理图*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	//PWM模#式1		计数值<比较值 ，通道有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 100;					//比较值	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//通道有效时为高电平
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	
	/* 配置TIM4 的输出PWM通道2  通道几要看原理图*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	//PWM模#式1		计数值<比较值 ，通道有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 100;					//比较值	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//通道有效时为高电平
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	
	/* 配置TIM4 的输出PWM通道3  通道几要看原理图*/
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;	//PWM模#式1		计数值<比较值 ，通道有效
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = 100;					//比较值	
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;		//通道有效时为高电平
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);

	
	TIM_Cmd(TIM4, ENABLE);		//使能TIM4工作	
}


void rgb_led1_crl(int color,int level)
{
	if(color == red)	//红光
	{
		TIM_SetCompare2(TIM1, 100-level);	//设置TIM4通道1的比较值
		TIM_SetCompare3(TIM1, 100);	//设置TIM4通道2的比较值
		TIM_SetCompare4(TIM1, 100);	//设置TIM4通道3的比较值
	}
	else if(color == green)	//绿色
	{
		TIM_SetCompare2(TIM1, 100);	//设置TIM4通道1的比较值
		TIM_SetCompare3(TIM1, 100-level);	//设置TIM4通道2的比较值
		TIM_SetCompare4(TIM1, 100);	//设置TIM4通道3的比较值
	}
	else if(color == blue)	//蓝色
	{
		TIM_SetCompare2(TIM1, 100);	//设置TIM4通道1的比较值
		TIM_SetCompare3(TIM1, 100);	//设置TIM4通道2的比较值
		TIM_SetCompare4(TIM1, 100-level);	//设置TIM4通道3的比较值
	}
	else if(color == yellow)	//黄色
	{
		TIM_SetCompare2(TIM1, 100-level);	//设置TIM4通道1的比较值
		TIM_SetCompare3(TIM1, 100-level);	//设置TIM4通道2的比较值
		TIM_SetCompare4(TIM1, 100);	//设置TIM4通道3的比较值
	}
	else if(color == purple)	//紫色
	{
		TIM_SetCompare2(TIM1, 100-level);	//设置TIM4通道1的比较值
		TIM_SetCompare3(TIM1, 100);	//设置TIM4通道2的比较值
		TIM_SetCompare4(TIM1, 100-level);	//设置TIM4通道3的比较值
	}
	else if(color == cyan)	//浅蓝色
	{
		TIM_SetCompare2(TIM1, 100);	//设置TIM4通道1的比较值
		TIM_SetCompare3(TIM1, 100-level);	//设置TIM4通道2的比较值
		TIM_SetCompare4(TIM1, 100-level);	//设置TIM4通道3的比较值
	}
	else if(color == white)		//白色
	{
		TIM_SetCompare2(TIM1, 100-level);	//设置TIM4通道1的比较值
		TIM_SetCompare3(TIM1, 100-level);	//设置TIM4通道2的比较值
		TIM_SetCompare4(TIM1, 100-level);	//设置TIM4通道3的比较值
	}
	else if(color == black)		//灭
	{
		TIM_SetCompare2(TIM1, 100);	//设置TIM4通道1的比较值
		TIM_SetCompare3(TIM1, 100);	//设置TIM4通道2的比较值
		TIM_SetCompare4(TIM1, 100);	//设置TIM4通道3的比较值
	}
	
}


void rgb_led2_crl(int color,int level)
{
	if(color == red)	//红光
	{
		TIM_SetCompare1(TIM4, 100-level);	//设置TIM4通道1的比较值
		TIM_SetCompare2(TIM4, 100);	//设置TIM4通道2的比较值
		TIM_SetCompare3(TIM4, 100);	//设置TIM4通道3的比较值
	}
	else if(color == green)	//绿色
	{
		TIM_SetCompare1(TIM4, 100);	//设置TIM4通道1的比较值
		TIM_SetCompare2(TIM4, 100-level);	//设置TIM4通道2的比较值
		TIM_SetCompare3(TIM4, 100);	//设置TIM4通道3的比较值
	}
	else if(color == blue)	//蓝色
	{
		TIM_SetCompare1(TIM4, 100);	//设置TIM4通道1的比较值
		TIM_SetCompare2(TIM4, 100);	//设置TIM4通道2的比较值
		TIM_SetCompare3(TIM4, 100-level);	//设置TIM4通道3的比较值
	}
	else if(color == yellow)	//黄色
	{
		TIM_SetCompare1(TIM4, 100-level);	//设置TIM4通道1的比较值
		TIM_SetCompare2(TIM4, 100-level);	//设置TIM4通道2的比较值
		TIM_SetCompare3(TIM4, 100);	//设置TIM4通道3的比较值
	}
	else if(color == purple)	//紫色
	{
		TIM_SetCompare1(TIM4, 100-level);	//设置TIM4通道1的比较值
		TIM_SetCompare2(TIM4, 100);	//设置TIM4通道2的比较值
		TIM_SetCompare3(TIM4, 100-level);	//设置TIM4通道3的比较值
	}
	else if(color == cyan)	//浅蓝色
	{
		TIM_SetCompare1(TIM4, 100);	//设置TIM4通道1的比较值
		TIM_SetCompare2(TIM4, 100-level);	//设置TIM4通道2的比较值
		TIM_SetCompare3(TIM4, 100-level);	//设置TIM4通道3的比较值
	}
	else if(color == white)		//白色
	{
		TIM_SetCompare1(TIM4, 100-level);	//设置TIM4通道1的比较值
		TIM_SetCompare2(TIM4, 100-level);	//设置TIM4通道2的比较值
		TIM_SetCompare3(TIM4, 100-level);	//设置TIM4通道3的比较值
	}
	else if(color == black)		//灭
	{
		TIM_SetCompare1(TIM4, 100);	//设置TIM4通道1的比较值
		TIM_SetCompare2(TIM4, 100);	//设置TIM4通道2的比较值
		TIM_SetCompare3(TIM4, 100);	//设置TIM4通道3的比较值
	}
	
}

extern int level_led1;		//在main.c中定义
extern int color_led1;
extern int level_led2;		//在main.c中定义
extern int color_led2;
static OS_ERR 		err;

void led1_color_right(void)
{
	//等待互斥锁 ，避免其他任务同时修改  led的颜色 或者亮度
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	if(color_led1!=7)
		color_led1++;
	if(color_led1==7)
		color_led1 = 0;
	//释放互斥锁,等待锁的任务（最高优先级且就绪）得到立即执行
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}
void led2_color_right(void)
{
	//等待互斥锁 ，避免其他任务同时修改  led的颜色 或者亮度
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	if(color_led2!=7)
		color_led2++;
	if(color_led2==7)
		color_led2 = 0;
	//释放互斥锁,等待锁的任务（最高优先级且就绪）得到立即执行
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led1_color_left(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	color_led1--;
	if(color_led1==-1)
		color_led1 = 6;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led2_color_left(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	color_led2--;
	if(color_led2==-1)
		color_led2 = 6;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led1_level_up(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	if(level_led1<=90)
		level_led1 += 10;
	else if(level_led1 >90 && level_led1 <100)
		level_led1 += 1;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led2_level_up(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	if(level_led2<=90)
		level_led2 += 10;
	else if(level_led2 >90 && level_led2 <100)
		level_led2 += 1;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led1_level_down(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	if(level_led1 >= 10)
		level_led1 -= 10;
	else if(level_led1 >0 && level_led1 <10)
		level_led1 -= 1;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led2_level_down(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	if(level_led2 >= 10)
		level_led2 -= 10;
	else if(level_led2 >0 && level_led2 <10)
		level_led2 -= 1;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led1_level_min(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	level_led1 = 0;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led2_level_min(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	level_led2 = 0;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led1_level_max(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	level_led1 = 100;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

void led2_level_max(void)
{
	OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
	level_led2 = 100;
	OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
}

//给LED灯发送消息
void send_message_to_led(void)
{
	OS_ERR err;
	//发送消息给led1任务
	OSQPost(&g_queue_led1,(void *)"1",1,OS_OPT_POST_FIFO,&err);
	//发送消息给led2任务
	OSQPost(&g_queue_led2,(void *)"1",1,OS_OPT_POST_FIFO,&err);
}
