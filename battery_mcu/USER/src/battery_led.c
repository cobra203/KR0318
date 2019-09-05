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
    {LED_GPIO_COM2,	LED_PIN_COM3},
    {LED_GPIO_TAB1,	LED_PIN_TAB1},
    {LED_GPIO_TAB2,	LED_PIN_TAB2},
    {LED_GPIO_TAB3,	LED_PIN_TAB3},
};

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

static void _led_register(uint8_t *task_id, TASK_F task) {
	timer_task(task_id, TMR_ONCE, 0, 0, task, STM_NULL);
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
	LED_INDEX_E	led_id = (int)args;

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
	uint16_t	power	= cp_sys->sys_status.power;
	LED_INDEX_E	led_start_id = LED_ID_COM1;

	if(power >= POWER_CORRECTION_25) {
		led_start_id = LED_ID_COM2;
	}
	if(power >= POWER_CORRECTION_50) {
		led_start_id = LED_ID_COM3;
	}

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
	uint16_t	power	= cp_sys->sys_status.power;

	leds.set(&leds, LED_ID_RED, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM1, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM2, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM3, LED_STATUS_CLOSED);

	/* 0% ~ 5% */
	if(power <= POWER_CORRECTION_5 && !cp_sys->sys_status.charge) {
		timer_task(&leds.task_battery_id, TMR_ONCE, 200, 0, _led_battery_vbat_low, STM_NULL);
		return;
	}

	/* 5% ~ 25% */
	if(power < POWER_CORRECTION_25 && !cp_sys->sys_status.charge) {
		leds.set(&leds, LED_ID_RED, LED_STATUS_CONNECT);
	}

	/* 25% ~ 50% */
	if(power >= POWER_CORRECTION_25) {
		leds.set(&leds, LED_ID_COM1, LED_STATUS_CONNECT);
	}

	/* 50% ~ 75% */
	if(power >= POWER_CORRECTION_50) {
		leds.set(&leds, LED_ID_COM2, LED_STATUS_CONNECT);
	}

	/* 75% ~ 100% */
	if(power >= POWER_CORRECTION_75 && !cp_sys->sys_status.charge) {
		leds.set(&leds, LED_ID_COM3, LED_STATUS_CONNECT);
	}

	if(cp_sys->sys_status.charge) {
		timer_task(&leds.task_battery_id, TMR_ONCE, 500, 0, _led_battery_change_show_start, STM_NULL);
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
	LED_INDEX_E	led_id = (int)args;

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

	leds.set(&leds, LED_ID_TAB1, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_TAB2, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_TAB3, LED_STATUS_CLOSED);

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
	CP_SYS_S	*cp_sys = leds.cp_sys;

	timer_free(&leds.task_battery_id);
	leds.set(&leds, LED_ID_RED, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM1, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM2, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM3, LED_STATUS_CLOSED);
	_led_register(&leds.task_battery_id, _led_battery_server_start);
}

static void _led_tablet_restart(void)
{
	CP_SYS_S	*cp_sys = leds.cp_sys;

	timer_free(&leds.task_tablet_id);
	leds.set(&leds, LED_ID_TAB1, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_TAB2, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_TAB3, LED_STATUS_CLOSED);
	_led_register(&leds.task_tablet_id, _led_tablet_server_start);
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

	for(i = 0; i < TAB_LED_NUM; i++) {
		_led_id_transform(RED_LED_NUM + COM_LED_NUM + i, &pin, &port);
        leds.tab_led[i].init = stm32_led_init;
        leds.tab_led[i].init(&leds.tab_led[i], pin, port);
		leds.set(&leds, RED_LED_NUM + COM_LED_NUM + i, LED_STATUS_CLOSED);
    }

	_led_register(&leds.task_battery_id, _led_battery_server_start);
	_led_register(&leds.task_tablet_id, _led_tablet_server_start);
}
