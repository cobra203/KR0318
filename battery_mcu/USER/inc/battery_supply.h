#ifndef _BATTERY_SUPPLY_H_
#define _BATTERY_SUPPLY_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include <button.h>
#include <charge_pal_sys.h>

typedef struct battery_supply_s
{
    CP_SYS_S		*cp_sys;
	STM_BOOL		into;
	uint8_t			task;
    BUTTON_S        detect;
    void            (*process)      (struct battery_supply_s *);
    void            (*set_active)   (struct battery_supply_s *);
} BATTERY_SUPPLY_S;

void supply_itc(void);
void supply_init(CP_SYS_S *sys_status);


#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_SUPPLY_H_ */

