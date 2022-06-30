#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp8266.h"

extern int level_led1;		//LED1 ��ǰ����   main.c�ж��� 
extern int color_led1;		//LED1 ��ǰ��ɫ	main.c�ж���

extern int level_led2;		//LED2 ��ǰ����   main.c�ж��� 
extern int color_led2;		//LED2 ��ǰ��ɫ	main.c�ж���

volatile uint8_t  g_esp8266_rx_buf[512];
volatile uint32_t g_esp8266_rx_cnt=0;
volatile uint32_t g_esp8266_rx_end=0;

volatile uint32_t g_esp8266_transparent_transmission_sta=0;

//esp8266��ʼ��   ����2
void esp8266_init(void)
{
	int rt;
	
	usart2_init(115200);
	
	//�˳�͸��ģʽ����������ATָ��
	rt=esp8266_exit_transparent_transmission();
	if(rt == 0)
	{
		printf("esp8266_exit_transparent_transmission success\r\n");
	}	
	
	delay_ms(2000);
		
	//�Ͽ�������
	rt = esp8266_disconnect_server();
	if(rt == 0)
		printf("esp8266_disconnect_server success\r\n");
	else
		printf("esp8266 is no connection to the server\r\n");
		
		
	
	//��Ҫ����֮��Ҫ����ʱ��������һ��ATָ��������
	delay_ms(2000);	
	
	//�����Լ����
	while(esp8266_self_test())
	{
		printf("esp8266 self testing ...\r\n");
		delay_ms(500);
	}
	

	//��Ҫ����֮��Ҫ����ʱ��������һ��ATָ��������
	delay_ms(2000);
	
	//�����ȵ�
	//
	rt = esp8266_connect_ap("LED","88888888");
	if(rt == 0)
		printf("esp8266_connect_ap success\r\n");
	else
		printf("esp8266_connect_ap fail\r\n");	
	
	//��Ҫ����֮��Ҫ����ʱ��������һ��ATָ��������
	delay_ms(2000);
}

//ͨ��esp8266�ϱ�led״̬
void esp8266_commit_led_status(void)
{
	char commit_msg[64]={0};
	sprintf(commit_msg,"%d,%d,%d,%d#",color_led1,level_led1,color_led2,level_led2);
	esp8266_send_at(commit_msg);
}

//esp8266�����ַ���
void esp8266_send_at(char *str)
{
	//��ս��ջ�����
	memset((void *)g_esp8266_rx_buf,0, sizeof g_esp8266_rx_buf);
	
	//��ս��ռ���ֵ
	g_esp8266_rx_cnt = 0;	
	
	//����2��������
	usart2_send_str(str);
}

void esp8266_send_bytes(uint8_t *buf,uint32_t len)
{
	usart2_send_bytes(buf,len);

}

void esp8266_send_str(char *buf)
{
	usart2_send_str(buf);

}

/* ���ҽ������ݰ��е��ַ��� */
int32_t esp8266_find_str_in_rx_packet(char *str,uint32_t timeout)
{
	char *p = str;
	
	//�ȴ����ڽ�����ϻ�ʱ�˳�
	while((strstr((const char *)g_esp8266_rx_buf,p)==NULL) && timeout)
	{		
		delay_ms(1);
		timeout--;
	}
	

	//printf("[find str]%s"(const char *)g_esp8266_rx_buf);

	if(timeout) 	//û�г�ʱ,�ҵ���
	{

		return 0; 
	}
		                      

	return -1; 
}


/* �Լ���� */
int32_t  esp8266_self_test(void)
{
	esp8266_send_at("AT\r\n");
	
	return esp8266_find_str_in_rx_packet("OK",1000);
}

/**
 * ���ܣ������ȵ�
 * ������
 *         ssid:�ȵ���
 *         pwd:�ȵ�����
 * ����ֵ��
 *         ���ӽ��,0���ӳɹ�,��0����ʧ��
 * ˵���� 
 *         ʧ�ܵ�ԭ�������¼���(UARTͨ�ź�ESP8266���������)
 *         1. WIFI�������벻��ȷ
 *         2. ·���������豸̫��,δ�ܸ�ESP8266����IP
 */
int32_t esp8266_connect_ap(char* ssid,char* pswd)
{

	char buf[64]={0};
	
    //����ΪSTATIONģʽ	
	esp8266_send_at("AT+CWMODE_CUR=1\r\n"); 
	
	if(esp8266_find_str_in_rx_packet("OK",1000))
		return -1;


	//����Ŀ��AP
	sprintf(buf,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,pswd);
	esp8266_send_at(buf); 
	
	if(esp8266_find_str_in_rx_packet("OK",5000))
		if(esp8266_find_str_in_rx_packet("CONNECT",5000))
			return -2;

	return 0;
}



/* �˳�͸��ģʽ */
int32_t esp8266_exit_transparent_transmission (void)
{
	//�ٷ�Ҫ�������һ��ָ��Ҫ1��
	delay_ms ( 1000 );
	
	esp8266_send_at ("+++");
	
	delay_ms ( 500 ); 
	
	//��¼��ǰesp8266�����ڷ�͸��ģʽ
	g_esp8266_transparent_transmission_sta = 0;

	return esp8266_find_str_in_rx_packet("+++",5000);
}

/* ����͸��ģʽ */
int32_t  esp8266_entry_transparent_transmission(void)
{
	//����͸��ģʽ
	esp8266_send_at("AT+CIPMODE=1\r\n");  
	if(esp8266_find_str_in_rx_packet("OK",5000))
		return -1;
	
	
	//��������״̬
	esp8266_send_at("AT+CIPSEND\r\n");
	if(esp8266_find_str_in_rx_packet("OK",5000))
		return -2;

	//��¼��ǰesp8266������͸��ģʽ
	g_esp8266_transparent_transmission_sta = 1;
	return 0;
}


/**
 * ���ܣ�ʹ��ָ��Э��(TCP/UDP)���ӵ�������
 * ������
 *         mode:Э������ "TCP","UDP"
 *         ip:Ŀ�������IP
 *         port:Ŀ���Ƿ������˿ں�
 * ����ֵ��
 *         ���ӽ��,��0���ӳɹ�,0����ʧ��
 * ˵���� 
 *         ʧ�ܵ�ԭ�������¼���(UARTͨ�ź�ESP8266���������)
 *         1. Զ�̷�����IP�Ͷ˿ں�����
 *         2. δ����AP
 *         3. �������˽�ֹ���(һ�㲻�ᷢ��)
 */
int32_t esp8266_connect_server(char* mode,char* ip,uint16_t port)
{
	
	char buf[64]={0};
   
	//���ӷ�����
	sprintf((char*)buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",mode,ip,port);
	
	esp8266_send_at(buf);

	if(esp8266_find_str_in_rx_packet("CONNECT",5000))
		if(esp8266_find_str_in_rx_packet("OK",5000))
			return -1;
	return 0;
}

/* �Ͽ������� */
int32_t esp8266_disconnect_server(void)
{
	esp8266_send_at("AT+CIPCLOSE\r\n");
		
	if(esp8266_find_str_in_rx_packet("CLOSED",5000))
		if(esp8266_find_str_in_rx_packet("OK",5000))
			return -1;	//������û�ҵ�  ���� -1
	
	return 0;	
}

