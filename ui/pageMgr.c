#include "pageMgr.h"

#include "var.h"
#include "oled.h"

#include "page_main.h"
#include "page_menu.h"
#include "page_time.h"
// 根据上面include page的数量来填写这个值
#define Page_Nums  3
// 全部页面
static IPageHandle * const pages[Page_Nums] = {
    &iPage_Main,
    &iPage_Menu,
    &iPage_Time,
};
static IPageHandle *currPage;

static const char* t1 = "PA8:  LEFT\nPA9:  RIGHT\nPA10: TOP\nPA11: BOTTOM\nPA12: ENTER\nPA15: CLOSE\0";
static const char* t2 = "Author\nInkWn\n \nDate\n20260428\0";

#define btn_spacing 4
static IGeometry IG_Btn;

void PageMgr_Init(void) {
    /*================ I2C ================*/
    OLED_PrintList(OLED_I2C, 2, 16, t1, 12, 6, FONT_EN_0508, 1, 0);
	OLED_DrawLine(OLED_I2C, 75, 16, 75, OLED_HEIGHT - 1, MODE_NOIID);
	OLED_PrintList(OLED_I2C, 79, 16, t2, 8, 5, FONT_EN_0508, 1, 0);
    IWidget temp_w = {0};
    uint8_t x = 0;
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        temp_w = OLED_PrintChar(OLED_I2C, x, 0, '0', FONT_EN_0508, MODE_NOIID);
        x += temp_w.geo.sizeX + btn_spacing;
		if (x >= OLED_WIDTH) break;
    }
	IG_Btn = temp_w.geo;
    /*================ SPI ================*/
    for (uint8_t i = 0; i < Page_Nums; i++) {
        pages[i]->initFunc();
    }
	currPage = &iPage_Main;
    // 仅更新提示屏幕
    OLED_Update_I2C1(100);
}

/**
  * @brief: 切换页面
  */
void PageMgr_SwitchTo(IPageHandle *iPageH) {
    if (currPage->exitFunc) { currPage->exitFunc(); }
    OLED_ControlScreen(OLED_SPI, 0);
    iPageH->showFunc();
    currPage = iPageH;
}

void PageMgr_Loop(void) {
    if (currPage->loopFunc)
        currPage->loopFunc();
    else {
        gError = ERROR_Undefine;
        return;
    }
}

static Button_TypeDef btnsGroup_temp[KEY_COUNT];
static __bool btnNeedUpdate = False;

__bool PageMgr_UpdateButton(void) {
	uint8_t x = 0;
	for (uint8_t i = 0; i < KEY_COUNT; i++) {
        if (btnsGroup[i].pushed != btnsGroup_temp[i].pushed) {
            OLED_PrintChar(OLED_I2C, x, 0, 'x', FONT_EN_0508, MODE_CLEAR);  // 清空
            if (btnsGroup[i].pushed) OLED_PrintChar(OLED_I2C, x, 0, '1', FONT_EN_0508, MODE_NOIID);
            else                     OLED_PrintChar(OLED_I2C, x, 0, '0', FONT_EN_0508, MODE_NOIID);
            btnNeedUpdate = True;
            btnsGroup_temp[i].pushed = btnsGroup[i].pushed;
        }
		x += IG_Btn.sizeX + btn_spacing;
		if (x >= OLED_WIDTH) return False;
	}
    if (btnNeedUpdate) 
	    OLED_UpdateArea_I2C1(0, 0, x - btn_spacing, IG_Btn.sizeY, 100);
	return True;
}
