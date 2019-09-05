#include <stm32f0xx_gpio.h>

#include <battery_supply.h>
#include <debug.h>
#include <sys_common.h>
#include <stm32_timer.h>

static BATTERY_SUPPLY_S	battery_supply;


static void _supply_access(void *args)
{
	int enable = (int)args;
	if(enable) {
		GPIO_SetBits(SUPPLY_GPIO, SUPPLY_PIN);
	}
	else {
		GPIO_ResetBits(SUPPLY_GPIO, SUPPLY_PIN);
	}
}

static void _supply_handle(uint8_t event)
{
	if(TIMERS_NUM != battery_supply.task) {
		timer_free(&battery_supply.task);
	}

	switch(event) {
	case SUPPLY_EVENT_SUPPLY_ENABLE:
		_supply_access((void *)STM_TRUE);
		break;

	case SUPPLY_EVENT_SUPPLY_ENABLE_DELAY:
		timer_task(&battery_supply.task, TMR_ONCE, SUPPLY_DELAY_MS, 0, _supply_access, (void *)STM_TRUE);
		break;

	case SUPPLY_EVENT_SUPPLY_DISABLE:
		_supply_access((void *)STM_FALSE);
		break;

	case SUPPLY_EVENT_SUPPLY_SWTICH:
		battery_supply.allowable = battery_supply.allowable ? STM_FALSE : STM_TRUE;
		if(battery_supply.allowable) {
			_supply_access((void *)STM_TRUE);
		}
		else {
			_supply_access((void *)STM_FALSE);
		}
		break;
	}
}

static void _supply_process(BATTERY_SUPPLY_S *supply)
{
	static int last_status = ECT_LOOSE;
	CP_SYS_S *cp_sys = supply->cp_sys;

    if(supply->detect.state.avtice) {
        
		switch(supply->detect.state.effective) {
		case ECT_LOOSE:
			DEBUG("supply:ect[%d]\n", supply->detect.state.effective);
#if (SUPPLY_FUNC_MODE == SUPPLY_MODE_BUTTON)
			if(last_status == ECT_PRESSED) {
				cp_sys->sys_evt.supply_updata = STM_TRUE;
			}
#elif (SUPPLY_FUNC_MODE == SUPPLY_MODE_DETECT)
			if(STM_TRUE == supply->allowable) {
				supply->allowable = STM_FALSE;
				cp_sys->sys_evt.supply_updata = STM_TRUE;
			}
#endif
			break;
		case ECT_PRESSED:
			DEBUG("supply:ect[%d]\n", supply->detect.state.effective);

#if (SUPPLY_FUNC_MODE == SUPPLY_MODE_DETECT)
			if(STM_FALSE == supply->allowable) {
				supply->allowable = STM_TRUE;
				cp_sys->sys_evt.supply_updata = STM_TRUE;
			}
#endif
			break;
		default:
			break;
		}
		last_status = supply->detect.state.effective;
		supply->detect.state.avtice = 0;/* clear status flag */
    }
}

static void _supply_pin_detect(void)
{
	uint16_t gpio_vol;

	if(battery_supply.detect.detect_count) {
		gpio_vol = GPIO_ReadInputData(SUPPLY_DET_GPIO);
		battery_supply.detect.state.press = (~gpio_vol) & SUPPLY_DET_PIN ? 1 : 0;
		battery_supply.detect.detect_count--;
	}
}

static void _supply_server_start(void *args)
{
	uint8_t task = TIMERS_NUM;

	_supply_pin_detect();
	battery_supply.detect.check_active(&battery_supply.detect);
    battery_supply.process(&battery_supply);
	
	timer_task(&task, TMR_ONCE, 10, 0, _supply_server_start, STM_NULL);
}

static void _supply_register(void)
{
	uint8_t task = TIMERS_NUM;
	timer_task(&task, TMR_ONCE, 0, 0, _supply_server_start, STM_NULL);
}

void supply_itc(void)
{
	battery_supply.detect.detect_count = 10;
}

void supply_init(CP_SYS_S *cp_sys)
{ 
    GPIO_InitTypeDef    init_struct = {0};

    battery_supply.cp_sys		= cp_sys;

    init_struct.GPIO_Mode   = GPIO_Mode_IN;
    init_struct.GPIO_PuPd   = GPIO_PuPd_UP;
    init_struct.GPIO_Speed  = GPIO_Speed_Level_3;
    init_struct.GPIO_Pin    = SUPPLY_DET_PIN;
    GPIO_Init(SUPPLY_DET_GPIO, &init_struct);

    init_struct.GPIO_Mode   = GPIO_Mode_OUT;
    init_struct.GPIO_PuPd   = GPIO_PuPd_DOWN;
	init_struct.GPIO_Speed  = GPIO_Speed_Level_3;
    init_struct.GPIO_OType  = GPIO_OType_PP;
    init_struct.GPIO_Pin    = SUPPLY_PIN;
    GPIO_Init(SUPPLY_GPIO, &init_struct);

	battery_supply.task = TIMERS_NUM;
	battery_supply.allowable = STM_FALSE;
	_supply_access((void *)STM_FALSE);

    battery_supply.detect.check_active      = button_check_active;
    battery_supply.detect.interval.shake    = 3;
    battery_supply.detect.interval.pressed  = 20;
    battery_supply.detect.interval.focused  = 100;

    battery_supply.process	= _supply_process;
	battery_supply.handle	= _supply_handle;
    cp_sys->supply			= &battery_supply;

	supply_itc();
	_supply_register();
}

