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

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource8);
    exti_cfg.EXTI_Line      = EXTI_Line8;
    exti_cfg.EXTI_Mode      = EXTI_Mode_Interrupt;
    exti_cfg.EXTI_Trigger   = EXTI_Trigger_Rising_Falling;
    exti_cfg.EXTI_LineCmd   = ENABLE;
    EXTI_Init(&exti_cfg);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, SUPPLY_EXIT_SOURCE);
    exti_cfg.EXTI_Line      = SUPPLY_EXTI_LINE;
    exti_cfg.EXTI_Mode      = EXTI_Mode_Interrupt;
    exti_cfg.EXTI_Trigger   = EXTI_Trigger_Rising_Falling;
    exti_cfg.EXTI_LineCmd   = ENABLE;
    EXTI_Init(&exti_cfg);
   
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

	if(STM_TRUE == cp_sys->sys_evt.tablet_into) {
#if (LED_CLOSE_WHEN_OUT_OF_CHARGE == STM_TRUE)
		cp_sys->leds->restart();
#endif
		cp_sys->sys_evt.tablet_into = STM_FALSE;
	}

	if(STM_TRUE == cp_sys->sys_evt.tablet_out_of) {
#if (LED_CLOSE_WHEN_OUT_OF_CHARGE == STM_TRUE)
		cp_sys->leds->stop();
#endif
		cp_sys->sys_evt.tablet_out_of = STM_FALSE;
	}

	if(STM_TRUE == cp_sys->sys_evt.charge_into) {
		cp_sys->leds->vbat_status.charge = STM_TRUE;
#if (LED_CLOSE_WHEN_OUT_OF_CHARGE == STM_TRUE)
		if(STM_TRUE == cp_sys->tablet->work) {
			cp_sys->leds->restart();
		}
#else
		cp_sys->leds->restart();
#endif
		cp_sys->sys_evt.charge_into = STM_FALSE;
	}

	if(STM_TRUE == cp_sys->sys_evt.charge_out_of) {
		cp_sys->leds->vbat_status.charge = STM_FALSE;
#if (LED_CLOSE_WHEN_OUT_OF_CHARGE == STM_TRUE)
		if(STM_TRUE == cp_sys->tablet->work) {
			cp_sys->leds->restart();
		}
#else
		cp_sys->leds->restart();
#endif
		cp_sys->sys_evt.charge_out_of = STM_FALSE;
	}

	if(STM_TRUE == cp_sys->sys_evt.vbat_update) {
		cp_sys->leds->vbat_status.power = cp_sys->power->vbat_power;
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
	tablet_init(&cp_sys);
	power_init(&cp_sys);
	supply_init(&cp_sys);

	cp_sys_register();
}

