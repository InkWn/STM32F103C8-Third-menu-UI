/*******
iid系统暂时有问题，不能使用，不过本程序也用不到

本程序暂不支持在运行时动态增删菜单单项

项目地址：https://github.com/InkWn/STM32F103C8-Third-menu-UI
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
		case ERROR_I2C_ADDR_NACK:
			LED_Control(2); break;
		case ERROR_TIME_OUT:
			LED_Control(3); break;
		case ERROR_UNDEFINE:
			LED_Control(4); break;
		case ERROR_OVER_FLOW:
			LED_Control(5); break;
		default:
			break;
	}
	while(1) {
		if (isNeedSysNBCallback) {
			v_vCallBack cb = sysNB.cb;
			sysNB.cb = 0;
			if (cb) cb();
			isNeedSysNBCallback = False;
		} 
	}
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
		if (timeNeedUpdate) {
			timeNeedUpdate = False;
			timeAdd();
			displayNeedRefresh = True;
		}
		PageMgr_UpdateButton();
		PageMgr_Loop();
		if (isNeedSysNBCallback) {
			v_vCallBack cb = sysNB.cb;
			sysNB.cb = 0;
			if (cb) cb();
			isNeedSysNBCallback = False;
		}
		Button_Input_Check();
		delay_ms(10);
		Button_Input_Confirm();
	}
}

void SystemInit(void) {}
