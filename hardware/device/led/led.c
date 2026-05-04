#include "led.h"

#include "delay.h"
#include "types.h"
#include "sfrdef.h"

#define flash_timer 3  // 闪烁次数

static inline void LED_ON(void)     { GPIOC->BRR  |= (0x01 << LED); }
static inline void LED_OFF(void)    { GPIOC->BSRR |= (0x01 << LED); }
static inline void LED_Toggle(void) { GPIOC->ODR  ^= (0x01 << LED); }

typedef enum {
	LED_STATE_ON,
	LED_STATE_OFF,
	LED_STATE_FLASH_1,
	LED_STATE_FLASH_2,
	LED_STATE_FLASH_3,
	LED_STATE_FLASH_4
} LED_STATE;

static LED_STATE ledState = LED_STATE_OFF;

static __bool loop = False;   // 是否循环

static void LED_Update(void) {
	static int8_t var = 0;  // 内部变量
	
	switch (ledState) {
		case LED_STATE_ON:
			LED_ON();
			break;
		case LED_STATE_OFF:
			LED_OFF();
			break;
		case LED_STATE_FLASH_1:
			if (loop) {
				if (var < 0) {
					LED_OFF();
					ledState = LED_STATE_OFF;
					var = 0;
					loop = False;
					break;
				} else if (var == 0) { var = (flash_timer * 4 - 1); }  // ...,7,5,3,1,-1  本次循环结束，如果var=0，则代表刚开始循环
				
				LED_Toggle();
				var -= 2;
				delay_nb(100, LED_Update);
			} else {
				LED_OFF();
				ledState = LED_STATE_OFF;
				var = 0;
			}
			break;
		case LED_STATE_FLASH_2:
			if (loop) {
				LED_Toggle();
				delay_nb(500, LED_Update);
			} else {
				LED_OFF();
				ledState = LED_STATE_OFF;
			}
			break;
		case LED_STATE_FLASH_3:
			if (loop) {
				LED_Toggle();
				delay_nb(300, LED_Update);
			} else {
				LED_OFF();
				ledState = LED_STATE_OFF;
			}
			break;
		case LED_STATE_FLASH_4:
			if (loop) {
				LED_Toggle();
				delay_nb(200, LED_Update);
			} else {
				LED_OFF();
				ledState = LED_STATE_OFF;
			}
			break;
	}
}

/**
  * @param type:  0 = 灭
				  1 = 亮
				  2 = 快闪     flash_timer次
				  3 = 闪烁     0.5s/次
				  4 = 闪烁     0.3s/次
				  5 = 闪烁     0.2s/次
  * @note 翻转LED不是原子操作，须知
  */
void LED_Control(uint8_t type) {
	switch (type) {
		case 0:
			loop = False;
			ledState = LED_STATE_OFF;
			break;
		case 1:
			loop = False;
			ledState = LED_STATE_ON;
			break;
		case 2:
			loop = True;
			ledState = LED_STATE_FLASH_1;
			break;
		case 3:
			loop = True;
			ledState = LED_STATE_FLASH_2;
			break;
		case 4:
			loop = True;
			ledState = LED_STATE_FLASH_3;
			break;
		case 5:
			loop = True;
			ledState = LED_STATE_FLASH_4;
			break;
		default:
			loop = False;
			ledState = LED_STATE_OFF;
			break;
	}
	LED_Update();
}

