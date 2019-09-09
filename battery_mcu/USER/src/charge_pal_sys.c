#include <charge_pal_sys.h>
#include <stm32_timer.h>
#include <battery_led.h>
#include <battery_stat.h>
#include <battery_tablet.h>
#include <battery_power.h>
#include <battery_supply.h>
#include <sys_common.h>
#include <debug.h>

static CP_SYS_S cp_sys;

static void sys_exti_init(void)
{
    EXTI_InitTypeDef    exti_cfg = {0};
    NVIC_InitTypeDef    nvic_cfg;
    
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource2);
    exti_cfg.EXTI_Line      = EXTI_Line2;
    exti_cfg.EXTI_Mode      = EXTI_Mode_Interrupt;
    exti_cfg.EXTI_Trigger   = EXTI_Trigger_Rising_Falling;
    exti_cfg.EXTI_LineCmd   = ENABLE;
    EXTI_Init(&exti_cfg);

#if FUNC_TABLET
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);
    exti_cfg.EXTI_Line      = EXTI_Line8;
    exti_cfg.EXTI_Mode      = EXTI_Mode_Interrupt;
    exti_cfg.EXTI_Trigger   = EXTI_Trigger_Rising_Falling;
    exti_cfg.EXTI_LineCmd   = ENABLE;
    EXTI_Init(&exti_cfg);
#endif

#if FUNC_SUPPLY
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, SUPPLY_EXIT_SOURCE);
    exti_cfg.EXTI_Line      = SUPPLY_EXTI_LINE;
    exti_cfg.EXTI_Mode      = EXTI_Mode_Interrupt;
    exti_cfg.EXTI_Trigger   = EXTI_Trigger_Rising_Falling;
    exti_cfg.EXTI_LineCmd   = ENABLE;
    EXTI_Init(&exti_cfg);
#endif

    nvic_cfg.NVIC_IRQChannelPriority    = 3;
    nvic_cfg.NVIC_IRQChannel            = EXTI2_3_IRQn;
    nvic_cfg.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&nvic_cfg);

    nvic_cfg.NVIC_IRQChannelPriority    = 1;
    nvic_cfg.NVIC_IRQChannel            = EXTI4_15_IRQn;
    nvic_cfg.NVIC_IRQChannelCmd         = ENABLE;
    NVIC_Init(&nvic_cfg);
}

void cp_sys_handle(void *args)
{
	CP_SYS_S	*cp_sys = (CP_SYS_S *)args;
	uint8_t		task = TIMERS_NUM;

	/* 1. supply event */
	if(cp_sys->supply) {
		if(STM_TRUE == cp_sys->sys_evt.supply_updata) {
			/* ignore when sys_status had cut_off */
			if(STM_FALSE == cp_sys->sys_status.cut_off) {
#if (SUPPLY_FUNC_MODE == SUPPLY_MODE_BUTTON)
				cp_sys->supply->handle(SUPPLY_EVENT_SUPPLY_SWTICH);
#elif (SUPPLY_FUNC_MODE == SUPPLY_MODE_DETECT)
				if(STM_TRUE == cp_sys->supply->allowable) {
					cp_sys->supply->handle(SUPPLY_EVENT_SUPPLY_ENABLE_DELAY);
				}
				else {
					cp_sys->supply->handle(SUPPLY_EVENT_SUPPLY_DISABLE);
				}
#endif
			}
			cp_sys->sys_evt.supply_updata = STM_FALSE;
		}
	}
	
	/* 1. tablet event */
	if(cp_sys->tablet) {
		if(STM_TRUE == cp_sys->sys_evt.tablet_into) {
			cp_sys->sys_status.tablet = STM_TRUE;
			cp_sys->leds->tab_restart();
			cp_sys->sys_evt.tablet_into = STM_FALSE;
		}

		if(STM_TRUE == cp_sys->sys_evt.tablet_out_of) {
			cp_sys->sys_status.tablet = STM_FALSE;
			cp_sys->leds->tab_restart();
			cp_sys->sys_evt.tablet_out_of = STM_FALSE;
		}
	}

	/* 2. charge event */
	if(STM_TRUE == cp_sys->sys_evt.charge_into) {
		cp_sys->sys_status.charge = STM_TRUE;
		if(STM_TRUE == cp_sys->sys_status.cut_off) {
			cp_sys->sys_status.cut_off = STM_FALSE;
			if(STM_TRUE == cp_sys->supply->allowable) {
				cp_sys->supply->handle(SUPPLY_EVENT_SUPPLY_ENABLE);
			}
		}
		cp_sys->leds->bat_restart();
		cp_sys->sys_evt.charge_into = STM_FALSE;
	}

	if(STM_TRUE == cp_sys->sys_evt.charge_out_of) {
		cp_sys->sys_status.charge = STM_FALSE;
		cp_sys->leds->bat_restart();
		cp_sys->sys_evt.charge_out_of = STM_FALSE;
	}

	/* 3. power event */
	if(STM_TRUE == cp_sys->sys_evt.vbat_update) {
		cp_sys->sys_status.power = cp_sys->power->vbat_power;
#if (SUPPLY_FUNC_MODE == SUPPLY_MODE_DETECT)
		if(STM_FALSE == cp_sys->sys_status.charge
				&& cp_sys->sys_status.power <= POWER_CORRECTION_0
				&& STM_FALSE == cp_sys->sys_status.cut_off) {
			cp_sys->sys_status.cut_off = STM_TRUE;
			cp_sys->supply->handle(SUPPLY_EVENT_SUPPLY_DISABLE);
		}
#endif
		cp_sys->sys_evt.vbat_update = STM_FALSE;
	}
	
	timer_task(&task, TMR_ONCE, 10, 0, cp_sys_handle, (void *)cp_sys);
}

void cp_sys_register(void)
{
	uint8_t task = TIMERS_NUM;

	timer_task(&task, TMR_ONCE, 10, 0, cp_sys_handle, (void *)&cp_sys);
}

void cp_sys_init(void)
{
    timer_init();
	sys_exti_init();
    
    led_init(&cp_sys);
    stat_init(&cp_sys);
#if FUNC_TABLET
	tablet_init(&cp_sys);
#endif
	power_init(&cp_sys);
#if FUNC_SUPPLY
	supply_init(&cp_sys);
#endif

	cp_sys_register();
}

