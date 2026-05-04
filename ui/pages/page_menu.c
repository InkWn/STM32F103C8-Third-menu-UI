/* Page Menu */
#include "page_menu.h"

#include "var.h"
#include "sfrdef.h"
#include "menuMgr.h"
#include "page_main.h"

static __bool __exitPage = False;       // 离开菜单页面，防止退出后仍然刷新
static __bool __needUpdate = False;       // 需要更新页面
static IMenuHandle *currImh = NULL;     // 当前菜单头

/*================ menu单项 ================*/
/*----- 一些定义 -----*/
#define FONT_CN      {FONTLIB_CN_1616, Chinese, 16, 16}  // 全局变量不给直接用FONT_EN_0508，所以定义一个
#define FONT_EN      {FONTLIB_EN_0816, English, 8, 16}

static const CN_Index ci_x_1[3] = {CN_XUAN_001, CN_XIANG_001, CN_YI_001};
static const CN_Index ci_0_2[3] = {CN_XUAN_001, CN_XIANG_001, CN_ER_001};
static const CN_Index ci_0_3[3] = {CN_XUAN_001, CN_XIANG_001, CN_SAN_001};
static const CN_Index ci_0_4[3] = {CN_XUAN_001, CN_XIANG_001, CN_SI_001};
static const CN_Index ci_0_5[3] = {CN_XUAN_001, CN_XIANG_001, CN_WU_001};
static const CN_Index ci_2_1[2] = {CN_FAN_001, CN_HUI_001};
/*------ 一级菜单 ------*/
static IMenuHandle iMh_0 = {.geo = FullScreen, .topIdx = 0, .mh = 16};
static IMenuItem item_0_1 = {.font = FONT_CN, .text = ci_x_1, .len = 3};
static IMenuItem item_0_2 = {.font = FONT_CN, .text = ci_0_2, .len = 3};
static IMenuItem item_0_3 = {.font = FONT_CN, .text = ci_0_3, .len = 3};
static IMenuItem item_0_4 = {.font = FONT_CN, .text = ci_0_4, .len = 3};
static IMenuItem item_0_5 = {.font = FONT_CN, .text = ci_0_5, .len = 3};
static IMenuItem item_0_6 = {.font = FONT_EN, .text = "0-6", .len = 3};
static IMenuItem item_0_7 = {.font = FONT_EN, .text = "0-7", .len = 3};
/*------ 二级菜单 ------*/
static IMenuHandle iMh_1 = {.geo = FullScreen, .topIdx = 0, .mh = 16};
static IMenuItem item_1_1 = {.font = FONT_CN, .text = ci_x_1, .len = 3};
static IMenuItem item_1_2 = {.font = FONT_EN, .text = "1-2", .len = 3};
/*------ 三级菜单 ------*/
static IMenuHandle iMh_2 = {.geo = FullScreen, .topIdx = 0, .mh = 16};
static IMenuItem item_2_1 = {.font = FONT_CN, .text = ci_2_1, .len = 2};
/*------ 初始化函数 ------*/ 
static void Menu_0_Init(void) {
	Menu_AddMenuItem(&iMh_0, &item_0_1);
	Menu_AddMenuItem(&iMh_0, &item_0_2);
	Menu_AddMenuItem(&iMh_0, &item_0_3);
	Menu_AddMenuItem(&iMh_0, &item_0_4);
	Menu_AddMenuItem(&iMh_0, &item_0_5);
	Menu_AddMenuItem(&iMh_0, &item_0_6);
	Menu_AddMenuItem(&iMh_0, &item_0_7);
}
static void Menu_1_Init(void) {
	Menu_AddMenuItem(&iMh_1, &item_1_1);
	Menu_AddMenuItem(&iMh_1, &item_1_2);
}
static void Menu_2_Init(void) {
	Menu_AddMenuItem(&iMh_2, &item_2_1);
}
/*------ 定义enter函数 ------*/
static void __item_0_1_enter(void) { Menu_SetIsTurnPage(True); currImh = &iMh_1; __needUpdate = True; }
static void __item_1_1_enter(void) { Menu_SetIsTurnPage(True); currImh = &iMh_2; __needUpdate = True; }
static void __item_2_1_enter(void) { Menu_SetIsTurnPage(True); currImh = &iMh_1; __needUpdate = True; }
/*------ 定义leave函数 ------*/ 
static void __item_0_x_leave(void) { PageMgr_SwitchTo(&iPage_Main); }
static void __item_1_x_leave(void) { Menu_SetIsTurnPage(True); currImh = &iMh_0; __needUpdate = True; }
static void __item_2_x_leave(void) { Menu_SetIsTurnPage(True); currImh = &iMh_1; __needUpdate = True; }
/*------ 绑定函数-1 ------*/ 
static void Menu_0_BindFunc(void) {
	// enter
	item_0_1.enterCall = __item_0_1_enter;
	// leave
	IMenuItem *p = iMh_0.begin;
	while (p) {
		p->leaveCall = __item_0_x_leave;
		p = p->next;
	}
}
/*------ 绑定函数-2 ------*/ 
static void Menu_1_BindFunc(void) {
	// enter
	item_1_1.enterCall = __item_1_1_enter;
	// leave
	IMenuItem *p = iMh_1.begin;
	while (p) {
		p->leaveCall = __item_1_x_leave;
		p = p->next;
	}
}
/*------ 绑定函数-3 ------*/ 
static void Menu_2_BindFunc(void) {
	// enter
	item_2_1.enterCall = __item_2_1_enter;
	// leave
	IMenuItem *p = iMh_2.begin;
	while (p) {
		p->leaveCall = __item_2_x_leave;
		p = p->next;
	}
}
/*================ 页面管理 ================*/
static void __initFunc(void) {
	Menu_0_Init();
	Menu_1_Init();
	Menu_2_Init();
	Menu_0_BindFunc();
	Menu_1_BindFunc();
	Menu_2_BindFunc();
}

static void __loopFunc(void) {
	for (uint8_t i = 0; i < KEY_COUNT; i++) {
		if (!(btnsGroup[i].pushed && !btnsGroup[i].handled)) continue;
		switch (i) {
			case KEY_UP:
				Menu_MoveCursor(currImh, KEY_UP);
				__needUpdate = True;
				break;
			case KEY_DOWN:
				Menu_MoveCursor(currImh, KEY_DOWN);
				__needUpdate = True;
				break;
			case KEY_ENTER:
				if (currImh->select->enterCall) {
					currImh->select->enterCall();
					__needUpdate = True;
				}
				break;
			case KEY_CLOSE:
				if (currImh->select->leaveCall) {
					currImh->select->leaveCall();
					//
					__needUpdate = True;
				}
				break;
			default: continue;
		}
		btnsGroup[i].handled = True;
	}
	if (!__exitPage && __needUpdate) {
		Menu_ShowAll(currImh);
		__needUpdate = False;
	}
}

static void __showFunc(void) {
	__exitPage = False;
	currImh = &iMh_0;
	Menu_ShowAll(currImh);
}

static void __exitFunc(void) {
	__exitPage = True;
}

IPageHandle iPage_Menu = {
	.geo = FullScreen,
	.initFunc = __initFunc,
	.loopFunc = __loopFunc,
	.showFunc = __showFunc,
	.exitFunc = __exitFunc
};
