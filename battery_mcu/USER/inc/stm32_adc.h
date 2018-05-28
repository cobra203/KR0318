#ifndef _STM32_ADC_H_
#define _STM32_ADC_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stm32f0xx.h>

typedef struct stm32_adc_s
{
    GPIO_TypeDef    *port;
    uint16_t        pin;
	uint16_t		voltage;
	void            (*init)         (struct stm32_adc_s *, uint16_t, GPIO_TypeDef *);
	void			(*conversion)	(struct stm32_adc_s *);
} STM32_ADC_S;

void stm32_adc_init(STM32_ADC_S *adc, uint16_t pin, GPIO_TypeDef *port);

#ifdef __cplusplus
}
#endif

#endif /* _STM32_ADC_H_ */

