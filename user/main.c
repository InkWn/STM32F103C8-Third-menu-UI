/*******
iid系统暂时有问题，不能使用，不过本程序也用不到

本程序暂不支持动态增删菜单单项
********/

#include "rcc.h"
#include "sys.h"
#include "gpio.h"
#include "nvic.h"
#include "delay.h"

#include "var.h"

#include "led.h"
#include "oled.h"
#include "button.h"

#include "pageMgr.h"


static void timeAdd(void) {
	if (++time_s > 59) {
		time_s = 0;
		if (++time_m > 59) {
			time_m = 0;
			if (++time_h > 23)
				time_h = 0;
		}
	}
}

// 错误处理
void Error_DealWith(void) {
	LED_Control(2);
	delay_s(2);
	switch (gError) {
		case ERROR_TIME_OUT:
			LED_Control(3);
		case ERROR_Undefine:
			LED_Control(4);
		case ERROR_OVER_FLOW:
			LED_Control(5);
		default:
			break;
	}
	while(1);
}


int main(void) {
	NVIC_Init();
	RCC_Init();
	SysTick_Init();
	GPIO_Init();

	delay_ms(10);
	OLED_Init();

	PageMgr_Init();
	while (1) {
		if (gError != ERROR_NONE) {
			Error_DealWith();
		}
		if (is1000ms > 1) {
			timeAdd();
			is1000ms -= 1;
		}
		Get_Input();
		PageMgr_UpdateButton();
		PageMgr_Loop();
		delay_ms(10);
	}
}

void SystemInit(void) {}
