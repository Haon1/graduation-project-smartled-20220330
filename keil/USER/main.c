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


int level_led1=0;		//LED1 ������
int color_led1=7;		//LED1 ��ǰ��ɫ

int level_led2=0;		//LED2 ������
int color_led2=7;		//LED2 ��ǰ��ɫ


//����init���ƿ�
OS_TCB app_task_tcb_init;
void app_task_init(void *parg);
CPU_STK app_task_stk_init[512];			//�����ջ����СΪ512�֣�Ҳ����512�� 2048�ֽ�


//����esp8266���ӷ��������ƿ�
OS_TCB app_task_tcb_esp8266_connect_server;
void app_task_esp8266_connect_server(void *parg);
CPU_STK app_task_stk_esp8266_connect_server[512];			//�����ջ����СΪ128��  512�ֽ�

//����led1���ƿ�
OS_TCB app_task_tcb_led1;
void app_task_led1(void *parg);
CPU_STK app_task_stk_led1[512];			//�����ջ����СΪ512��  2048�ֽ�

//����led2���ƿ�
OS_TCB app_task_tcb_led2;
void app_task_led2(void *parg);
CPU_STK app_task_stk_led2[512];			//�����ջ����СΪ512��  2048�ֽ�

//�������ƿ��ƿ�
OS_TCB app_task_tcb_paj7620;
void app_task_paj7620(void *parg);
CPU_STK app_task_stk_paj7620[512];			//�������ջ����СΪ512��  2048�ֽ�

//����adc���ƿ�
OS_TCB app_task_tcb_adc;
void app_task_adc(void *parg);
CPU_STK app_task_stk_adc[512];			//�����ջ����СΪ512��  2048�ֽ�

//����key���ƿ�
OS_TCB app_task_tcb_key;
void app_task_key(void *parg);
CPU_STK app_task_stk_key[512];			//�����ջ����СΪ512��  2048�ֽ�

//������Ϣ������ƿ�
OS_TCB app_task_tcb_msg;
void app_task_msg(void *parg);
CPU_STK app_task_stk_msg[512];			//�����ջ����СΪ512��  2048�ֽ�


//�ں˶��� 
OS_Q   					g_queue_msg;			//��Ϣ���еĶ���
OS_Q   					g_queue_led1;			//��Ϣ���еĶ���
OS_Q   					g_queue_led2;			//��Ϣ���еĶ���
OS_Q   					g_queue_connect;		//��Ϣ���еĶ���


OS_MUTEX				g_mutex_printf;			//�������Ķ���
OS_MUTEX				g_mutex_led_value;		//�������Ķ���

OS_FLAG_GRP   			g_flag_grp;				//�¼���־��	


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


//������
int main(void)
{
	OS_ERR err;

	systick_init();  													//ʱ�ӳ�ʼ��
	usart_init(115200);  												//���ڳ�ʼ��
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						//�жϷ�������

	//OS��ʼ�������ǵ�һ�����еĺ���,��ʼ�����ֵ�ȫ�ֱ����������ж�Ƕ�׼����������ȼ����洢��
	OSInit(&err);


	//������ʼ������
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_init,									//������ƿ飬��ͬ���߳�id
					(CPU_CHAR *)"app_task_init",									//��������֣����ֿ����Զ����
					(OS_TASK_PTR)app_task_init,										//����������ͬ���̺߳���
					(void *)0,												//���ݲ�������ͬ���̵߳Ĵ��ݲ���
					(OS_PRIO)6,											 	//��������ȼ�6		
					(CPU_STK *)app_task_stk_init,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
					
	if(err!=OS_ERR_NONE)
	{
		printf("init  fail\r\n");
		
		while(1);
	}

	//����OS�������������
	OSStart(&err);
					
					
	while(1);
	
}

