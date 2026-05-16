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
static IWidget IW_SetArrow(IMenuItem *im, Draw_Mode mode) {
	Font f = FONT_EN_0508;  // 默认

	for (MixText *seg = im->text; seg->type != MIX_END; seg++) {
		if (seg->font.fH > f.fH) {
			if (seg->font.lang == Chinese) {
				if (seg->font.fontSize == FONTLIB_CN_1616) {
					f = FONT_EN_0816;
				} else { gError = ERROR_UNDEFINE; }
			} else { f = seg->font; }
		}
	}

	return OLED_PrintChar(
		OLED_SPI,
		im->base.geo.posX + im->base.geo.sizeX - f.fW * 2,
		im->viewY, '<', f, mode
	);
}

void IW_DrawSelected(IWidget *iw) { IW_SetArrow((IMenuItem*)iw, MODE_NOIID); }
void IW_DrawNoSelect(IWidget *iw) { IW_SetArrow((IMenuItem*)iw, MODE_CLEAR); }


#if 0
void IW_DrawClicked(IWidget *iw) {
	// 点击时的实现动画，可以考虑要不要，反正点击后就进入下一级菜单了
	OLED_InvertRegion(
		OLED_SPI, iw->geo.posX, iw->geo.posY,
		iw->geo.posX + iw->geo.sizeX, iw->geo.posY + iw->geo.sizeY
	);
	delay_ms(100);
	OLED_InvertRegion(
		OLED_SPI, iw->geo.posX, iw->geo.posY,
		iw->geo.posX + iw->geo.sizeX, iw->geo.posY + iw->geo.sizeY
	);
}
#endif
