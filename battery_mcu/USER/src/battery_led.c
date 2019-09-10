#include <battery_led.h>
#include <stm32_led.h>
#include <sys_common.h>
#include <debug.h>
#include <stm32_timer.h>

static BATTERY_LED_S leds;

typedef struct led_gpio_pin_s
{
    GPIO_TypeDef    *gpio;
    uint16_t        pin;
} LED_GPIO_PIN_S;

static LED_GPIO_PIN_S led_table_id_transform[RED_LED_NUM + COM_LED_NUM + TAB_LED_NUM] = 
{
    {LED_GPIO_RED,	LED_PIN_RED},
    {LED_GPIO_COM1,	LED_PIN_COM1},
    {LED_GPIO_COM2,	LED_PIN_COM2},
    {LED_GPIO_COM3,	LED_PIN_COM3},
#if (PLATFORM_TYPE == PLATFORM_KR0302)
	{LED_GPIO_COM4,	LED_PIN_COM4},
    {LED_GPIO_COM5,	LED_PIN_COM5},
#endif
#if FUNC_TABLET
	{LED_GPIO_TAB1, LED_PIN_TAB1},
	{LED_GPIO_TAB2, LED_PIN_TAB2},
	{LED_GPIO_TAB3, LED_PIN_TAB3},
#endif
};

typedef struct led_charge_section_s
{
	uint8_t		min;
	uint8_t		max;
	uint8_t		normal;
	uint8_t		charge;
} LED_CHARGE_SECTION_S;

static LED_CHARGE_SECTION_S led_charge_section_indicate_table[] =
{
#if (PLATFORM_TYPE == PLATFORM_KR0318) || (PLATFORM_TYPE == PLATFORM_KR2109)
	{POWER_CORRECTION_0,	POWER_CORRECTION_5,		LED_ID_RED,		LED_ID_BUTT},
	{POWER_CORRECTION_5,	POWER_CORRECTION_25,	LED_ID_RED,		LED_ID_BUTT},
	{POWER_CORRECTION_25,	POWER_CORRECTION_50,	LED_ID_COM1,	LED_ID_COM1},
	{POWER_CORRECTION_50,	POWER_CORRECTION_75,	LED_ID_COM2,	LED_ID_COM2},
	{POWER_CORRECTION_75,	POWER_CORRECTION_100,	LED_ID_COM3,	LED_ID_COM2},
#elif (PLATFORM_TYPE == PLATFORM_KR0302)
	{POWER_CORRECTION_0,	POWER_CORRECTION_5,		LED_ID_RED,		LED_ID_BUTT},
	{POWER_CORRECTION_5,	POWER_CORRECTION_16,	LED_ID_RED,		LED_ID_BUTT},
	{POWER_CORRECTION_16,	POWER_CORRECTION_33,	LED_ID_COM1,	LED_ID_COM1},
	{POWER_CORRECTION_33,	POWER_CORRECTION_50,	LED_ID_COM2,	LED_ID_COM2},
	{POWER_CORRECTION_50,	POWER_CORRECTION_66,	LED_ID_COM3,	LED_ID_COM3},
	{POWER_CORRECTION_66,	POWER_CORRECTION_83,	LED_ID_COM4,	LED_ID_COM4},
	{POWER_CORRECTION_83,	POWER_CORRECTION_100,	LED_ID_COM5,	LED_ID_COM4},
#endif
};
#define SECTION_NUM	(sizeof(led_charge_section_indicate_table)/sizeof(LED_CHARGE_SECTION_S))

static int _get_charge_led_section(uint8_t power, uint8_t charge)
{
	static int current_section = -1;
	int section_min = 0;
	int section_max = 0;
	int i = 0;

	if(-1 == current_section) {
		for(i = 0; i < SECTION_NUM; i++) {
			if(led_charge_section_indicate_table[i].min <= power
				&& power < led_charge_section_indicate_table[i].max) {
				current_section = i;
				DEBUG("section init=%d, power=%d, min=%d, max=%d", current_section, power,
					led_charge_section_indicate_table[i].min, led_charge_section_indicate_table[i].max);
				return i;
			}
		}
	}

	section_min = led_charge_section_indicate_table[current_section].min;
	section_max = led_charge_section_indicate_table[current_section].max;

	if(charge) {
		if(current_section > 0) {
			section_min -= 3;
		}
	}
	else {
		if(current_section < SECTION_NUM - 1) {
			section_max += 3;
		}

	}

	if(power < section_min && current_section > 0) {
		current_section--;
		return current_section;
	}

	if(power >= section_max && current_section < SECTION_NUM - 1) {
		current_section++;
		return current_section;
	}

	return current_section;
}