//��ʼ��Ӳ������
void app_task_init(void *parg)
{
	OS_ERR err;

	

	printf("task1 init  create ok\r\n");
	
	rgb_led_init();		//ȫ��LED��ʼ��	
	LED_Init();     	//ϵͳLED��ʼ��	 
	bt24_init(9600);	//����ģ���ʼ��
	key_board_init();	//���������ʼ��
	
	adc_init();		//adc��ʼ��
	printf("ADC OK\r\n");
	 
	//PAJ7620U2��������ʼ��
 	while(!paj7620u2_init())
	{
	    printf("PAJ7620U2 Error!!!\r\n");
		delay_ms(500);
	}
    printf("PAJ7620U2 OK\r\n");
	
	//esp8266��ʼ��
	esp8266_init();
	printf("Esp8266 init OK\r\n");
	
	
	//������Ϣ���У�֧��16����Ϣ��ʵ�����ܹ��洢16��ָ�룩
	OSQCreate(&g_queue_connect,"g_queue_connect",16,&err);
	OSQCreate(&g_queue_msg,"g_queue_msg",16,&err);
	OSQCreate(&g_queue_led1,"g_queue_led1",16,&err);
	OSQCreate(&g_queue_led2,"g_queue_led2",16,&err);
	
		
	//����������
	OSMutexCreate(&g_mutex_printf,	"g_mutex_printf",&err);
	OSMutexCreate(&g_mutex_led_value,	"g_mutex_led_value",&err);	
	
	//�����¼���־��
	OSFlagCreate(&g_flag_grp,"g_flag_grp",0,&err);

	//�������ӷ���������
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_esp8266_connect_server,			//������ƿ�
					(CPU_CHAR *)"app_task_esp8266_connect_server",			//���������
					(OS_TASK_PTR)app_task_esp8266_connect_server,			//������
					(void *)0,												//���ݲ���
					(OS_PRIO)6,											 	//��������ȼ�6		
					(CPU_STK *)app_task_stk_esp8266_connect_server,			//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
	
	//��������led1
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_led1,									//������ƿ�
					(CPU_CHAR *)"app_task_led1",									//���������
					(OS_TASK_PTR)app_task_led1,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)7,											 	//��������ȼ�7		
					(CPU_STK *)app_task_stk_led1,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
					
	//��������led2
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_led2,									//������ƿ�
					(CPU_CHAR *)"app_task_led2",									//���������
					(OS_TASK_PTR)app_task_led2,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)7,											 	//��������ȼ�7		
					(CPU_STK *)app_task_stk_led2,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
					
	//������������
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_paj7620,									//������ƿ�
					(CPU_CHAR *)"app_task_paj7620",									//���������
					(OS_TASK_PTR)app_task_paj7620,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)7,											 	//��������ȼ�7		
					(CPU_STK *)app_task_stk_paj7620,						//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
	//�������񰴼�
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_key,									//������ƿ�
					(CPU_CHAR *)"app_task_key",									//���������
					(OS_TASK_PTR)app_task_key,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)7,											 	//��������ȼ�7		
					(CPU_STK *)app_task_stk_key,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
					
	//��������adc
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_adc,									//������ƿ�
					(CPU_CHAR *)"app_task_adc",									//���������
					(OS_TASK_PTR)app_task_adc,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)7,											 	//��������ȼ�7		
					(CPU_STK *)app_task_stk_adc,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
					
	//����������Ϣ����
	OSTaskCreate(	(OS_TCB *)&app_task_tcb_msg,									//������ƿ�
					(CPU_CHAR *)"app_task_msg",									//���������
					(OS_TASK_PTR)app_task_msg,										//������
					(void *)0,												//���ݲ���
					(OS_PRIO)7,											 	//��������ȼ�7		
					(CPU_STK *)app_task_stk_msg,									//�����ջ����ַ
					(CPU_STK_SIZE)512/10,									//�����ջ�����λ���õ����λ�ã��������ټ���ʹ��
					(CPU_STK_SIZE)512,										//�����ջ��С			
					(OS_MSG_QTY)0,											//��ֹ������Ϣ����
					(OS_TICK)0,												//Ĭ��ʱ��Ƭ����																
					(void  *)0,												//����Ҫ�����û��洢��
					(OS_OPT)OS_OPT_TASK_NONE,								//û���κ�ѡ��
					&err													//���صĴ�����
				);
					
	
	//ɾ���������񣬽�������̬
	OSTaskDel(NULL,&err);
}


