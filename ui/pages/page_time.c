/* Page Time */
#include "page_time.h"

#include "var.h"
#include "sys.h"
#include "oled.h"
#include "sfrdef.h"

#include "pageMgr.h"
#include "page_main.h"

static __bool __exitPage = False;
static IGeometry IG_TimeLabel;

// sizeof(text)аш>=9
static void getLabel(char *text) {
	text[0] = '0' + (time_h / 10);
	text[1] = '0' + (time_h % 10);
	text[2] = ':';
	text[3] = '0' + (time_m / 10);
	text[4] = '0' + (time_m % 10);
	text[5] = ':';
	text[6] = '0' + (time_s / 10);
	text[7] = '0' + (time_s % 10);
	text[8] = '\0';
}


static void __initFunc(void) {
    IWidget temp_iw = OLED_PrintLabel(
        OLED_SPI, 0, 0, "00:00:00", 8,
        FONT_EN_0816, MODE_NODO, Horizontal, 0
    );
    IG_TimeLabel = temp_iw.geo;

    IG_TimeLabel.posX = (OLED_WIDTH - IG_TimeLabel.sizeX) / 2;
    IG_TimeLabel.posY = (OLED_HEIGHT - IG_TimeLabel.sizeY) / 2;
}

static void __loopFunc(void) {
    for (uint8_t i = 0; i < KEY_COUNT; i++) {
        if (!(btnsGroup[i].pushed && !btnsGroup[i].handled)) continue;
		switch (i) {
            case KEY_CLOSE:
                PageMgr_SwitchTo(&iPage_Main);
                break;
            default: continue;
        }
    }
    if (!__exitPage && displayNeedRefresh) {
        char text[9];
		getLabel(text);
        OLED_PrintLabel(OLED_SPI, IG_TimeLabel.posX, IG_TimeLabel.posY, text, 8, FONT_EN_0816, MODE_CLEAR, Horizontal, 0);
        OLED_PrintLabel(OLED_SPI, IG_TimeLabel.posX, IG_TimeLabel.posY, text, 8, FONT_EN_0816, MODE_NOIID, Horizontal, 0);
        OLED_UpdateArea_SPI1(
            IG_TimeLabel.posX, IG_TimeLabel.posY,
            IG_TimeLabel.posX + IG_TimeLabel.sizeX,
            IG_TimeLabel.posY + IG_TimeLabel.sizeY, 100
        );
        displayNeedRefresh = False;
    }
}

static void __showFunc(void) {
    __exitPage = False;
    displayNeedRefresh = True;
    OLED_Update_SPI1(100);
}

static void __exitFunc(void) {
    __exitPage = True;
}

IPageHandle iPage_Time = {
    .geo = FullScreen,
    .initFunc = __initFunc,
	.loopFunc = __loopFunc,
	.showFunc = __showFunc,
    .exitFunc = __exitFunc
};

