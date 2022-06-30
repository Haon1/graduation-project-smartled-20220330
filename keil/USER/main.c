#include "sys.h"
#include "delay.h"
#include "usart.h"
//#include "led.h"
#include "adc.h"
#include "key.h"
#include "rgb_led.h"
#include "esp8266.h"
#include "paj7620u2.h"
#include "includes.h"


int level_led1=0;		//LED1 当亮度
int color_led1=7;		//LED1 当前颜色

int level_led2=0;		//LED2 当亮度
int color_led2=7;		//LED2 当前颜色


//任务init控制块
OS_TCB app_task_tcb_init;
void app_task_init(void *parg);
CPU_STK app_task_stk_init[512];			//任务堆栈，大小为512字，也就是512字 2048字节


//任务esp8266连接服务器控制块
OS_TCB app_task_tcb_esp8266_connect_server;
void app_task_esp8266_connect_server(void *parg);
CPU_STK app_task_stk_esp8266_connect_server[512];			//任务堆栈，大小为128字  512字节

//任务led1控制块
OS_TCB app_task_tcb_led1;
void app_task_led1(void *parg);
CPU_STK app_task_stk_led1[512];			//任务堆栈，大小为512字  2048字节

//任务led2控制块
OS_TCB app_task_tcb_led2;
void app_task_led2(void *parg);
CPU_STK app_task_stk_led2[512];			//任务堆栈，大小为512字  2048字节

//任务手势控制块
OS_TCB app_task_tcb_paj7620;
void app_task_paj7620(void *parg);
CPU_STK app_task_stk_paj7620[512];			//的任务堆栈，大小为512字  2048字节

//任务adc控制块
OS_TCB app_task_tcb_adc;
void app_task_adc(void *parg);
CPU_STK app_task_stk_adc[512];			//任务堆栈，大小为512字  2048字节

//任务key控制块
OS_TCB app_task_tcb_key;
void app_task_key(void *parg);
CPU_STK app_task_stk_key[512];			//任务堆栈，大小为512字  2048字节

//任务消息处理控制块
OS_TCB app_task_tcb_msg;
void app_task_msg(void *parg);
CPU_STK app_task_stk_msg[512];			//任务堆栈，大小为512字  2048字节


//内核对象 
OS_Q   					g_queue_msg;			//消息队列的对象
OS_Q   					g_queue_led1;			//消息队列的对象
OS_Q   					g_queue_led2;			//消息队列的对象
OS_Q   					g_queue_connect;		//消息队列的对象


OS_MUTEX				g_mutex_printf;			//互斥锁的对象
OS_MUTEX				g_mutex_led_value;		//互斥锁的对象

OS_FLAG_GRP   			g_flag_grp;				//事件标志组	


#define DEBUG_PRINTF_EN	1
void dgb_printf_safe(const char *format, ...)
{
#if DEBUG_PRINTF_EN	
	OS_ERR err;
	
	va_list args;
	va_start(args, format);
	
	OSMutexPend(&g_mutex_printf,0,OS_OPT_PEND_BLOCKING,NULL,&err);	
	vprintf(format, args);
	OSMutexPost(&g_mutex_printf,OS_OPT_POST_NONE,&err);
	
	va_end(args);
#else
	(void)0;
#endif
}


//主函数
int main(void)
{
	OS_ERR err;

	systick_init();  													//时钟初始化
	usart_init(115200);  												//串口初始化
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//中断分组配置

	//OS初始化，它是第一个运行的函数,初始化各种的全局变量，例如中断嵌套计数器、优先级、存储器
	OSInit(&err);


	//创建初始化任务
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_init,									//任务控制块，等同于线程id
					(CPU_CHAR *)"app_task_init",									//任务的名字，名字可以自定义的
					(OS_TASK_PTR)app_task_init,										//任务函数，等同于线程函数
					(void *)0,												//传递参数，等同于线程的传递参数
					(OS_PRIO)6,											 	//任务的优先级6		
					(CPU_STK *)app_task_stk_init,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
					
	if(err!=OS_ERR_NONE)
	{
		printf("init  fail\r\n");
		
		while(1);
	}

	//启动OS，进行任务调度
	OSStart(&err);
					
					
	while(1);
	
}