static void _led_id_transform(uint8_t id, uint16_t *pin, GPIO_TypeDef **port)
{
    *port   = led_table_id_transform[id].gpio;
    *pin    = led_table_id_transform[id].pin;
}

static void _led_status_set(BATTERY_LED_S *leds, uint8_t idx, LED_STATUS_E mode)
{
    STM32_LED_S *led = (idx == 0) ? &leds->red_led : &leds->com_led[idx - RED_LED_NUM];

    switch(mode) {
    case LED_STATUS_CLOSED:
        led->set(led, 1, 0);
        break;
    case LED_STATUS_CONNECT:
        led->set(led, 0, 1);
        break;
    default:
        break;
    }

    led->doing(led);
}

static void _led_register(uint8_t *task_id, TASK_F task, int delay_ms) {
	timer_task(task_id, TMR_ONCE, delay_ms, 0, task, STM_NULL);
}

static void _led_battery_server_start(void *args);
static void _led_tablet_server_start(void *args);

static void _led_battery_vbat_low(void *args)
{
	leds.set(&leds, LED_ID_RED, LED_STATUS_CONNECT);
	timer_task(&leds.task_battery_id, TMR_ONCE, 50, 0, _led_battery_server_start, STM_NULL);
}

static void _led_battery_change_show_next(void *args)
{
	CP_SYS_S	*cp_sys = leds.cp_sys;
	int	        led_id = (int)args;

	leds.set(&leds, led_id++, LED_STATUS_CONNECT);

	if(cp_sys->sys_status.charge && LED_ID_TAB1 > led_id) {
		timer_task(&leds.task_battery_id, TMR_ONCE, 1000, 0, _led_battery_change_show_next, (void *)led_id);
	}
	else {
		timer_task(&leds.task_battery_id, TMR_ONCE, 1000, 0, _led_battery_server_start, STM_NULL);
	}
}

static void _led_battery_change_show_start(void *args)
{
	CP_SYS_S	*cp_sys = leds.cp_sys;
	int	        led_start_id = (int)args;

	if(cp_sys->sys_status.charge) {
		timer_task(&leds.task_battery_id, TMR_ONCE, 500, 0, _led_battery_change_show_next, (void *)led_start_id);
	}
	else {
		timer_task(&leds.task_battery_id, TMR_ONCE, 0, 0, _led_battery_server_start, STM_NULL);
	}
}

static void _led_battery_power_show(void *args)
{
	CP_SYS_S	*cp_sys = leds.cp_sys;
	int             i = 0;
	int             led_start_id = LED_ID_COM1;

	for(i = 0; i < RED_LED_NUM + COM_LED_NUM; i++) {
		leds.set(&leds, LED_ID_RED + i, LED_STATUS_CLOSED);
    }

	int current_section = _get_charge_led_section(cp_sys->sys_status.power, cp_sys->sys_status.charge);

	if(cp_sys->sys_status.cut_off) {
		current_section = 0;
	}

	if(!cp_sys->sys_status.charge) {
		/* 0% ~ 5% */
		if(0 == current_section) {
			timer_task(&leds.task_battery_id, TMR_ONCE, 200, 0, _led_battery_vbat_low, STM_NULL);
			return;
		}
		/* 5% ~ x% */
		if(1 == current_section) {
			leds.set(&leds, LED_ID_RED, LED_STATUS_CONNECT);
		}
		else {
			for(i = LED_ID_COM1;
				i < led_charge_section_indicate_table[current_section].normal + 1;
				i++) {
				leds.set(&leds, i, LED_STATUS_CONNECT);
			}
		}
	}
	else {
		if(current_section > 1) {
			led_start_id = led_charge_section_indicate_table[current_section].charge + 1;
			for(i = LED_ID_COM1;
				i < led_start_id;
				i++) {
				leds.set(&leds, i, LED_STATUS_CONNECT);
			}
		}
	}

	if(cp_sys->sys_status.charge) {
		timer_task(&leds.task_battery_id, TMR_ONCE, 500, 0, _led_battery_change_show_start, (void *)led_start_id);
	}
	else {
		timer_task(&leds.task_battery_id, TMR_ONCE, 1000, 0, _led_battery_server_start, STM_NULL);
	}
}

