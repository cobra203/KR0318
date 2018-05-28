#include <battery_led.h>
#include <stm32_led.h>
#include <sys_pin_def.h>
#include <debug.h>
#include <stm32_timer.h>

static BATTERY_LED_S leds;

typedef struct led_gpio_pin_s
{
    GPIO_TypeDef    *gpio;
    uint16_t        pin;
} LED_GPIO_PIN_S;

static LED_GPIO_PIN_S led_table_id_transform[RED_LED_NUM + COM_LED_NUM] = 
{
    {LED_GPIO_RED,  LED_PIN_RED},
    {LED_GPIO_COM1, LED_PIN_COM1},
    {LED_GPIO_COM2, LED_PIN_COM2},
    {LED_GPIO_COM2, LED_PIN_COM3},
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

static void _led_server_start(void *args);

static void _led_next_step(void *args)
{
	int id = (int)args;

	leds.set(&leds, id++, LED_STATUS_CONNECT);
	if(LED_ID_BUTT == id) {
		timer_task(&leds.task_id, TMR_ONCE, 1000, 0, _led_server_start, (void *)1000);
	}
	else {
		timer_task(&leds.task_id, TMR_ONCE, 1000, 0, _led_next_step, (void *)id);
	}
}

static void _led_vbat_low(void *args)
{
	leds.set(&leds, LED_ID_RED, LED_STATUS_CONNECT);
	timer_task(&leds.task_id, TMR_ONCE, 50, 0, _led_server_start, STM_NULL);
}

static void _led_server_start(void *args)
{
	LED_INDEX_E	next	= LED_ID_COM1;
	int			delay	= (int)args;

	leds.set(&leds, LED_ID_RED, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM1, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM2, LED_STATUS_CLOSED);
	leds.set(&leds, LED_ID_COM3, LED_STATUS_CLOSED);
	
	if(leds.vbat_status.power < 25) {
		if(!leds.vbat_status.charge) {
			timer_task(&leds.task_id, TMR_ONCE, 200, 0, _led_vbat_low, STM_NULL);
			return;
		}
	}
	else {
		leds.set(&leds, LED_ID_COM1, LED_STATUS_CONNECT);
		next++;
	}

	if(leds.vbat_status.power >= 50) {
		leds.set(&leds, LED_ID_COM2, LED_STATUS_CONNECT);
		next++;
	}

	if(leds.vbat_status.power >= 75) {
		if(!leds.vbat_status.charge) {
			leds.set(&leds, LED_ID_COM3, LED_STATUS_CONNECT);
		}
	}

	if(leds.vbat_status.charge) {
		timer_task(&leds.task_id, TMR_ONCE, delay, 0, _led_next_step, (void *)next);
	}
	else {
		timer_task(&leds.task_id, TMR_ONCE, 1000, 0, _led_server_start, STM_NULL);
	}
}

static void _led_register(void) {
	timer_task(&leds.task_id, TMR_ONCE, 0, 0, _led_server_start, STM_NULL);
}

void _led_restart(void)
{
	timer_free(&leds.task_id);
	_led_register();
}

void led_init(CP_SYS_S *cp_sys)
{
    int i = 0;
	GPIO_TypeDef	*port;
	uint16_t		pin;

    leds.cp_sys		= cp_sys;
    cp_sys->leds	= &leds;
	leds.set        = _led_status_set;
	leds.restart	= _led_restart;

	_led_id_transform(LED_ID_RED, &pin, &port);
    leds.red_led.init = stm32_led_init;
    leds.red_led.init(&leds.red_led, pin, port);
	leds.set(&leds, 0, LED_STATUS_CONNECT);
    
    for(i = 0; i < COM_LED_NUM; i++) {
		_led_id_transform(RED_LED_NUM + i, &pin, &port);
        leds.com_led[i].init = stm32_led_init;
        leds.com_led[i].init(&leds.com_led[i], pin, port);
		leds.set(&leds, RED_LED_NUM + i, LED_STATUS_PAIRING);
    }

	_led_register();
}
