#ifndef _BATTERY_TABLET_H_
#define _BATTERY_TABLET_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include <button.h>
#include <charge_pal_sys.h>

typedef struct battery_tablet_s
{
    CP_SYS_S		*cp_sys;
    BUTTON_S        tablet;
	STM_BOOL		work;
    void            (*process)      (struct battery_tablet_s *);
} BATTERY_TABLET_S;

void tablet_itc(void);
void tablet_init(CP_SYS_S *sys_status);


#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_TABLET_H_ */

