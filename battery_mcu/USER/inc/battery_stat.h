#ifndef _BATTERY_STAT_H_
#define _BATTERY_STAT_H_

#ifdef __cplusplus
 extern "C" {
#endif 

#include <button.h>
#include <charge_pal_sys.h>

typedef struct battery_stat_s
{
    CP_SYS_S		*cp_sys;
    BUTTON_S        stat;
    void            (*process)      (struct battery_stat_s *);
    void            (*set_active)   (struct battery_stat_s *);
} BATTERY_STAT_S;

void stat_itc(void);
void stat_init(CP_SYS_S *sys_status);


#ifdef __cplusplus
}
#endif

#endif /* _BATTERY_H_ */