//esp8266���ӷ���������
void app_task_esp8266_connect_server(void *parg)
{
	int rt;
	OS_ERR err;
	OS_MSG_SIZE msg_size=0;//������յ���Ϣ�Ĵ�С
	
	char *p = NULL;
	char *tmp = NULL;
	char msg[128]={0};
	char server_ip[16]={0};		//������ip
	uint16_t server_port;		//�������˿�
	
	dgb_printf_safe("task esp8266 connect server is create ok\r\n");

	while(1)
	{	
		dgb_printf_safe("get_server_infomation\r\n");
		//�ȴ���Ϣ
		p = OSQPend(&g_queue_connect,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		if(strstr(p,"ip")&&strstr(p,"port")&&strstr(p,"#"))
		{
			//��Ϣ��ʽ ip:192.168.2.3,port:8026#	ǰ��Ϊip,����˿�
			strncpy(msg,p,msg_size);
			tmp = strtok(msg,",");				//�� ","�и������ ip:xxx.xxx.xxx.xxx  port:xxxx
			strcpy(server_ip,tmp+3);			//��ȡip
			tmp = strtok(NULL,",");
			server_port = atoi(tmp+5);			//��ȡ�˿�
			
			printf("ip is %s, port is %d\r\n",server_ip,server_port);
			
			//����qt������
			rt =esp8266_connect_server("TCP",server_ip,server_port);
			if(rt == 0)
				printf("esp8266_connect_server success\r\n");
			else
				printf("esp8266_connect_server fail\r\n");
			
			delay_ms(2000);
			
			//����͸��ģʽ
			rt =esp8266_entry_transparent_transmission();
			if(rt == 0)
				printf("esp8266_entry_transparent_transmission success\r\n");
			else
				printf("esp8266_entry_transparent_transmission fail\r\n");
			
			esp8266_commit_led_status();	//�ϱ�led��ǰ״̬
			
			break;
		}
	}
	
	printf("connect task exit\r\n");
	memset(p,0,msg_size);
	//ɾ���������񣬽�������̬
	OSTaskDel(NULL,&err);
}


//led1����
void app_task_led1(void *parg)
{
	OS_FLAGS  flags=0;
	OS_ERR err;
	OS_MSG_SIZE msg_size=0;//������յ���Ϣ�Ĵ�С
	
	char *p = NULL;
	
	
	printf("task led  1 is create ok\r\n");

	while(1)
	{
		//�ȴ���Ϣ����
		//0,һֱ�ȴ�
		//OS_OPT_PEND_BLOCKING��������ʽ�ȴ������ȴ�������Ϣ�����ó�CPUʹ��Ȩ����������
		//����Ҫʱ�����ʱ���ǣ�������Ҫ��ȡ��Ϣ��������ʱ��
		p = OSQPend(&g_queue_led1,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		dgb_printf_safe("[ led1 ] color:%d, level:%d\r\n",color_led1,level_led1);
		
		rgb_led1_crl(color_led1,level_led1);
		
		//�����¼���־���bit0
		OSFlagPost(&g_flag_grp,FLAG_GRP_LED_1_OK,OS_OPT_POST_FLAG_SET,&err);
	}
}

//led 2����
void app_task_led2(void *parg)
{
	OS_FLAGS  flags=0;
	OS_ERR err;
	OS_MSG_SIZE msg_size=0;//������յ���Ϣ�Ĵ�С
	

	char *p = NULL;
	
	dgb_printf_safe("task led 2 is create ok\r\n");

	while(1)
	{
		//�ȴ���Ϣ����
		//0,һֱ�ȴ�
		//OS_OPT_PEND_BLOCKING��������ʽ�ȴ������ȴ�������Ϣ�����ó�CPUʹ��Ȩ����������
		//����Ҫʱ�����ʱ���ǣ�������Ҫ��ȡ��Ϣ��������ʱ��
		p = OSQPend(&g_queue_led2,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		dgb_printf_safe("[ led2 ] color:%d, level:%d\r\n",color_led2,level_led2);
		
		rgb_led2_crl(color_led2,level_led2);
		
		//�����¼���־���bit1
		OSFlagPost(&g_flag_grp,FLAG_GRP_LED_2_OK,OS_OPT_POST_FLAG_SET,&err);
	}
}

//����ʶ������
void app_task_paj7620(void *parg)
{
	
	dgb_printf_safe("task paj is create ok\r\n");

	while(1)
	{
		Gesture_test();
	}
}

//ģ��ת������
void app_task_adc(void *parg)
{
	OS_ERR 		err;
	OS_FLAGS  	flags=0;
	
	uint32_t 	adc_val;
	
	
	dgb_printf_safe("task adc is create ok \r\n");
	
	//����ADC3ת��
	ADC_SoftwareStartConv(ADC3);
	delay_ms(1000);

	while(1)
	{
		//�ȴ��¼���־		���Ϊ1�Ͷ�ȡ�������ó�cpuʹ��Ȩ
		flags = OSFlagPend(&g_flag_grp,FLAG_GRP_OPEN_ADC_CTRL,0,OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_BLOCKING,NULL,&err);

		
		//��ȡ�˲��㷨���
		adc_val = adc_filer();		// 15 3830
		
		OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		level_led1 = level_led2 = adc_val;
		OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
		
		rgb_led1_crl(color_led1,level_led1);		//����LED1����
		rgb_led2_crl(color_led2,level_led2);		//����LED2����
		
		dgb_printf_safe("adc_val=%d\r\n",adc_val);			//286-3760 = 3473   
		

		esp8266_commit_led_status();	//�ϱ�״̬
		
		delay_ms(200);
	}
}

//����ʶ������
void app_task_key(void *parg)
{
	uint32_t key_sta=0;
	char key_old=0;
	char key_cur=0;
	int rt=0;
	
	dgb_printf_safe("task key is create ok\r\n");

	while(1)
	{
		/* ʹ��״̬��˼��õ�������״̬ */
		switch(key_sta)
		{
			case 0://��ȡ���µİ���
			{
				
				key_cur = get_key_board();	

				if(key_cur != 'N')
				{
					key_old = key_cur;
					key_sta=1;
				}
			}break;
			
			case 1://ȷ�ϰ��µİ���
			{
				
				key_cur = get_key_board();	
					
				if((key_cur != 'N') && (key_cur == key_old))
				{
					dgb_printf_safe("KEY %c Down\r\n",key_cur);
					key_sta=2;
					
					rt = deal_key_code(key_cur);			//�������¼�
					if(rt == 1)
					{
						send_message_to_led();		//֪ͨled
						esp8266_commit_led_status();	//�������ϱ�
					}
				}			
			}break;
		
			case 2://��ȡ�ͷŵİ���
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

//��Ϣ��������
void app_task_msg(void *parg)
{
	OS_ERR err;
	OS_FLAGS  flags=0;
	OS_MSG_SIZE msg_size=0;//������յ���Ϣ�Ĵ�С
	
	int 	ctrl=0;
	int 	color;
	int 	level;
	int 	device;	//�豸
	char 	*p=NULL;
	int 	change=1;		//�Ƿ�Ҫ�ı�Ƶ�״̬  1��  2��
	
	dgb_printf_safe("task msg is create ok\r\n");

	while(1)
	{
		//�ȴ���Ϣ����
		//0,һֱ�ȴ�
		//OS_OPT_PEND_BLOCKING��������ʽ�ȴ������ȴ�������Ϣ�����ó�CPUʹ��Ȩ����������
		//����Ҫʱ�����ʱ���ǣ�������Ҫ��ȡ��Ϣ��������ʱ��
		p = OSQPend(&g_queue_msg,0,OS_OPT_PEND_BLOCKING,&msg_size,NULL,&err);
		
		if(err!=OS_ERR_NONE)
		{
			dgb_printf_safe("OSQPend g_queue_msg error code = %d\r\n",err);
			delay_ms(2000);
			continue;
		}
		
		//dgb_printf_safe("msg[%s] len[%d]\r\n",p,msg_size);
		//������Ϣ  ������Ϣ��ʽ����
//		ctrl:1,1#	ctrl:2,1#
//		ctrl:3,2#	ctrl:4,2#
//		ctrl:5,3#	ctrl:6,3#
//		color:0,1#
//		level:86,2#
		
		//�ȴ�������
		OSMutexPend(&g_mutex_led_value,0,OS_OPT_PEND_BLOCKING,NULL,&err);
		
		if(strstr(p,"color"))     //color:1,3#
		{
			device = atoi(strstr(p,",")+1);
			color = atoi(p+6);	//��ȡ��ɫֵ
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
			if(ctrl == 1)		//ȫ��  ��ɫ���䣬����Ϊԭ����
			{
				led1_level_max();	led2_level_max();
			}
			else if(ctrl == 2)	//ȫ��  ��ɫ���䣬����Ϊ0
			{
				led1_level_min();	led2_level_min();
			}
			else if(ctrl == 3)	//�����Զ�����
			{
				OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_ADC_CTRL,OS_OPT_POST_FLAG_SET,&err);
				change = 0;
			}
			else if(ctrl == 4)	//�ر��Զ�����
			{
				OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_ADC_CTRL,OS_OPT_POST_FLAG_CLR,&err);
				change = 0;
			}
			else if(ctrl == 5)		//������
			{
				OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_PAJ7620,OS_OPT_POST_FLAG_SET,&err);
				change = 0;
			}
			else if(ctrl == 6)		//������
			{
				OSFlagPost(&g_flag_grp,FLAG_GRP_OPEN_PAJ7620,OS_OPT_POST_FLAG_CLR,&err);
				change = 0;
			}
			
			
		}
		//�ͷŻ�����,�ȴ���������������ȼ��Ҿ������õ�����ִ��
		OSMutexPost(&g_mutex_led_value,OS_OPT_POST_NONE,&err);
		
		if(change)
		{
			//������Ϣ������led����
			send_message_to_led();

			//�ȴ��¼���־��
			//��ͬ��,Ҳ���ǵȴ�  led1 �� led2��������Ӧ�ı�֮�󣬲Ż�������
			flags = OSFlagPend(&g_flag_grp,FLAG_GRP_LED_1_OK
										  |FLAG_GRP_LED_2_OK,
										  0,OS_OPT_PEND_FLAG_SET_ALL+OS_OPT_PEND_FLAG_CONSUME+OS_OPT_PEND_BLOCKING,NULL,&err);
			//led��ɫ�ı�֮���ϱ�״̬����λ��
			esp8266_commit_led_status();
		}
		
		
		change = 1;
		//���esp8266��Ϣ���ջ�����
		memset(p,0,msg_size);	
		
	}
}




