#include <stm32f0xx_gpio.h>

#include <battery_tablet.h>
#include <debug.h>
#include <sys_common.h>
#include <stm32_timer.h>

static BATTERY_TABLET_S	battery_tablet;

static void _tablet_process(BATTERY_TABLET_S *tablet)
{
    CP_SYS_S *cp_sys = tablet->cp_sys;

    if(tablet->tablet.state.avtice) {

		switch(tablet->tablet.state.effective) {
		case ECT_LOOSE:
			DEBUG("tablet:ect[%d]\n", tablet->tablet.state.effective);
			cp_sys->sys_evt.tablet_out_of = STM_TRUE;
			battery_tablet.work = STM_FALSE;
			break;
		case ECT_PRESSED:

			DEBUG("tablet:ect[%d]\n", tablet->tablet.state.effective);
			cp_sys->sys_evt.tablet_into = STM_TRUE;
			battery_tablet.work = STM_TRUE;
			break;
		default:
			break;
		}
		tablet->tablet.state.avtice = 0;/* clear status flag */
    }
}

static void _tablet_server_start(void *args)
{
	uint8_t task = TIMERS_NUM;

	battery_tablet.tablet.check_active(&battery_tablet.tablet);
	battery_tablet.process(&battery_tablet);

	timer_task(&task, TMR_ONCE, 10, 0, _tablet_server_start, STM_NULL);
}

static void _tablet_register(void)
{
	uint8_t task = TIMERS_NUM;
	timer_task(&task, TMR_ONCE, 0, 0, _tablet_server_start, STM_NULL);
}

static void _tablet_work_status_init(void)
{
	CP_SYS_S *cp_sys = battery_tablet.cp_sys;

	battery_tablet.work = GPIO_ReadInputData(TABLET_GPIO) & TABLET_PIN ? 1 : 0;

	if(STM_TRUE == battery_tablet.work) {
		cp_sys->sys_evt.tablet_into = STM_TRUE;
	}
	else {
		cp_sys->sys_evt.tablet_out_of = STM_TRUE;
	}
}

void tablet_init(CP_SYS_S *cp_sys)
{
    GPIO_InitTypeDef    init_struct = {0};

    battery_tablet.cp_sys		= cp_sys;

    init_struct.GPIO_Mode   = GPIO_Mode_IN;
    init_struct.GPIO_PuPd   = GPIO_PuPd_UP;
    init_struct.GPIO_Speed  = GPIO_Speed_Level_3;
    init_struct.GPIO_Pin    = TABLET_PIN;
    GPIO_Init(TABLET_GPIO, &init_struct);

    battery_tablet.tablet.check_active      = button_check_active;
    battery_tablet.tablet.interval.shake    = 3;
    battery_tablet.tablet.interval.pressed  = 20;
    battery_tablet.tablet.interval.focused  = 100;

    battery_tablet.process	= _tablet_process;
    cp_sys->tablet			= &battery_tablet;

	_tablet_work_status_init();
	_tablet_register();
}


void tablet_itc(void)
{
    uint16_t gpio_vol = GPIO_ReadInputData(TABLET_GPIO);

    battery_tablet.tablet.state.press = gpio_vol & TABLET_PIN ? 1 : 0;
}

