#ifndef SYS_COMMON_H
#define SYS_COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif

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

#define		CUT_OFF_VOLTAGE	7400/3
#define		FULL_VOLTAGE	8400/3

typedef enum battery_led_type_e
{
    LED_TYPE_RED = 0,  
    LED_TYPE_COM,
    LED_TYPE_BUTT,
} BATTERY_LED_TYPE_E;

#ifdef __cplusplus
}
#endif

#endif /* SYS_COMMON_H */

