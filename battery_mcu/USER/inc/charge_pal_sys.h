#ifndef _CHARGE_PAL_SYS_H_
#define _CHARGE_PAL_SYS_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stm32f0xx.h>
#include <sys_common.h>

typedef struct sys_evt_s
{
    uint8_t charge_into			:1;
    uint8_t charge_out_of		:1;
	uint8_t tablet_into			:1;
    uint8_t tablet_out_of		:1;
    uint8_t vbat_update			:1;
    uint8_t						:3;
} SYS_EVT_S;

typedef struct charge_pal_sys_s
{
    SYS_EVT_S               sys_evt;
	struct battery_tablet_s	*tablet;
    struct battery_stat_s	*stat;
    struct battery_led_s    *leds;
    struct battery_power_s	*power;
} CP_SYS_S;

void cp_sys_init(void);


#ifdef __cplusplus
}
#endif

#endif /* _CHARGE_PAL_SYS_H_ */

