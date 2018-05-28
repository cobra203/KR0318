#include <stm32f0xx_gpio.h>
#include <stm32f0xx_adc.h>

#include <stm32_adc.h>

static void stm32_adc_conversion(STM32_ADC_S *adc)
{
	ADC_StartOfConversion(ADC1);

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));

	adc->voltage = (ADC_GetConversionValue(ADC1) * 3300) / 0xFFF;
}

void stm32_adc_init(STM32_ADC_S *adc, uint16_t pin, GPIO_TypeDef *port)
{
	GPIO_InitTypeDef	gpio_struct;
    ADC_InitTypeDef		adc_struct;

	adc->pin		= pin;
	adc->port		= port;
	adc->conversion	= stm32_adc_conversion;

	gpio_struct.GPIO_Speed	= GPIO_Speed_50MHz;
	gpio_struct.GPIO_Mode	= GPIO_Mode_AN;
	gpio_struct.GPIO_PuPd	= GPIO_PuPd_NOPULL;
    gpio_struct.GPIO_Pin	= adc->pin;
    GPIO_Init(adc->port, &gpio_struct);

    ADC_DeInit(ADC1);

	adc_struct.ADC_Resolution			= ADC_Resolution_12b;
	adc_struct.ADC_ContinuousConvMode	= DISABLE;
	adc_struct.ADC_ExternalTrigConv		= ADC_ExternalTrigConvEdge_None;
	adc_struct.ADC_ExternalTrigConvEdge	= ADC_ExternalTrigConvEdge_None;  
	adc_struct.ADC_DataAlign			= ADC_DataAlign_Right;
	adc_struct.ADC_ScanDirection		= ADC_ScanDirection_Upward;

	ADC_Init(ADC1, &adc_struct);

	ADC_ChannelConfig(ADC1, ADC_Channel_0 , ADC_SampleTime_55_5Cycles);
	ADC_GetCalibrationFactor(ADC1);
	  
	ADC_Cmd(ADC1, ENABLE);

	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN));
}

