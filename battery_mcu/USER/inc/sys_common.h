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
#define		CUT_OFF_VOLTAGE	70000/57
#define		FULL_VOLTAGE	84000/57
#else
#define		CUT_OFF_VOLTAGE	100000/57
#define		FULL_VOLTAGE	126000/57
#endif

#define		CHARGE_VOLTAGE_DETECT_ENABLE		STM_FALSE
#define		LED_CLOSE_WHEN_OUT_OF_CHARGE		STM_FALSE

#ifdef __cplusplus
}
#endif

#endif /* SYS_COMMON_H */

