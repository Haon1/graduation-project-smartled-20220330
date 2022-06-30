#include "includes.h"


//adc��ʼ��
void adc_init(void)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef       ADC_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;

	//ʹ�ܶ˿�C��Ӳ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	
	//ʹ��ADC3Ӳ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

	//����PC0����Ϊģ���ź�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;	//��0������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;//ģ���ź�ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;//Ŀǰ����Ҫ����������ӹ���
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//ʹ��һ��ADC����
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div2;//ADC��Ӳ��ʱ��=84MHz/2=42MHz
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;//�ر�DMA
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//����������ļ����5��ʱ�����ھ���5*(1/42MHz)
	ADC_CommonInit(&ADC_CommonInitStructure);
	
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12�ֱ��ʣ�Ҳ���������adcֵΪ12bit
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;//ʹ�ܵ�ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//��������
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//����Ҫ����������ADC����
	//ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//���ڼĴ����Ҳ�
	ADC_InitStructure.ADC_NbrOfConversion = 1;//ֻ��һ��ͨ������ɨ��
	ADC_Init(ADC3, &ADC_InitStructure);
	
	/* ADC3 regular channel 10 configuration ������1����������ת��ʱ��Ϊ3��ʱ�����ڣ�����3*(1/42MHz)*/
	ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_3Cycles);
	
	//ADC3����
	ADC_Cmd(ADC3, ENABLE);
}

//adc����
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
		//�ȴ�ADC1ת������
		while(ADC_GetFlagStatus(ADC3,ADC_FLAG_EOC)==RESET);
		ADC_ClearFlag(ADC3,ADC_FLAG_EOC);

		//��ȡת�����
		adc_val = ADC_GetConversionValue(ADC3);

		//�����ֵת��Ϊ�Ƚ�ֵ
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
	
	//ȥ����ߺ����
	for(count =1;count<N-1;count++)
		sum += Value_buf[count];
			

	return (sum/(N-2));
}