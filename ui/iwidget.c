#include "iwidget.h"

#include "var.h"
#include "oled.h"
#include "delay.h"
#include "Rstring.h"

#if 0
// 下划线版本
static IGeometry IW_SetUnderline(IWidget *iw, Draw_Mode mode) {
	IGeometry temp_i = {
		.posX = iw->geo.posX,
		.posY = iw->geo.posY + iw->geo.sizeY + 1,
		.sizeX = iw->geo.posX + iw->geo.sizeX,
		.sizeY = iw->geo.posY + iw->geo.sizeY + 1,
	};
	OLED_DrawLine(OLED_SPI, temp_i.posX, temp_i.posY, temp_i.sizeX, temp_i.sizeY, mode);
	return temp_i;
}
#endif

/**
  * @brief: 在最右侧设置左箭头，不刷新，得手动刷新
  */
static IWidget IW_SetArrow(IMenuItem *iw, Draw_Mode mode) {
	IMenuItem temp_iw = *iw;
	
	if (iw->font.lang == Chinese) {
		if (iw->font.fontSize == FONTLIB_CN_1616) {
			temp_iw.font = FONT_EN_0816;
		} else {
			gError = ERROR_Undefine;
		}
	}	
	return OLED_PrintChar(
		OLED_SPI,
		iw->base.geo.posX + iw->base.geo.sizeX - temp_iw.font.fW * 2,
		iw->viewY, '<', temp_iw.font, mode
	);
}

void IW_DrawSelected(IMenuItem *iw) { IW_SetArrow(iw, MODE_NOIID); }
void IW_DrawNoSelect(IMenuItem *iw) { IW_SetArrow(iw, MODE_CLEAR); }


void IW_DrawClicked(IMenuItem *iw) {
	// 点击时的实现动画，可以考虑要不要，反正点击后就进入下一级菜单了
	OLED_InvertRegion(
		OLED_SPI, iw->base.geo.posX, iw->base.geo.posY,
		iw->base.geo.posX + iw->base.geo.sizeX, iw->base.geo.posY + iw->base.geo.sizeY
	);
	delay_ms(100);
	OLED_InvertRegion(
		OLED_SPI, iw->base.geo.posX, iw->base.geo.posY,
		iw->base.geo.posX + iw->base.geo.sizeX, iw->base.geo.posY + iw->base.geo.sizeY
	);
}
