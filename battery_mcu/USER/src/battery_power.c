#include <battery_power.h>
#include <debug.h>
#include <stm32_timer.h>
#include <sys_common.h>

static BATTERY_POWER_S power;

static uint8_t _power_voltage_to_level(int16_t voltage)
{
	uint8_t value = 0;
	int	power_val = 0;

	if(!voltage) {
		return value;
	}

	power_val = (voltage - CUT_OFF_VOLTAGE) * 100 / (FULL_VOLTAGE - CUT_OFF_VOLTAGE);

	if(power_val < 0) {
		value = 0;
	}
	else if(power_val > 100) {
		value = 100;
	}
	else {
		value = power_val;
	}

	return value;
}

static void _power_voltage_correction_tablet(int16_t *voltage)
{
#if FUNC_TABLET
	*voltage += CORRECTION_VOLTAGE_TABLET;
#endif
}

static void _power_voltage_correction_charge(int16_t *voltage)
{
	if(FULL_VOLTAGE <= *voltage) {
		*voltage = FULL_VOLTAGE;
		return;
	}
	
	if(CHANGE_NINETY_VOLTAGE < *voltage) {
		*voltage -= (CHANGE_NINETY_RATIO * (FULL_VOLTAGE - *voltage));
		return;
	}

	*voltage -= CORRECTION_VOLTAGE_CHARGE;
}

static int16_t _power_voltage_correction(uint16_t voltage)
{
	CP_SYS_S *cp_sys = power.cp_sys;
	int16_t correction_voltage = voltage;

	if(correction_voltage <= 0) {
		return 0;
	}

	if(cp_sys->sys_status.tablet) {
		_power_voltage_correction_tablet(&correction_voltage);
	}

	if(cp_sys->sys_status.charge) {
		_power_voltage_correction_charge(&correction_voltage);
	}

	return correction_voltage;
}

static void _power_vbat_get(BATTERY_POWER_S *power)
{
	static int16_t voltage = 0;

	CP_SYS_S *cp_sys = power->cp_sys;
	int i = 0;
	int times = 10;
	int tmp_val = 0;

	for(i = 0; i < times; i++) {
		power->vbat.conversion(&power->vbat);
		tmp_val += power->vbat.voltage;
	}

	voltage = _power_voltage_correction((uint16_t)(tmp_val/times));

	power->vbat_power = _power_voltage_to_level(voltage);\

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

