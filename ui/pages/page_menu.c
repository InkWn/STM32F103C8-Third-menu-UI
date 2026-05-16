/* Page Menu */
#include "page_menu.h"

#include "var.h"
#include "led.h"
#include "sfrdef.h"
#include "menuMgr.h"
#include "page_main.h"

static __bool __exitPage = False;       // 离开菜单页面，防止退出后仍然刷新
static __bool __needUpdate = False;     // 需要更新页面
static IMenuHandle *currImh = NULL;     // 当前菜单头

// 拼接先CN后EN的MixText
#define MT_ENTRY_CE(cn_ci, ascii_str) { \
	{.font = FONT_CN_1616_DEF, .type = MIX_CN, .data.ci = (cn_ci), .len = sizeof(cn_ci) / sizeof(cn_ci[0])}, \
	{.font = FONT_EN_0816_DEF, .type = MIX_ASCII, .data.ascii = (ascii_str), .len = sizeof(ascii_str) - 1}, \
	{.type = MIX_END} \
}

/*================ menu单项 ================*/
static CN_Index ciGroup_choose[2] = {CN_XUAN_001, CN_XIANG_001};   // 选项
static CN_Index ciGroup_enter[2]  = {CN_JIN_001, CN_RU_001};       // 进入
static CN_Index ciGroup_return[2] = {CN_FAN_001, CN_HUI_001};      // 返回
/*------ 一级菜单 ------*/
static IMenuHandle iMh_0 = {.geo = FullScreen, .topIdx = 0, .mh = 16};
#define ITEM0_CNT  8
static IMenuItem item_0_1, item_0_2, item_0_3, item_0_4, item_0_5, item_0_6, item_0_7, item_0_8;
static MixText itemText0[ITEM0_CNT][3] = {  // 这个3指，中文+英文+END，若再插入中文（中文+英文+中文+END），则为4
	[0] = {{.font = FONT_EN_0816_DEF, .data.ascii = "This is Lv1", .type = MIX_ASCII, .len = 11 }, {.type = MIX_END}},
	[1] = MT_ENTRY_CE(ciGroup_enter, "Lv2"),
	[2] = MT_ENTRY_CE(ciGroup_enter, "Lv3"),
	[3] = {{.font = FONT_EN_0816_DEF, .data.ascii = "On LED", .type = MIX_ASCII, .len = 6 }, {.type = MIX_END}},
	[4] = {{.font = FONT_EN_0816_DEF, .data.ascii = "Off LED", .type = MIX_ASCII, .len = 7 }, {.type = MIX_END}},
	[5] = MT_ENTRY_CE(ciGroup_choose, "1-6"),
	[6] = MT_ENTRY_CE(ciGroup_choose, "1-7"),
	[7] = MT_ENTRY_CE(ciGroup_choose, "1-8"),
};
/*------ 二级菜单 ------*/
static IMenuHandle iMh_1 = {.geo = FullScreen, .topIdx = 0, .mh = 16};
#define ITEM1_CNT  4
static IMenuItem item_1_1, item_1_2, item_1_3, item_1_4;
static MixText itemText1[ITEM1_CNT][3] = {
	[0] = {{.font = FONT_EN_0816_DEF, .data.ascii = "This is Lv2", .type = MIX_ASCII, .len = 11 }, {.type = MIX_END}},
	[1] = MT_ENTRY_CE(ciGroup_enter, "Lv3"),
	[2] = MT_ENTRY_CE(ciGroup_return, "Lv1"),
	[3] = MT_ENTRY_CE(ciGroup_choose, "2-4"),
};
/*------ 三级菜单 ------*/
static IMenuHandle iMh_2 = {.geo = FullScreen, .topIdx = 0, .mh = 16};
#define ITEM2_CNT  3
static IMenuItem item_2_1, item_2_2, item_2_3;
static MixText itemText2[ITEM2_CNT][3] = {
	[0] = {{.font = FONT_EN_0816_DEF, .data.ascii = "This is Lv3", .type = MIX_ASCII, .len = 11 }, {.type = MIX_END}},
	[1] = MT_ENTRY_CE(ciGroup_return, "Lv2"),
	[2] = MT_ENTRY_CE(ciGroup_return, "main"),
};
/*------ 初始化函数 ------*/ 
static void Menu_0_Init(void) {
	IMenuItem *items[] = {
		&item_0_1, &item_0_2, &item_0_3, &item_0_4,
		&item_0_5, &item_0_6, &item_0_7, &item_0_8
	};

	for (uint8_t i = 0; i < ITEM0_CNT; i++) {
		items[i]->text = itemText0[i];
		Menu_AddMenuItem(&iMh_0, items[i]);
	}
}
static void Menu_1_Init(void) {
	IMenuItem *items[] = {
		&item_1_1, &item_1_2, &item_1_3, &item_1_4,
	};

	for (uint8_t i = 0; i < ITEM1_CNT; i++) {
		items[i]->text = itemText1[i];
		Menu_AddMenuItem(&iMh_1, items[i]);
	}
}
static void Menu_2_Init(void) {
	IMenuItem *items[] = {
		&item_2_1, &item_2_2, &item_2_3,
	};

	for (uint8_t i = 0; i < ITEM2_CNT; i++) {
		items[i]->text = itemText2[i];
		Menu_AddMenuItem(&iMh_2, items[i]);
	}
}
/*------ 定义enter函数 ------*/
static void __item_0_2_enter(void) { Menu_SetIsTurnPage(True); currImh = &iMh_1; __needUpdate = True; }
static void __item_0_3_enter(void) { Menu_SetIsTurnPage(True); currImh = &iMh_2; __needUpdate = True; }
static void __item_0_4_enter(void) { LED_Control(1); }
static void __item_0_5_enter(void) { LED_Control(0); }

static void __item_1_2_enter(void) { Menu_SetIsTurnPage(True); currImh = &iMh_2; __needUpdate = True; }
static void __item_1_3_enter(void) { Menu_SetIsTurnPage(True); currImh = &iMh_0; __needUpdate = True; }

static void __item_2_2_enter(void) { Menu_SetIsTurnPage(True); currImh = &iMh_1; __needUpdate = True; }
static void __item_2_3_enter(void) { PageMgr_SwitchTo(&iPage_Main); }
/*------ 定义leave函数 ------*/ 
static void __item_0_x_leave(void) { PageMgr_SwitchTo(&iPage_Main); }
static void __item_1_x_leave(void) { Menu_SetIsTurnPage(True); currImh = &iMh_0; __needUpdate = True; }
static void __item_2_x_leave(void) { Menu_SetIsTurnPage(True); currImh = &iMh_1; __needUpdate = True; }
/*------ 绑定函数-1 ------*/ 
static void Menu_0_BindFunc(void) {
	// enter
	item_0_2.enterCall = __item_0_2_enter;
	item_0_3.enterCall = __item_0_3_enter;
	item_0_4.enterCall = __item_0_4_enter;
	item_0_5.enterCall = __item_0_5_enter;
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
	item_1_2.enterCall = __item_1_2_enter;
	item_1_3.enterCall = __item_1_3_enter;
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
	item_2_2.enterCall = __item_2_2_enter;
	item_2_3.enterCall = __item_2_3_enter;
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

#define Reset_iMh(iMh, firstItem) {\
	iMh.curIdx = 0;\
	iMh.topIdx = 0;\
	iMh.select = &firstItem;\
}

static void __showFunc(void) {
	__exitPage = False;
	currImh = &iMh_0;
	Reset_iMh(iMh_0, item_0_1);
	Reset_iMh(iMh_1, item_1_1);
	Reset_iMh(iMh_2, item_2_1);
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
