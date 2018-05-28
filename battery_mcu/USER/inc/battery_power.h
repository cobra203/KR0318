#ifndef _BATTERY_POWER_H_
#define _BATTERY_POWER_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include <charge_pal_sys.h>
#include <stm32_adc.h>

typedef struct battery_power_s
{
    CP_SYS_S	*cp_sys;
	uint8_t		vbat_power;
	STM32_ADC_S	vbat;
	void		(*vbat_get)		(struct battery_power_s *);
} BATTERY_POWER_S;

void power_init(CP_SYS_S *cp_sys);


#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_H_ */

