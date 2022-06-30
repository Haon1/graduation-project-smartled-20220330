#include <stm32f4xx.h>
#include "sys.h"
#include "rgb_led.h"
#include "delay.h"
#include "includes.h"

/*
����
		c1  PD5
		c2  PD0
		c3  PE7
		c4  PE9
		
		r1  PD15
		r2  PD1
		r3  PE8
		r4  PE10
*/


//������̳�ʼ��
void key_board_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
    
    //ʹ�ܶ˿�D��Ӳ��ʱ�ӣ����ǶԶ˿�D����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    //ʹ�ܶ˿�E��Ӳ��ʱ�ӣ����ǶԶ˿�E����
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    
    //PD14 PD0 PE7 PE9      //Pd15 Pd1 PE8 PE9
	//ʹ��GPIO_Init����������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;        //���ģʽ
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;       //�������ģʽ,Ĭ�ϵ�
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_0; //ָ����0 5������
	GPIO_InitStructure.GPIO_Speed=GPIO_High_Speed;     //���٣����ǹ��������
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;  //���������������ʹ���������裩
	GPIO_Init(GPIOD,&GPIO_InitStructure);              //D��
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7|GPIO_Pin_9; //ָ����7 9 ������
    GPIO_Init(GPIOE,&GPIO_InitStructure);              //E��

	
    
	/* �����������ģʽ����ʹ���ڲ��������裬�������յ�ʱ��Ϊ�̶��ĸߵ�ƽ */
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;	   				//����
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;         				//����ģʽ
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_15 | GPIO_Pin_1;            //ָ���� 1 15������
    GPIO_Init(GPIOD,&GPIO_InitStructure);              //D��
    
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_10;            //ָ���� 8  10������
    GPIO_Init(GPIOE,&GPIO_InitStructure);              //E��
    
}

//����ɨ��
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

//�������¼�  ����1��Ҫ�ϱ����ݣ�����0����Ҫ
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
