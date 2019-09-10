#ifndef SYS_COMMON_H
#define SYS_COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif

#include <sys_pin_def.h>

#define     BIT_ISSET(a, s) (((a) >> (s)) & 0x1)
#define     BIT_SET(a, s)   ((a) = (a) | 0x1 << (s))
#define     BIT_CLR(a, s)   ((a) = (a) & ~(0x1 << (s)))

#define     STM_SUCCESS     0
#define     STM_FAILURE     -1

#define     STM_TRUE        1
#define     STM_FALSE       0

#define     STM_ENABLE      STM_TRUE
#define     STM_DISABLE     STM_FALSE

#define		STM_NULL		((void *)0)
#define		STM_BOOL		uint8_t

/* function defined */
#if (PLATFORM_TYPE == PLATFORM_KR0318)
#define		FUNC_TABLET		1
#define		FUNC_SUPPLY		1
#define		CUT_OFF_VOLTAGE				(96000/57)
#define		FULL_VOLTAGE				(126000/57)
#define		CORRECTION_VOLTAGE_TABLET	(5000/57)
#define		CORRECTION_VOLTAGE_CHARGE	(4000/57)

#elif (PLATFORM_TYPE == PLATFORM_KR0302)
#define		FUNC_TABLET		0
#define		FUNC_SUPPLY		1
#define		CUT_OFF_VOLTAGE				(67000/57)
#define		FULL_VOLTAGE				(84000/57)
#define		CORRECTION_VOLTAGE_CHARGE	(2700/57)

#elif (PLATFORM_TYPE == PLATFORM_KR2109)
#define		FUNC_TABLET		0
#define		FUNC_SUPPLY		0
#define		CUT_OFF_VOLTAGE				(320000/133)
#define		FULL_VOLTAGE				(420000/133)
#define		CORRECTION_VOLTAGE_CHARGE	(13000/133)

#endif

#define		CHANGE_NINETY_RATIO			5
#define		CHANGE_NINETY_VOLTAGE		(FULL_VOLTAGE -  CORRECTION_VOLTAGE_CHARGE/CHANGE_NINETY_RATIO)

/********* CORRECTION ***********/
/*	0%	= 10.32V, power = 24	*/
/*	5%	= 10.51V, power = 30	*/
/*	25%	= 10.98V, power = 46	*/
/*	50%	= 11.53V, power = 64	*/
/*	75%	= 12.15V, power = 85	*/
/********************************/

#if (PLATFORM_TYPE == PLATFORM_KR0318)
#define		POWER_CORRECTION_0		24
#define		POWER_CORRECTION_5		30
#define		POWER_CORRECTION_25		46
#define		POWER_CORRECTION_50		64
#define		POWER_CORRECTION_75		85
#elif (PLATFORM_TYPE == PLATFORM_KR0302)
#define		POWER_CORRECTION_0		0
#define		POWER_CORRECTION_5		5
#define		POWER_CORRECTION_16		16
#define		POWER_CORRECTION_33		33
#define		POWER_CORRECTION_50		50
#define		POWER_CORRECTION_66		66
#define		POWER_CORRECTION_83		83
#elif (PLATFORM_TYPE == PLATFORM_KR2109)
#define		POWER_CORRECTION_0		0
#define		POWER_CORRECTION_5		5
#define		POWER_CORRECTION_25		25
#define		POWER_CORRECTION_50		50
#define		POWER_CORRECTION_75		75
#endif
#define		POWER_CORRECTION_100	100

#if (PLATFORM_TYPE == PLATFORM_KR0318)
#define RED_LED_NUM 1
#define COM_LED_NUM	3
#define TAB_LED_NUM 3
#elif (PLATFORM_TYPE == PLATFORM_KR0302)
#define RED_LED_NUM 1
#define COM_LED_NUM	5
#define TAB_LED_NUM 0
#elif (PLATFORM_TYPE == PLATFORM_KR2109)
#define RED_LED_NUM 1
#define COM_LED_NUM	3
#define TAB_LED_NUM 0
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_COMMON_H */

