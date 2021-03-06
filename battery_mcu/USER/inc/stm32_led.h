#ifndef _STM32_LED_H_
#define _STM32_LED_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stm32f0xx.h>

typedef struct stm32_led_s
{
    GPIO_TypeDef    *port;
    uint16_t        pin;
    uint8_t         timer;
    uint16_t        interval[2];
    uint8_t         status;
    void            (*init)         (struct stm32_led_s *, uint16_t, GPIO_TypeDef *);
    void            (*set)          (struct stm32_led_s *, uint16_t, uint16_t);
    void            (*doing)        (struct stm32_led_s *);
    void            (*dark)         (struct stm32_led_s *);
    void            (*bright)       (struct stm32_led_s *);
} STM32_LED_S;

void stm32_led_init(STM32_LED_S *led, uint16_t pin, GPIO_TypeDef *port);

#ifdef __cplusplus
}
#endif

#endif /* _STM32_LED_H_ */

