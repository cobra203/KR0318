#ifndef SYS_PIN_DEF_H
#define SYS_PIN_DEF_H

#ifdef __cplusplus
 extern "C" {
#endif

/*************** branch item define start ***********/
#define SUPPLY_MODE_DETECT	0
#define SUPPLY_MODE_BUTTON	1

#define PLATFORM_KR0318		0
#define PLATFORM_KR0302		1

#define EXTI_RANGE_2_3		0
#define EXTI_RANGE_4_15		1
/*************** branch item define end *************/

#define PLATFORM_TYPE		PLATFORM_KR0302

#define STAT_PIN			GPIO_Pin_2      /* fixed */
#define STAT_GPIO           GPIOA

#define VBAT_PIN			GPIO_Pin_0
#define VBAT_GPIO			GPIOA

#define TABLET_PIN			GPIO_Pin_8
#define TABLET_GPIO			GPIOA

#if (PLATFORM_TYPE == PLATFORM_KR0318)
/* supply define */
#define SUPPLY_DET_PIN		GPIO_Pin_12
#define SUPPLY_PIN			GPIO_Pin_1

#define SUPPLY_EXIT_SOURCE	EXTI_PinSource12
#define SUPPLY_EXTI_LINE	EXTI_Line12
#define SUPPLY_EXTI_RANGE	EXTI_RANGE_4_15

#define SUPPLY_DELAY_MS		3000
#define SUPPLY_FUNC_MODE	SUPPLY_MODE_DETECT

#elif (PLATFORM_TYPE == PLATFORM_KR0302)
/* supply define */
#define SUPPLY_DET_PIN		GPIO_Pin_12
#define SUPPLY_PIN			GPIO_Pin_1

#define SUPPLY_EXIT_SOURCE	EXTI_PinSource12
#define SUPPLY_EXTI_LINE	EXTI_Line12
#define SUPPLY_EXTI_RANGE	EXTI_RANGE_4_15

#define SUPPLY_DELAY_MS		3000
#define SUPPLY_FUNC_MODE	SUPPLY_MODE_DETECT

#endif

#define SUPPLY_DET_GPIO		GPIOA
#define SUPPLY_GPIO			GPIOA

#if (PLATFORM_TYPE == PLATFORM_KR0318)
#define LED_PIN_RED			GPIO_Pin_15
#define LED_PIN_COM1        GPIO_Pin_10
#define LED_PIN_COM2        GPIO_Pin_11
#define LED_PIN_COM3        GPIO_Pin_9
#define LED_PIN_TAB1		GPIO_Pin_4
#define LED_PIN_TAB2		GPIO_Pin_5
#define LED_PIN_TAB3		GPIO_Pin_6
#elif (PLATFORM_TYPE == PLATFORM_KR0302)
#define LED_PIN_RED			GPIO_Pin_15
#define LED_PIN_COM1        GPIO_Pin_10
#define LED_PIN_COM2        GPIO_Pin_11
#define LED_PIN_COM3        GPIO_Pin_9
#define LED_PIN_COM4		GPIO_Pin_5
#define LED_PIN_COM5		GPIO_Pin_4
#endif

#define LED_GPIO_RED		GPIOA
#define LED_GPIO_COM1       GPIOA
#define LED_GPIO_COM2       GPIOA
#define LED_GPIO_COM3       GPIOA
#define LED_GPIO_COM4       GPIOA
#define LED_GPIO_COM5       GPIOA
#define LED_GPIO_TAB1		GPIOA
#define LED_GPIO_TAB2		GPIOA
#define LED_GPIO_TAB3		GPIOA


#ifdef __cplusplus
}
#endif

#endif /* SYS_PIN_DEF_H */
