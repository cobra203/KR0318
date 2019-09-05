#ifndef _BATTERY_LED_H_
#define _BATTERY_LED_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include <charge_pal_sys.h>
#include <stm32_led.h>

#define RED_LED_NUM 1
#define COM_LED_NUM	3
#define TAB_LED_NUM 3

typedef enum led_index_e
{
	LED_ID_RED,
	LED_ID_COM1,
	LED_ID_COM2,
	LED_ID_COM3,
	LED_ID_TAB1,
	LED_ID_TAB2,
	LED_ID_TAB3,
	LED_ID_BUTT,
} LED_INDEX_E;

typedef enum led_status_e
{
    LED_STATUS_CLOSED,
    LED_STATUS_CONNECT,
    LED_STATUS_PAIRING,
} LED_STATUS_E;

typedef struct battery_led_s
{
    CP_SYS_S		*cp_sys;
	uint8_t			task_battery_id;
	uint8_t			task_tablet_id;
    STM32_LED_S     red_led;
    STM32_LED_S     com_led[COM_LED_NUM];
	STM32_LED_S     tab_led[TAB_LED_NUM];
    void            (*set)		(struct battery_led_s *, uint8_t, LED_STATUS_E);
	void			(*bat_restart)	(void);
	void			(*tab_restart)	(void);
} BATTERY_LED_S;


void led_init(CP_SYS_S *cp_sys);

#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_LED_H_ */
