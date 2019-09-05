#ifndef _BATTERY_SUPPLY_H_
#define _BATTERY_SUPPLY_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include <button.h>
#include <charge_pal_sys.h>

typedef enum battery_supply_event_e
{
	SUPPLY_EVENT_SUPPLY_ENABLE,
	SUPPLY_EVENT_SUPPLY_ENABLE_DELAY,
	SUPPLY_EVENT_SUPPLY_DISABLE,
	SUPPLY_EVENT_SUPPLY_SWTICH
} BATTERY_SUPPLY_EVENT_E;

typedef struct battery_supply_s
{
    CP_SYS_S		*cp_sys;
	STM_BOOL		allowable;
	uint8_t			task;
    BUTTON_S        detect;
    void            (*process)      (struct battery_supply_s *);
    void            (*handle)   	(uint8_t event);
} BATTERY_SUPPLY_S;

void supply_itc(void);
void supply_init(CP_SYS_S *sys_status);


#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_SUPPLY_H_ */

