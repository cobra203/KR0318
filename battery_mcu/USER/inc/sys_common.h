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

#if (PLATFORM_TYPE == PLATFORM_KR0302)
#define		CUT_OFF_VOLTAGE		(70000/57)
#define		FULL_VOLTAGE		(84000/57)
#else
#define		CUT_OFF_VOLTAGE		(96000/57)
#define		FULL_VOLTAGE		(126000/57)
#endif

#define		CORRECTION_VOLTAGE_TABLET	(5000/57)
#define		CORRECTION_VOLTAGE_CHARGE	(4000/57)
#define		NINETY_RATIO				5
#define		NINETY_VOLTAGE				(FULL_VOLTAGE -  CORRECTION_VOLTAGE_CHARGE/NINETY_RATIO)

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
#else
#define		POWER_CORRECTION_0		0
#define		POWER_CORRECTION_5		5
#define		POWER_CORRECTION_16		10
#define		POWER_CORRECTION_33		20
#define		POWER_CORRECTION_50		40
#define		POWER_CORRECTION_66		60
#define		POWER_CORRECTION_83		80
#endif

#if (PLATFORM_TYPE == PLATFORM_KR0318)
#define RED_LED_NUM 1
#define COM_LED_NUM	3
#define TAB_LED_NUM 3
#else
#define RED_LED_NUM 1
#define COM_LED_NUM	5
#define TAB_LED_NUM 0
#endif

#ifdef __cplusplus
}
#endif

#endif /* SYS_COMMON_H */

