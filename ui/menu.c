#include "menu.h"

#include "ui.h"
#include "oled.h"
#include "sfrdef.h"
#include "Rstring.h"

#define FONT { FONTLIB_EN_0816, English, 8, 16 }

/*================ 一级 ================*/
IMenuHandle iMh_0 = {
	.geo.posX = 0, .geo.posY = 0,
	.geo.sizeX = OLED_WIDTH,
	.geo.sizeY = OLED_HEIGHT,
	.topIdx = 0,
	.mh = 16
};

IMenuItem item_0_1 = {
	.font = FONT,
	.text = "enter2",
	.len = 6,
};
IMenuItem item_0_2 = {
	.font = FONT,
	.text = "0-2",
	.len = 3,
};
IMenuItem item_0_3 = {
	.font = FONT,
	.text = "0-3",
	.len = 3,
};
IMenuItem item_0_4 = {
	.font = FONT,
	.text = "0-4",
	.len = 3,
};
IMenuItem item_0_5 = {
	.font = FONT,
	.text = "0-5",
	.len = 3,
};
IMenuItem item_0_6 = {
	.font = FONT,
	.text = "No-1",
	.len = 4,
};
IMenuItem item_0_7 = {
	.font = FONT,
	.text = "No-2",
	.len = 4,
};
/*================ 二级 ================*/
IMenuHandle iMh_1 = {
	.geo.posX = 0, .geo.posY = 0,
	.geo.sizeX = OLED_WIDTH,
	.geo.sizeY = OLED_HEIGHT,
	.topIdx = 0,
	.mh = 16	
};
IMenuItem item_1_1 = {
	.font = FONT,
	.text = "enter3",
	.len = 6,
};
IMenuItem item_1_2 = {
	.font = FONT,
	.text = "1-2",
	.len = 3,
};
IMenuItem item_1_3 = {
	.font = FONT,
	.text = "1-3",
	.len = 3,
};
/*================ 三级 ================*/
IMenuHandle iMh_2 = {
	.geo.posX = 0, .geo.posY = 0,
	.geo.sizeX = OLED_WIDTH,
	.geo.sizeY = OLED_HEIGHT,
	.topIdx = 0,
	.mh = 16	
};
IMenuItem item_2_1 = {
	.font = FONT,
	.text = "return",
	.len = 6,
};
/*================ 结束 ================*/
void Menu_0_Init(void) {
	Menu_AddMenuItem(&iMh_0, &item_0_1);
	Menu_AddMenuItem(&iMh_0, &item_0_2);
	Menu_AddMenuItem(&iMh_0, &item_0_3);
	Menu_AddMenuItem(&iMh_0, &item_0_4);
	Menu_AddMenuItem(&iMh_0, &item_0_5);
	Menu_AddMenuItem(&iMh_0, &item_0_6);
	Menu_AddMenuItem(&iMh_0, &item_0_7);
}

void Menu_1_Init(void) {
	Menu_AddMenuItem(&iMh_1, &item_1_1);
	Menu_AddMenuItem(&iMh_1, &item_1_2);
	Menu_AddMenuItem(&iMh_1, &item_1_3);
}

void Menu_2_Init(void) {
	Menu_AddMenuItem(&iMh_2, &item_2_1);
}

/*================ 定义enter函数 ================*/
static void __item_0_1_enter(void) {
	OLED_ControlScreen(OLED_SPI, 0);
	UI_SetCurrImh(&iMh_1);
}
static void __item_1_1_enter(void) {
	OLED_ControlScreen(OLED_SPI, 0);
	UI_SetCurrImh(&iMh_2);
}
static void __item_2_1_enter(void) {
	OLED_ControlScreen(OLED_SPI, 0);
	UI_SetCurrImh(&iMh_1);
}
/*================ 定义leave函数 ================*/
#include "led.h"
static void __item_1_x_leave(void) {
	LED_Control(2);
	OLED_ControlScreen(OLED_SPI, 0);
	UI_SetCurrImh(&iMh_0);
}
static void __item_2_x_leave(void) {
	LED_Control(2);
	OLED_ControlScreen(OLED_SPI, 0);
	UI_SetCurrImh(&iMh_1);
}
/*================ 绑定函数-1 ================*/
void Menu_0_BindFunc(void) {
	// enter
	item_0_1.enterCall = __item_0_1_enter;
	// leave
	IMenuItem *p = iMh_0.begin;
	while (p) {
		p->leaveCall = UI_ReturnMainUI;
		p = p->next;
	}
}
/*================ 绑定函数-2 ================*/
void Menu_1_BindFunc(void) {
	// enter
	item_1_1.enterCall = __item_1_1_enter;
	// leave
	IMenuItem *p = iMh_1.begin;
	while (p) {
		p->leaveCall = __item_1_x_leave;
		p = p->next;
	}
}
/*================ 绑定函数-3 ================*/
void Menu_2_BindFunc(void) {
	// enter
	item_2_1.enterCall = __item_2_1_enter;
	// leave
	IMenuItem *p = iMh_2.begin;
	while (p) {
		p->leaveCall = __item_2_x_leave;
		p = p->next;
	}
}
