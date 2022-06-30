#include "paj7620u2.h"
#include "paj7620u2_cfg.h"
#include "delay.h"
#include "rgb_led.h"
#include "includes.h"
#include <stdio.h>


extern void esp8266_commit_led_status();


//选择PAJ7620U2 BANK区域
void paj7620u2_selectBank(bank_e bank)
{
	switch(bank)
	{
		case BANK0: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK0);break;//BANK0寄存器区域
		case BANK1: GS_Write_Byte(PAJ_REGITER_BANK_SEL,PAJ_BANK1);break;//BANK1寄存器区域
	}		
}

//PAJ7620U2唤醒
uint8_t paj7620u2_wakeup(void)
{ 
	uint8_t data=0x0a;
	GS_WakeUp();//唤醒PAJ7620U2
	delay_ms(5);//唤醒时间>400us
	GS_WakeUp();//唤醒PAJ7620U2
	delay_ms(5);//唤醒时间>400us
	paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
	data = GS_Read_Byte(0x00);//读取状态
	if(data!=0x20) return 0; //唤醒失败
	
	return 1;
}

//PAJ7620U2初始化
//返回值：0:失败 1:成功
uint8_t paj7620u2_init(void)
{
	uint8_t i;
	uint8_t status;
	
	GS_i2c_init();//IIC初始化
    status = paj7620u2_wakeup();//唤醒PAJ7620U2
	if(!status) 
		return 0;
	paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
	for(i=0;i<INIT_SIZE;i++)
	{
		GS_Write_Byte(init_Array[i][0],init_Array[i][1]);//初始化PAJ7620U2
	}
    paj7620u2_selectBank(BANK0);//切换回BANK0寄存器区域
	
	return 1;
}



//手势识别测试
void Gesture_test(void)
{
	uint8_t i;
    uint8_t status;

	uint8_t data[2]={0x00};
	uint16_t gesture_data;
	
	OS_ERR 		err;
	OS_FLAGS  	flags=0;
	
	
	paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
	
	for(i=0;i<GESTURE_SIZE;i++)
	{
		GS_Write_Byte(gesture_arry[i][0],gesture_arry[i][1]);//手势识别模式初始化
	}
	
	paj7620u2_selectBank(BANK0);//切换回BANK0寄存器区域
	
	i=0;
	while(1)
	{
		//等待事件标志		如果为1就读取，否则让出cpu使用权
		flags = OSFlagPend(&g_flag_grp,FLAG_GRP_OPEN_PAJ7620,0,OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_BLOCKING,NULL,&err);
		//printf("paj7620u2  open!!\r\n");
		
		//读取手势状态			
        status = GS_Read_nByte(PAJ_GET_INT_FLAG1,2,&data[0]);
		
		if(!status)
		{   
			gesture_data =(uint16_t)data[1]<<8 | data[0];
			//printf("gesture_data is [%d]\r\n",gesture_data);
			if(gesture_data) 
			{
				switch(gesture_data)
				{
					case GES_UP:               
					                           printf("Up\r\n");
											   led1_level_up(); 	led2_level_up();
						 break; //向上
					case GES_DOWM:                  
											   printf("Dowm\r\n"); 
											   led1_level_down();	led2_level_down();
											  					
						 break; //向下
					case GES_LEFT:                       
  						                       printf("Left\r\n"); 
											   led1_color_left();	led2_color_left();
						 break; //向左
					case GES_RIGHT:               
                						       printf("Right\r\n");
											   led1_color_right();	led2_color_right();
						 break; //向右
					case GES_FORWARD:                 
						                       printf("Forward\r\n");             break; //向前
					case GES_BACKWARD:            
            						           printf("Backward\r\n");            break; //向后
					case GES_CLOCKWISE:             
                						       printf("Clockwise\r\n"); 
											   led1_level_max();	led2_level_max();
						 break; //顺时针
					case GES_COUNT_CLOCKWISE:   
                   						       printf("AntiClockwise\r\n"); 
											   led1_level_min();	led2_level_min();
						 break; //逆时针
					case GES_WAVE:                 
						                       printf("Wave\r\n");                break; //挥动
					default:   break;
					
				}
				//led颜色改变之后，上报状态给上位机
				if(gesture_data <=128 ){
					send_message_to_led();		//通知灯改变
					esp8266_commit_led_status();	//上报状态
					//printf("gesture_data = %d\r\n",gesture_data);
				}
				
			}
				
		}

		delay_ms(50);
	}
}

//接近检测测试
void Ps_test(void)
{
	uint8_t i;

	uint8_t data[2]={0x00};
	uint8_t obj_brightness=0;
	uint16_t obj_size=0;
	
	paj7620u2_selectBank(BANK0);//进入BANK0寄存器区域
	
	for(i=0;i<PROXIM_SIZE;i++)
	{
		GS_Write_Byte(proximity_arry[i][0],proximity_arry[i][1]);//接近检测模式初始化
	}
	
	paj7620u2_selectBank(BANK0);//切换回BANK0寄存器区域
	
	i=0;

	while(1)
	{	

		obj_brightness = GS_Read_Byte(PAJ_GET_OBJECT_BRIGHTNESS);//读取物体亮度
		data[0] = GS_Read_Byte(PAJ_GET_OBJECT_SIZE_1);//读取物体大小
		data[1] = GS_Read_Byte(PAJ_GET_OBJECT_SIZE_2);
		obj_size = ((uint16_t)data[1] & 0x0f)<<8 | data[0];
		printf("obj_brightness: %d\r\n",obj_brightness);
        printf("obj_size: %d\r\n",obj_size);
		
		delay_ms(100);
		
		i++;
	}	
}

