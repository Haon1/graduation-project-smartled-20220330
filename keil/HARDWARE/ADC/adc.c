#include "includes.h"


//adc初始化
void adc_init(void)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	//使能端口C的硬件时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//使能ADC3硬件时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	//配置PC0引脚为模拟信号引脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//第0根引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//模拟信号模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//目前不需要，否则会增加功耗
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//使能一个ADC工作
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//ADC的硬件时钟=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//关闭DMA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样点的间隔是5个时钟周期就是5*(1/42MHz)
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12分辨率，也就是输出的adc值为12bit
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//使能单通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续工作
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//不需要脉冲来触发ADC工作
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//放在寄存器右侧
	ADC_InitStructure.ADC_NbrOfConversion = 1;//只有一个通道进行扫描
	ADC_Init(ADC3, &ADC_InitStructure);
	
	/* ADC3 regular channel 10 configuration ，排序1，将采样点转换时间为3个时钟周期，就是3*(1/42MHz)*/
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
	
	//ADC3工作
	ADC_Cmd(ADC3, ENABLE);
}

//adc过滤
uint32_t adc_filer(void)
{
	#define N 200
	
	uint32_t temp;
	uint32_t adc_val,adc_vol;
	uint32_t count,i,j;
	uint32_t Value_buf[N];
	uint32_t sum=0;
	
	for(count=0;count<N;count++)
	{
		//等待ADC1转换结束
		while(ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)==RESET);
		ADC_ClearFlag(ADC3,ADC_FLAG_EOC);

		//读取转换结果
		adc_val = ADC_GetConversionValue(ADC3);

		//将结果值转换为比较值
		adc_vol = adc_val * 100 /3830;		
		
		Value_buf[count]= adc_vol;
	}
		
	
	for(j=0;j<(N-1);j++)
		for(i=0;i<(N-j);i++)
			if(Value_buf[i]>Value_buf[i+1])
			{
				 temp = Value_buf[i];
				 Value_buf[i]= Value_buf[i+1];
				  Value_buf[i+1]=temp;
			}
	
	//去掉最高和最低
	for(count =1;count<N-1;count++)
		sum += Value_buf[count];
			

	return (sum/(N-2));
}