//初始化硬件任务
void app_task_init(void *parg)
{
	OS_ERR err;

	

	printf("task1 init  create ok\r\n");
	
	rgb_led_init();		//全彩LED初始化	
	LED_Init();     	//系统LED初始化	 
	bt24_init(9600);	//蓝牙模块初始化
	key_board_init();	//按键矩阵初始化
	
	adc_init();		//adc初始化
	printf("ADC OK\r\n");
	 
	//PAJ7620U2传感器初始化
 	while(!paj7620u2_init())
	{
	    printf("PAJ7620U2 Error!!!\r\n");
		delay_ms(500);
	}
    printf("PAJ7620U2 OK\r\n");
	
	//esp8266初始化
	esp8266_init();
	printf("Esp8266 init OK\r\n");
	
	
	//创建消息队列，支持16条消息（实际上能够存储16个指针）
	OSQCreate(&g_queue_connect,"g_queue_connect",16,&err);
	OSQCreate(&g_queue_msg,"g_queue_msg",16,&err);
	OSQCreate(&g_queue_led1,"g_queue_led1",16,&err);
	OSQCreate(&g_queue_led2,"g_queue_led2",16,&err);
	
		
	//创建互斥锁
	OSMutexCreate(&g_mutex_printf,	"g_mutex_printf",&err);
	OSMutexCreate(&g_mutex_led_value,	"g_mutex_led_value",&err);	
	
	//创建事件标志组
	OSFlagCreate(&g_flag_grp,"g_flag_grp",0,&err);

	//创建连接服务器任务
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_esp8266_connect_server,			//任务控制块
					(CPU_CHAR *)"app_task_esp8266_connect_server",			//任务的名字
					(OS_TASK_PTR)app_task_esp8266_connect_server,			//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)6,											 	//任务的优先级6		
					(CPU_STK *)app_task_stk_esp8266_connect_server,			//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
	
	//创建任务led1
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_led1,									//任务控制块
					(CPU_CHAR *)"app_task_led1",									//任务的名字
					(OS_TASK_PTR)app_task_led1,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)7,											 	//任务的优先级7		
					(CPU_STK *)app_task_stk_led1,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
					
	//创建任务led2
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_led2,									//任务控制块
					(CPU_CHAR *)"app_task_led2",									//任务的名字
					(OS_TASK_PTR)app_task_led2,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)7,											 	//任务的优先级7		
					(CPU_STK *)app_task_stk_led2,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
					
	//创建任务手势
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_paj7620,									//任务控制块
					(CPU_CHAR *)"app_task_paj7620",									//任务的名字
					(OS_TASK_PTR)app_task_paj7620,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)7,											 	//任务的优先级7		
					(CPU_STK *)app_task_stk_paj7620,						//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
	//创建任务按键
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_key,									//任务控制块
					(CPU_CHAR *)"app_task_key",									//任务的名字
					(OS_TASK_PTR)app_task_key,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)7,											 	//任务的优先级7		
					(CPU_STK *)app_task_stk_key,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
					
	//创建任务adc
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_adc,									//任务控制块
					(CPU_CHAR *)"app_task_adc",									//任务的名字
					(OS_TASK_PTR)app_task_adc,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)7,											 	//任务的优先级7		
					(CPU_STK *)app_task_stk_adc,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
					
	//创建任务消息处理
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_msg,									//任务控制块
					(CPU_CHAR *)"app_task_msg",									//任务的名字
					(OS_TASK_PTR)app_task_msg,										//任务函数
					(void *)0,												//传递参数
					(OS_PRIO)7,											 	//任务的优先级7		
					(CPU_STK *)app_task_stk_msg,									//任务堆栈基地址
					(CPU_STK_SIZE)512/10,									//任务堆栈深度限位，用到这个位置，任务不能再继续使用
					(CPU_STK_SIZE)512,										//任务堆栈大小			
					(OS_MSG_QTY)0,											//禁止任务消息队列
					(OS_TICK)0,												//默认时间片长度																
					(void  *)0,												//不需要补充用户存储区
					(OS_OPT)OS_OPT_TASK_NONE,								//没有任何选项
					&err													//返回的错误码
				);
					
	
	//删除自身任务，进入休眠态
	OSTaskDel(NULL,&err);
}


