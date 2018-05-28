#include <stm32_led.h>
#include <stm32_timer.h>

static void _led_bright(STM32_LED_S *led)
{
    GPIO_ResetBits(led->port, led->pin);
    led->status = 1;
}

static void _led_dark(STM32_LED_S *led)
{
    GPIO_SetBits(led->port, led->pin);
    led->status = 0;
}

static void _led_set_attr(STM32_LED_S *led, uint16_t dark_time, uint16_t bright_time)
{
    led->interval[0]    = dark_time;
    led->interval[1]    = bright_time;
}

static void _led_callback_func(void *args)
{
    STM32_LED_S *led = (STM32_LED_S *)args;

    if(!led->status) {
        led->bright(led);
        timer_set_reload(led->timer, led->interval[0]);
    }
    else {
        led->dark(led);
        timer_set_reload(led->timer, led->interval[1]);
    }
}

static void _led_doing(STM32_LED_S *led)
{
#if 1
    if(led->timer < TIMERS_NUM) {   
        timer_free(&led->timer);
        led->dark(led);
    }
#endif

    if(!led->interval[1]) {
        led->dark(led);
    }
    else if(!led->interval[0]) {
        led->bright(led);
    }
    else { 
        timer_task(&led->timer, TMR_CYCLICITY, led->interval[0], led->interval[1], _led_callback_func, (void *)led);
    }
}

void stm32_led_init(STM32_LED_S *led, uint16_t pin, GPIO_TypeDef *port)
{
    GPIO_InitTypeDef    gpio_struct;
	
	led->pin	= pin;
	led->port	= port;

    gpio_struct.GPIO_Speed  = GPIO_Speed_Level_3;
    gpio_struct.GPIO_Mode   = GPIO_Mode_OUT;
    gpio_struct.GPIO_PuPd   = GPIO_PuPd_UP;
    gpio_struct.GPIO_OType  = GPIO_OType_OD;
    gpio_struct.GPIO_Pin    = led->pin;
    GPIO_Init(led->port, &gpio_struct);
    

    led->set    = _led_set_attr;
    led->doing  = _led_doing;
    led->dark   = _led_dark;
    led->bright = _led_bright;

    led->timer  = TIMERS_NUM;
    led->status = 0;
}


