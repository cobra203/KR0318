#include <stm32f0xx_gpio.h>

#include <battery_stat.h>
#include <debug.h>
#include <sys_pin_def.h>
#include <sys_common.h>
#include <battery_led.h>
#include <stm32_timer.h>


static BATTERY_STAT_S	battery_pair;

static void _stat_process(BATTERY_STAT_S *stat)
{
    CP_SYS_S *cp_sys = stat->cp_sys;
    
    if(stat->stat.state.avtice) {
        
		switch(stat->stat.state.effective) {
		case ECT_LOOSE:
			DEBUG("ect[%d]\n", stat->stat.state.effective);
			cp_sys->sys_evt.charge_out_of = STM_TRUE;
			break;
		case ECT_PRESSED:
			DEBUG("ect[%d]\n", stat->stat.state.effective);
			cp_sys->sys_evt.charge_into = STM_TRUE;
			break;
		default:
			break;
		}
		stat->stat.state.avtice = 0;/* clear status flag */
    }
}

static void _stat_server_start(void *args)
{
	uint8_t task;
	
	battery_pair.stat.check_active(&battery_pair.stat);
    battery_pair.process(&battery_pair);
	
	timer_task(&task, TMR_ONCE, 10, 0, _stat_server_start, STM_NULL);
}

static void _stat_register(void)
{
	uint8_t task;
	timer_task(&task, TMR_ONCE, 0, 0, _stat_server_start, STM_NULL);
}

void stat_init(CP_SYS_S *cp_sys)
{ 
    GPIO_InitTypeDef    init_struct = {0};

    battery_pair.cp_sys		= cp_sys;

    init_struct.GPIO_Mode   = GPIO_Mode_IN;
    init_struct.GPIO_PuPd   = GPIO_PuPd_UP;
    init_struct.GPIO_Speed  = GPIO_Speed_Level_3;
    init_struct.GPIO_Pin    = STAT_PIN;
    GPIO_Init(STAT_GPIO, &init_struct);

    battery_pair.stat.check_active      = button_check_active;
    battery_pair.stat.interval.shake    = 3;
    battery_pair.stat.interval.pressed  = 20;
    battery_pair.stat.interval.focused  = 100;

    battery_pair.process	= _stat_process;
    cp_sys->stat		= &battery_pair;

	_stat_register();
}


void stat_itc(void)
{
    uint16_t gpio_vol = GPIO_ReadInputData(STAT_GPIO);

    battery_pair.stat.state.press = (~gpio_vol) & STAT_PIN ? 1 : 0;
}
