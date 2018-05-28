#include <charge_pal_sys.h>
#include <stm32_timer.h>
#include <battery_led.h>
#include <battery_stat.h>
#include <battery_power.h>
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
	uint8_t		task;

	if(STM_TRUE == cp_sys->sys_evt.charge_into) {
		cp_sys->leds->vbat_status.charge = STM_TRUE;
		cp_sys->leds->restart();
		cp_sys->sys_evt.charge_into = STM_FALSE;
	}

	if(STM_TRUE == cp_sys->sys_evt.charge_out_of) {
		cp_sys->leds->vbat_status.charge = STM_FALSE;
		cp_sys->leds->restart();
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
	uint8_t task;

	timer_task(&task, TMR_ONCE, 10, 0, cp_sys_handle, (void *)&cp_sys);
}

void cp_sys_init(void)
{
    timer_init();
	sys_exti_init();
    
    led_init(&cp_sys);
    stat_init(&cp_sys);
	power_init(&cp_sys);

	cp_sys_register();
}

