#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "esp8266.h"

extern int level_led1;		//LED1 当前亮度   main.c中定义 
extern int color_led1;		//LED1 当前颜色	main.c中定义

extern int level_led2;		//LED2 当前亮度   main.c中定义 
extern int color_led2;		//LED2 当前颜色	main.c中定义

volatile uint8_t  g_esp8266_rx_buf[512];
volatile uint32_t g_esp8266_rx_cnt=0;
volatile uint32_t g_esp8266_rx_end=0;

volatile uint32_t g_esp8266_transparent_transmission_sta=0;

//esp8266初始化   串口2
void esp8266_init(void)
{
	int rt;
	
	usart2_init(115200);
	
	//退出透传模式，才能输入AT指令
	rt=esp8266_exit_transparent_transmission();
	if(rt == 0)
	{
		printf("esp8266_exit_transparent_transmission success\r\n");
	}	
	
	delay_ms(2000);
		
	//断开服务器
	rt = esp8266_disconnect_server();
	if(rt == 0)
		printf("esp8266_disconnect_server success\r\n");
	else
		printf("esp8266 is no connection to the server\r\n");
		
		
	
	//重要操作之间要加延时，否则下一条AT指令有问题
	delay_ms(2000);	
	
	//进行自检测试
	while(esp8266_self_test())
	{
		printf("esp8266 self testing ...\r\n");
		delay_ms(500);
	}
	

	//重要操作之间要加延时，否则下一条AT指令有问题
	delay_ms(2000);
	
	//连接热点
	//
	rt = esp8266_connect_ap("LED","88888888");
	if(rt == 0)
		printf("esp8266_connect_ap success\r\n");
	else
		printf("esp8266_connect_ap fail\r\n");	
	
	//重要操作之间要加延时，否则下一条AT指令有问题
	delay_ms(2000);
}

//通过esp8266上报led状态
void esp8266_commit_led_status(void)
{
	char commit_msg[64]={0};
	sprintf(commit_msg,"%d,%d,%d,%d#",color_led1,level_led1,color_led2,level_led2);
	esp8266_send_at(commit_msg);
}

//esp8266发送字符串
void esp8266_send_at(char *str)
{
	//清空接收缓冲区
	memset((void *)g_esp8266_rx_buf,0, sizeof g_esp8266_rx_buf);
	
	//清空接收计数值
	g_esp8266_rx_cnt = 0;	
	
	//串口2发送数据
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

/* 查找接收数据包中的字符串 */
int32_t esp8266_find_str_in_rx_packet(char *str,uint32_t timeout)
{
	char *p = str;
	
	//等待串口接收完毕或超时退出
	while((strstr((const char *)g_esp8266_rx_buf,p)==NULL) && timeout)
	{		
		delay_ms(1);
		timeout--;
	}
	

	//printf("[find str]%s"(const char *)g_esp8266_rx_buf);

	if(timeout) 	//没有超时,找到了
	{

		return 0; 
	}
		                      

	return -1; 
}


/* 自检程序 */
int32_t  esp8266_self_test(void)
{
	esp8266_send_at("AT\r\n");
	
	return esp8266_find_str_in_rx_packet("OK",1000);
}

/**
 * 功能：连接热点
 * 参数：
 *         ssid:热点名
 *         pwd:热点密码
 * 返回值：
 *         连接结果,0连接成功,非0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. WIFI名和密码不正确
 *         2. 路由器连接设备太多,未能给ESP8266分配IP
 */
int32_t esp8266_connect_ap(char* ssid,char* pswd)
{

	char buf[64]={0};
	
    //设置为STATION模式	
	esp8266_send_at("AT+CWMODE_CUR=1\r\n"); 
	
	if(esp8266_find_str_in_rx_packet("OK",1000))
		return -1;


	//连接目标AP
	sprintf(buf,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,pswd);
	esp8266_send_at(buf); 
	
	if(esp8266_find_str_in_rx_packet("OK",5000))
		if(esp8266_find_str_in_rx_packet("CONNECT",5000))
			return -2;

	return 0;
}



/* 退出透传模式 */
int32_t esp8266_exit_transparent_transmission (void)
{
	//官方要求，相隔上一条指令要1秒
	delay_ms ( 1000 );
	
	esp8266_send_at ("+++");
	
	delay_ms ( 500 ); 
	
	//记录当前esp8266工作在非透传模式
	g_esp8266_transparent_transmission_sta = 0;

	return esp8266_find_str_in_rx_packet("+++",5000);
}

/* 进入透传模式 */
int32_t  esp8266_entry_transparent_transmission(void)
{
	//进入透传模式
	esp8266_send_at("AT+CIPMODE=1\r\n");  
	if(esp8266_find_str_in_rx_packet("OK",5000))
		return -1;
	
	
	//开启发送状态
	esp8266_send_at("AT+CIPSEND\r\n");
	if(esp8266_find_str_in_rx_packet("OK",5000))
		return -2;

	//记录当前esp8266工作在透传模式
	g_esp8266_transparent_transmission_sta = 1;
	return 0;
}


/**
 * 功能：使用指定协议(TCP/UDP)连接到服务器
 * 参数：
 *         mode:协议类型 "TCP","UDP"
 *         ip:目标服务器IP
 *         port:目标是服务器端口号
 * 返回值：
 *         连接结果,非0连接成功,0连接失败
 * 说明： 
 *         失败的原因有以下几种(UART通信和ESP8266正常情况下)
 *         1. 远程服务器IP和端口号有误
 *         2. 未连接AP
 *         3. 服务器端禁止添加(一般不会发生)
 */
int32_t esp8266_connect_server(char* mode,char* ip,uint16_t port)
{
	
	char buf[64]={0};
   
	//连接服务器
	sprintf((char*)buf,"AT+CIPSTART=\"%s\",\"%s\",%d\r\n",mode,ip,port);
	
	esp8266_send_at(buf);

	if(esp8266_find_str_in_rx_packet("CONNECT",5000))
		if(esp8266_find_str_in_rx_packet("OK",5000))
			return -1;
	return 0;
}

/* 断开服务器 */
int32_t esp8266_disconnect_server(void)
{
	esp8266_send_at("AT+CIPCLOSE\r\n");
		
	if(esp8266_find_str_in_rx_packet("CLOSED",5000))
		if(esp8266_find_str_in_rx_packet("OK",5000))
			return -1;	//两个都没找到  返回 -1
	
	return 0;	
}