//esp8266连接服务器任务
void app_task_esp8266_connect_server(void *parg)
{
	int rt;
	OS_ERR err;
	OS_MSG_SIZE msg_size=0;//保存接收到消息的大小
	
	char *p = NULL;
	char *tmp = NULL;
	char msg[128]={0};
	char server_ip[16]={0};		//服务器ip
	uint16_t server_port;		//服务器端口
	
	dgb_printf_safe("task esp8266 connect server is create ok\r\n");

	while(1)
	{	
		dgb_printf_safe("get_server_infomation\r\n");
		//等待消息
		p = OSQPend(&g_queue_connect,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		if(strstr(p,"ip")&&strstr(p,"port")&&strstr(p,"#"))
		{
			//消息格式 ip:192.168.2.3,port:8026#	前面为ip,后面端口
			strncpy(msg,p,msg_size);
			tmp = strtok(msg,",");				//以 ","切割成两份 ip:xxx.xxx.xxx.xxx  port:xxxx
			strcpy(server_ip,tmp+3);			//获取ip
			tmp = strtok(NULL,",");
			server_port = atoi(tmp+5);			//获取端口
			
			printf("ip is %s, port is %d\r\n",server_ip,server_port);
			
			//连接qt服务器
			rt =esp8266_connect_server("TCP",server_ip,server_port);
			if(rt == 0)
				printf("esp8266_connect_server success\r\n");
			else
				printf("esp8266_connect_server fail\r\n");
			
			delay_ms(2000);
			
			//进入透传模式
			rt =esp8266_entry_transparent_transmission();
			if(rt == 0)
				printf("esp8266_entry_transparent_transmission success\r\n");
			else
				printf("esp8266_entry_transparent_transmission fail\r\n");
			
			esp8266_commit_led_status();	//上报led当前状态
			
			break;
		}
	}
	
	printf("connect task exit\r\n");
	memset(p,0,msg_size);
	//删除自身任务，进入休眠态
	OSTaskDel(NULL,&err);
}


//led1任务
void app_task_led1(void *parg)
{
	OS_FLAGS  flags=0;
	OS_ERR err;
	OS_MSG_SIZE msg_size=0;//保存接收到消息的大小
	
	char *p = NULL;
	
	
	printf("task led  1 is create ok\r\n");

	while(1)
	{
		//等待消息队列
		//0,一直等待
		//OS_OPT_PEND_BLOCKING，阻塞形式等待；若等待不了消息，则让出CPU使用权给其他任务
		//不需要时间戳（时间标记），不需要获取消息的所花的时间
		p = OSQPend(&g_queue_led1,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		dgb_printf_safe("[ led1 ] color:%d, level:%d\r\n",color_led1,level_led1);
		
		rgb_led1_crl(color_led1,level_led1);
		
		//设置事件标志组的bit0
		OSFlagPost(&g_flag_grp,FLAG_GRP_LED_1_OK,OS_OPT_POST_FLAG_SET,&err);
	}
}

//led 2任务
void app_task_led2(void *parg)
{
	OS_FLAGS  flags=0;
	OS_ERR err;
	OS_MSG_SIZE msg_size=0;//保存接收到消息的大小
	

	char *p = NULL;
	
	dgb_printf_safe("task led 2 is create ok\r\n");

	while(1)
	{
		//等待消息队列
		//0,一直等待
		//OS_OPT_PEND_BLOCKING，阻塞形式等待；若等待不了消息，则让出CPU使用权给其他任务
		//不需要时间戳（时间标记），不需要获取消息的所花的时间
		p = OSQPend(&g_queue_led2,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		dgb_printf_safe("[ led2 ] color:%d, level:%d\r\n",color_led2,level_led2);
		
		rgb_led2_crl(color_led2,level_led2);
		
		//设置事件标志组的bit1
		OSFlagPost(&g_flag_grp,FLAG_GRP_LED_2_OK,OS_OPT_POST_FLAG_SET,&err);
	}
}

//手势识别任务
void app_task_paj7620(void *parg)
{
	
	dgb_printf_safe("task paj is create ok\r\n");

	while(1)
	{
		Gesture_test();
	}
}

//模数转换任务
void app_task_adc(void *parg)
{
	OS_ERR 		err;
	OS_FLAGS  	flags=0;
	
	uint32_t 	adc_val;
	
	
	dgb_printf_safe("task adc is create ok \r\n");
	
	//启动ADC3转换
	ADC_SoftwareStartConv(ADC3);
	delay_ms(1000);

	while(1)
	{
		//等待事件标志		如果为1就读取，否则让出cpu使用权
		flags = OSFlagPend(&g_flag_grp,FLAG_GRP_OPEN_ADC_CTRL,0,OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_BLOCKING,NULL,&err);

		
		//获取滤波算法结果
		adc_val = adc_filer();		// 15 3830
		
		OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		level_led1 = level_led2 = adc_val;
		OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
		
		rgb_led1_crl(color_led1,level_led1);		//设置LED1亮度
		rgb_led2_crl(color_led2,level_led2);		//设置LED2亮度
		
		dgb_printf_safe("adc_val=%d\r\n",adc_val);			//286-3760 = 3473   
		

		esp8266_commit_led_status();	//上报状态
		
		delay_ms(200);
	}
}

//按键识别任务
void app_task_key(void *parg)
{
	uint32_t key_sta=0;
	char key_old=0;
	char key_cur=0;
	int rt=0;
	
	dgb_printf_safe("task key is create ok\r\n");

	while(1)
	{
		/* 使用状态机思想得到按键的状态 */
		switch(key_sta)
		{
			case 0://获取按下的按键
			{
				
				key_cur = get_key_board();	

				if(key_cur != 'N')
				{
					key_old = key_cur;
					key_sta=1;
				}
			}break;
			
			case 1://确认按下的按键
			{
				
				key_cur = get_key_board();	
					
				if((key_cur != 'N') && (key_cur == key_old))
				{
					dgb_printf_safe("KEY %c Down\r\n",key_cur);
					key_sta=2;
					
					rt = deal_key_code(key_cur);			//处理按键事件
					if(rt == 1)
					{
						send_message_to_led();		//通知led
						esp8266_commit_led_status();	//处理完上报
					}
				}			
			}break;
		
			case 2://获取释放的按键
			{
				key_cur = get_key_board();	
					
				if(key_cur == 'N')
				{
					dgb_printf_safe("KEY %c Up\r\n",key_old);
					
					key_sta=0;
					
					key_old =  'N';
				}				
			}break;
			
			default:break;
		}
	}
}

//消息处理任务
void app_task_msg(void *parg)
{
	OS_ERR err;
	OS_FLAGS  flags=0;
	OS_MSG_SIZE msg_size=0;//保存接收到消息的大小
	
	int 	ctrl=0;
	int 	color;
	int 	level;
	int 	device;	//设备
	char 	*p=NULL;
	int 	change=1;		//是否要改变灯的状态  1是  2否
	
	dgb_printf_safe("task msg is create ok\r\n");

	while(1)
	{
		//等待消息队列
		//0,一直等待
		//OS_OPT_PEND_BLOCKING，阻塞形式等待；若等待不了消息，则让出CPU使用权给其他任务
		//不需要时间戳（时间标记），不需要获取消息的所花的时间
		p = OSQPend(&g_queue_msg,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		if(err!=OS_ERR_NONE)
		{
			dgb_printf_safe("OSQPend g_queue_msg error code = %d\r\n",err);
			delay_ms(2000);
			continue;
		}
		
		//dgb_printf_safe("msg[%s] len[%d]\r\n",p,msg_size);
		//处理消息  所有消息格式如下
//		ctrl:1,1#	ctrl:2,1#
//		ctrl:3,2#	ctrl:4,2#
//		ctrl:5,3#	ctrl:6,3#
//		color:0,1#
//		level:86,2#
		
		//等待互斥锁
		OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		
		if(strstr(p,"color"))     //color:1,3#
		{
			device = atoi(strstr(p,",")+1);
			color = atoi(p+6);	//提取颜色值
			if(device == 1)
				color_led1 = color;
			else if(device ==2)
				color_led2 = color;
			else if(device == 3)
				color_led1 = color_led2 = color;

			dgb_printf_safe("msgtask led1 color:%d,led2 color:%d\r\n",color_led1,color_led2);
		}
		else if(strstr(p,"level"))
		{
			device = atoi(strstr(p,",")+1);
			level = atoi(p+6);
			if(device==1)
				level_led1 = level;
			else if(device == 2)
				level_led2 = level;
			else if(device == 3)
				level_led1 = level_led2 = level;
			
			dgb_printf_safe("msgtask led1 level:%d,led2 level:%d\r\n",level_led1,level_led2);
		}
		else if(strstr(p,"ctrl"))
		{
			ctrl = atoi(p+5);
			if(ctrl == 1)		//全开  颜色不变，亮度为原亮度
			{
				led1_level_max();	led2_level_max();
			}
			else if(ctrl == 2)	//全关  颜色不变，亮度为0
			{
				led1_level_min();	led2_level_min();
			}
			else if(ctrl == 3)	//开启自动调光
			{
				OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_ADC_CTRL,OS_OPT_POST_FLAG_SET,&err);
				change = 0;
			}
			else if(ctrl == 4)	//关闭自动调光
			{
				OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_ADC_CTRL,OS_OPT_POST_FLAG_CLR,&err);
				change = 0;
			}
			else if(ctrl == 5)		//开手势
			{
				OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_PAJ7620,OS_OPT_POST_FLAG_SET,&err);
				change = 0;
			}
			else if(ctrl == 6)		//关手势
			{
				OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_PAJ7620,OS_OPT_POST_FLAG_CLR,&err);
				change = 0;
			}
			
			
		}
		//释放互斥锁,等待锁的任务（最高优先级且就绪）得到立即执行
		OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
		
		if(change)
		{
			//发送消息给两个led任务
			send_message_to_led();

			//等待事件标志组
			//与同步,也就是等待  led1 和 led2都作出对应改变之后，才会往下走
			flags = OSFlagPend(&g_flag_grp,FLAG_GRP_LED_1_OK
										  |FLAG_GRP_LED_2_OK,
										  0,OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME+OS_OPT_PEND_BLOCKING,NULL,&err);
			//led颜色改变之后，上报状态给上位机
			esp8266_commit_led_status();
		}
		
		
		change = 1;
		//清空esp8266消息接收缓冲区
		memset(p,0,msg_size);	
		
	}
}