static void _led_battery_server_start(void *args)
{
	timer_task(&leds.task_battery_id, TMR_ONCE, 0, 0, _led_battery_power_show, STM_NULL);
}

static void _led_tablet_change_show_next(void *args)
{
	CP_SYS_S	*cp_sys = leds.cp_sys;
	int             led_id = (int)args;

	leds.set(&leds, led_id++, LED_STATUS_CONNECT);

	if(cp_sys->sys_status.tablet && LED_ID_BUTT > led_id) {
		timer_task(&leds.task_tablet_id, TMR_ONCE, 1000, 0, _led_tablet_change_show_next, (void *)led_id);
	}
	else {
		timer_task(&leds.task_tablet_id, TMR_ONCE, 1000, 0, _led_tablet_server_start, STM_NULL);
	}
}

static void _led_tablet_change_show_start(void *args)
{
	CP_SYS_S	*cp_sys = leds.cp_sys;
	LED_INDEX_E	led_start_id = LED_ID_TAB1;
	int i = 0;

	for(i = 0; i < TAB_LED_NUM; i++) {
		leds.set(&leds, LED_ID_TAB1 + i, LED_STATUS_CLOSED);
    }

	if(cp_sys->sys_status.tablet) {
		timer_task(&leds.task_tablet_id, TMR_ONCE, 1000, 0, _led_tablet_change_show_next, (void *)led_start_id);
	}
	else {
		timer_task(&leds.task_tablet_id, TMR_ONCE, 0, 0, _led_tablet_server_start, STM_NULL);
	}
}

static void _led_tablet_server_start(void *args)
{
	CP_SYS_S *cp_sys = leds.cp_sys;

	if(cp_sys->sys_status.tablet) {
		timer_task(&leds.task_battery_id, TMR_ONCE, 0, 0, _led_tablet_change_show_start, STM_NULL);
	}
	else {
		timer_task(&leds.task_battery_id, TMR_ONCE, 1000, 0, _led_tablet_server_start, STM_NULL);
	}
}

static void _led_battery_restart(void)
{
	int i = 0;

	timer_free(&leds.task_battery_id);
	for(i = 0; i < RED_LED_NUM + COM_LED_NUM; i++) {
		leds.set(&leds, LED_ID_RED + i, LED_STATUS_CLOSED);
    }
	_led_register(&leds.task_battery_id, _led_battery_server_start, 0);
}

static void _led_tablet_restart(void)
{
	int i = 0;

	timer_free(&leds.task_tablet_id);
	for(i = 0; i < TAB_LED_NUM; i++) {
		leds.set(&leds, LED_ID_TAB1 + i, LED_STATUS_CLOSED);
    }
	_led_register(&leds.task_tablet_id, _led_tablet_server_start, 0);
}

void led_init(CP_SYS_S *cp_sys)
{
    int i = 0;
	GPIO_TypeDef	*port;
	uint16_t		pin;

    leds.cp_sys				= cp_sys;
    cp_sys->leds			= &leds;
	leds.set        		= _led_status_set;
	leds.bat_restart		= _led_battery_restart;
	leds.tab_restart		= _led_tablet_restart;
	leds.task_battery_id	= TIMERS_NUM;
	leds.task_tablet_id		= TIMERS_NUM;

	_led_id_transform(LED_ID_RED, &pin, &port);
    leds.red_led.init = stm32_led_init;
    leds.red_led.init(&leds.red_led, pin, port);
	leds.set(&leds, 0, LED_STATUS_CLOSED);
    
    for(i = 0; i < COM_LED_NUM; i++) {
		_led_id_transform(RED_LED_NUM + i, &pin, &port);
        leds.com_led[i].init = stm32_led_init;
        leds.com_led[i].init(&leds.com_led[i], pin, port);
		leds.set(&leds, RED_LED_NUM + i, LED_STATUS_CLOSED);
    }

#if FUNC_TABLET
	for(i = 0; i < TAB_LED_NUM; i++) {
		_led_id_transform(RED_LED_NUM + COM_LED_NUM + i, &pin, &port);
        leds.tab_led[i].init = stm32_led_init;
        leds.tab_led[i].init(&leds.tab_led[i], pin, port);
		leds.set(&leds, RED_LED_NUM + COM_LED_NUM + i, LED_STATUS_CLOSED);
    }
#endif

	_led_register(&leds.task_battery_id, _led_battery_server_start, 200);
#if FUNC_TABLET
	_led_register(&leds.task_tablet_id, _led_tablet_server_start, 200);
#endif
}
