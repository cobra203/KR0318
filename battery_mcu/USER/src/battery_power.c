#include <battery_power.h>
#include <debug.h>
#include <stm32_timer.h>
#include <sys_pin_def.h>

static BATTERY_POWER_S power;

static void _power_vbat_get(BATTERY_POWER_S *power)
{
	CP_SYS_S *cp_sys = power->cp_sys;

	int tmp_val = 0;
	int i = 0;
	int times = 1000;
	int	power_val = 0;

	for(i = 0; i < times; i++) {
		power->vbat.conversion(&power->vbat);
		tmp_val += power->vbat.voltage;
	}
	
	power_val = (tmp_val/times - CUT_OFF_VOLTAGE) * 100 / (FULL_VOLTAGE - CUT_OFF_VOLTAGE);

	if(power_val < 0) {
		power->vbat_power = 0;
	}
	else if(power_val > 100) {
		power->vbat_power = 100;
	}
	else {
		power->vbat_power = power_val;
	}

	DEBUG("VOL:%d, PWR:%d, %d\n", tmp_val/times, power_val, power->vbat_power);
	cp_sys->sys_evt.vbat_update = STM_TRUE;
}

static void _power_handle(void *args)
{
	uint8_t task = TIMERS_NUM;

	power.vbat_get(&power);

	timer_task(&task, TMR_ONCE, 1000, 0, _power_handle, STM_NULL);
}

static void _power_register(void)
{
	uint8_t task = TIMERS_NUM;

	timer_task(&task, TMR_ONCE, 0, 0, _power_handle, STM_NULL);
}

void power_init(CP_SYS_S *cp_sys)
{
	power.cp_sys	= cp_sys;
	power.vbat_get	= _power_vbat_get;
	cp_sys->power	= &power;

	power.vbat.init = stm32_adc_init;
    power.vbat.init(&power.vbat, VBAT_PIN, VBAT_GPIO);

	power.vbat_power = 0;

	_power_register();
}

