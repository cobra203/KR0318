#ifndef SYS_PIN_DEF_H
#define SYS_PIN_DEF_H

#ifdef __cplusplus
 extern "C" {
#endif

#define VER_RELEASE         0
#define VER_DEBUG			1

#define VERSION VER_DEBUG

#define STAT_PIN			GPIO_Pin_2      /* fixed */
#define STAT_GPIO           GPIOA

#define VBAT_PIN			GPIO_Pin_0
#define VBAT_GPIO			GPIOA

#if (VERSION == VER_RELEASE)
#define LED_PIN_RED			GPIO_Pin_12
#define LED_PIN_COM1        GPIO_Pin_11
#define LED_PIN_COM2        GPIO_Pin_10
#define LED_PIN_COM3        GPIO_Pin_9
#elif (VERSION == VER_DEBUG)
#define LED_PIN_RED			GPIO_Pin_12
#define LED_PIN_COM1        GPIO_Pin_11
#define LED_PIN_COM2        GPIO_Pin_10
#define LED_PIN_COM3        GPIO_Pin_9
#endif

#define LED_GPIO_RED		GPIOA
#define LED_GPIO_COM1       GPIOA
#define LED_GPIO_COM2       GPIOA
#define LED_GPIO_COM3       GPIOA

#ifdef __cplusplus
}
#endif

#endif /* SYS_PIN_DEF_H */
