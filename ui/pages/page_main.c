/* Page Menu */
#include "page_main.h"

#include "var.h"
#include "oled.h"
#include "sfrdef.h"
#include "iwidget.h"

#include "pageMgr.h"
#include "page_menu.h"
#include "page_time.h"

#define MENU_POS_X  9
#define MENU_POS_Y  (OLED_HEIGHT - FONT_CN_1616.fH)

#define TIME_POS_X  (OLED_WIDTH  - FONT_CN_1616.fW * 2 - 9)
#define TIME_POS_Y  (OLED_HEIGHT - FONT_CN_1616.fH)


static const CN_Index ci1[2] = {CN_CAI_001, CN_DAN_001};
static const CN_Index ci2[2] = {CN_SHI_001, CN_JIAN_001};
static IButton IB_Menu = {.text = ci1, .len = 2};
static IButton IB_Time = {.text = ci2, .len = 2};

static __bool __exitPage   = False;  // 离开了页面，防止退出页面了但还在loop循环里
static __bool __needUpdate = False;  // 需要更新页面
static IButton *currIb = NULL;       // 当前选中的按钮

static void IB_Main_Menu_Enter(void) { PageMgr_SwitchTo(&iPage_Menu); }
static void IB_Main_Time_Enter(void) { PageMgr_SwitchTo(&iPage_Time); }

/**
  * @brief: 显示按钮文本
  */
static void IB_ShowLabel(IButton *ib) {
	OLED_PrintLabel(
		OLED_SPI, ib->base.geo.posX, ib->base.geo.posY,
		ib->text, ib->len, ib->font, MODE_NOIID, Horizontal, 0
	);
}

/**
  * @brief: 设置括号[]
  */
static void IB_SetBracket(IButton *ib, Draw_Mode mode) {
	IButton temp_ib = *ib;
	
	if (ib->font.lang == Chinese) {
		if (ib->font.fontSize == FONTLIB_CN_1616) {
			temp_ib.font = FONT_EN_0816;
		} else {
			gError = ERROR_Undefine;
			return;
		}
	}	
	OLED_PrintChar(
		OLED_SPI, ib->base.geo.posX - temp_ib.font.fW,
		ib->base.geo.posY, '[', temp_ib.font, mode
	);
	OLED_PrintChar(
		OLED_SPI, ib->base.geo.posX + ib->base.geo.sizeX,
		ib->base.geo.posY, ']', temp_ib.font, mode
	);
}

/**
  * @brief: 移动焦点、光标
  */
static __bool IB_MoveCursor(IButton *ib, KEY_Type key) {
	if (key == KEY_LEFT) {
		if (currIb->idx == 0) return False;
		IB_SetBracket(currIb, MODE_CLEAR);
		currIb = &IB_Menu;
		IB_SetBracket(currIb, MODE_NOIID);
	} else {
		if (currIb->idx == 1) return False;
		IB_SetBracket(currIb, MODE_CLEAR);
		currIb = &IB_Time;
		IB_SetBracket(currIb, MODE_NOIID);
	}
	__needUpdate = True;
	return True;
}

static void __initFunc(void) {
	IWidget temp;
	temp = OLED_PrintLabel(OLED_SPI, MENU_POS_X, MENU_POS_Y, ci1, 2, FONT_CN_1616, MODE_NORMAL, Horizontal, 0);
	IB_Menu.base = temp;
	IB_Menu.idx = 0;
	IB_Menu.font = FONT_CN_1616;
	IB_Menu.enterCall = IB_Main_Menu_Enter;
	temp = OLED_PrintLabel(OLED_SPI, TIME_POS_X, TIME_POS_Y, ci2, 2, FONT_CN_1616, MODE_NORMAL, Horizontal, 0);
	IB_Time.base = temp;
	IB_Time.idx = 1;
	IB_Time.font = FONT_CN_1616;
	IB_Time.enterCall = IB_Main_Time_Enter;

	currIb = &IB_Menu;
	IB_SetBracket(currIb, MODE_NOIID);
	__needUpdate = True;
}

static void __loopFunc(void) {
	for (uint8_t i = 0; i < KEY_COUNT; i++) {
		if (!(btnsGroup[i].pushed && !btnsGroup[i].handled)) continue;
		switch (i) {
			case KEY_LEFT:
				IB_MoveCursor(currIb, KEY_LEFT);
				break;
			case KEY_RIGHT:
				IB_MoveCursor(currIb, KEY_RIGHT);
				break;
			case KEY_ENTER:
				if (currIb->enterCall)
					currIb->enterCall();
				break;
			case KEY_CLOSE:
				if (currIb->leaveCall)
					currIb->leaveCall();
				break;
			default:
				continue;
		}
		btnsGroup[i].handled = True;
	}
	if (!__exitPage && __needUpdate) {
		OLED_Update_SPI1(100);
		__needUpdate = False;
	}
}

static void __showFunc(void) {
	__exitPage = False;
	IB_ShowLabel(&IB_Menu);
	IB_ShowLabel(&IB_Time);
	IB_SetBracket(currIb, MODE_NOIID);
	__needUpdate = True;
}

static void __exitFunc(void) {
	__exitPage = True;
}


IPageHandle iPage_Main = {
	.geo = FullScreen,
	.initFunc = __initFunc,
	.loopFunc = __loopFunc,
	.showFunc = __showFunc,
	.exitFunc = __exitFunc
};
